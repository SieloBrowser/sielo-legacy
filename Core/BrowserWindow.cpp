/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2018 Victor DENIS (victordenis01@gmail.com)                      **
**                                                                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
***********************************************************************************/

#include "BrowserWindow.hpp"

#include <QToolTip>
#include <QStatusBar>

#include <QList>

#include <QAction>

#include <QClipboard>

#include <QTimer>
#include <QMessageBox>

#include "Bookmarks/BookmarksUtils.hpp"

#include "MaquetteGrid/MaquetteGridItem.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Utils/DataPaths.hpp"
#include "Utils/RestoreManager.hpp"
#include "Utils/Settings.hpp"

#include "Web/LoadRequest.hpp"
#include "Web/WebPage.hpp"
#include "Web/Tab/WebTab.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/TitleBar.hpp"
#include "Widgets/AddressBar/AddressBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"
#include "Widgets/NavigationControlDialog.hpp"

#ifdef Q_OS_WIN
#include <windowsx.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <windows.h>

#if QT_VERSION == QT_VERSION_CHECK(5, 11, 0) || QT_VERSION == QT_VERSION_CHECK(5, 11, 1)
#error "The custom window don't work with Qt 5.11.0 or Qt 5.11.1"
#endif

#endif

QT_BEGIN_NAMESPACE
extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

#ifdef Q_OS_WIN
bool DWMEnabled(void)
{
	if (QSysInfo::windowsVersion() < QSysInfo::WV_VISTA) return false;
	BOOL useDWM;

	if (DwmIsCompositionEnabled(&useDWM) < 0) return false;
	return useDWM == TRUE;
}

void adjust_maximized_client_rect(HWND window, RECT& rect)
{
	WINDOWPLACEMENT placement;
	if (!GetWindowPlacement(window, &placement) || placement.showCmd != SW_MAXIMIZE)
		return;

	auto monitor = ::MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
	if (!monitor)
		return;

	MONITORINFO monitor_info{};
	monitor_info.cbSize = sizeof(monitor_info);
	if (!::GetMonitorInfoW(monitor, &monitor_info))
		return;

	// when maximized, make the client area fill just the monitor (without task bar) rect,
	// not the whole window rect which extends beyond the monitor.
	rect = monitor_info.rcWork;
}
#endif

namespace Sn
{
BrowserWindow::SavedWindow::SavedWindow()
{
	// Empty
}

BrowserWindow::SavedWindow::SavedWindow(BrowserWindow* window)
{
	windowState = window->isFullScreen() ? QByteArray() : window->saveState();
	windowGeometry = window->saveGeometry();

	const int tabsSpaceCount{window->tabsSpaceSplitter()->count()};
	tabsSpaces.reserve(tabsSpaceCount);

	for (int i{0}; i < tabsSpaceCount; ++i) {
		TabsSpaceSplitter::SavedTabsSpace tabsSpace{window->tabsSpaceSplitter(), window->tabsSpaceSplitter()->tabWidget(i)};
		if (!tabsSpace.isValid())
			continue;

		tabsSpaces.append(tabsSpace);
	}
}

BrowserWindow::SavedWindow::SavedWindow(MaquetteGridItem* maquetteGridItem)
{
	windowState = QByteArray();
	windowGeometry = QByteArray();

	const int tabsSpaceCount{maquetteGridItem->tabsSpaces().count()};
	tabsSpaces.reserve(tabsSpaceCount);

	for (int i{0}; i < tabsSpaceCount; ++i) {
		TabsSpaceSplitter::SavedTabsSpace tabsSpace = maquetteGridItem->tabsSpaces()[i];
		if (!tabsSpace.isValid())
			continue;

		tabsSpaces.append(tabsSpace);
	}
}

bool BrowserWindow::SavedWindow::isValid() const
{
	for (const TabsSpaceSplitter::SavedTabsSpace& tabsSpace : tabsSpaces) {
		if (!tabsSpace.isValid())
			return false;
	}

	return true;
}

void BrowserWindow::SavedWindow::clear()
{
	windowState.clear();
	windowGeometry.clear();
	tabsSpaces.clear();
}

QDataStream &operator<<(QDataStream &stream, const BrowserWindow::SavedWindow &window)
{
	stream << 1;
	stream << window.windowState;
	stream << window.windowGeometry;
	stream << window.tabsSpaces.count();

	for (int i{0}; i < window.tabsSpaces.count(); ++i)
		stream << window.tabsSpaces[i];

	return stream;
}

QDataStream &operator>>(QDataStream &stream, BrowserWindow::SavedWindow &window)
{
	int version{0};
	stream >> version;

	if (version < 1)
		return stream;

	stream >> window.windowState;
	stream >> window.windowGeometry;

	int tabsSpacesCount{-1};
	stream >> tabsSpacesCount;
	window.tabsSpaces.reserve(tabsSpacesCount);

	for (int i{0}; i < tabsSpacesCount; ++i) {
		TabsSpaceSplitter::SavedTabsSpace tabsSpace{};
		stream >> tabsSpace;
		window.tabsSpaces.append(tabsSpace);
	}

	return stream;
}

BrowserWindow::BrowserWindow(Application::WindowType type, const QUrl& url) :
	QMainWindow(nullptr),
	m_startUrl(url),
	m_windowType(type),
	m_backgroundTimer(new QTimer())
{
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_DontCreateNativeAncestors);
	setAcceptDrops(true);
	setMouseTracking(true);

#ifdef Q_OS_WIN
	setWindowFlags(Qt::FramelessWindowHint);
	SetWindowLongPtrW(reinterpret_cast<HWND>(winId()), GWL_STYLE, WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
	const MARGINS margins = {1,1,1,1};
	DwmExtendFrameIntoClientArea(reinterpret_cast<HWND>(winId()), &margins);
#endif

	setObjectName(QLatin1String("mainwindow"));
	setWindowTitle(tr("Sielo"));
	setProperty("private", Application::instance()->privateBrowsing());

	statusBar()->hide(); // Since we have a custom status bar, we hide the default one.

	setupUi();

	loadSettings();

	connect(m_backgroundTimer, &QTimer::timeout, this, &BrowserWindow::loadWallpaperSettings);
	m_backgroundTimer->start(1000);

	// Just wait some milli seconds before doing some post launch action
	QTimer::singleShot(10, this, &BrowserWindow::postLaunch);
}

BrowserWindow::~BrowserWindow()
{
	Application::instance()->plugins()->emitMainWindowDeleted(this);
}

void BrowserWindow::loadSettings()
{
	Settings settings{};

	m_homePage = settings.value(QLatin1String("Web-Settings/homePage"), QUrl("https://doosearch.sielo.app/")).toUrl();

	m_blur_radius = settings.value(QLatin1String("Settings/backdropBlur"), 100).toInt();

	// There is two possibility: the user use the floating button or not. 
	// Despite the floating button belongs to the window, the navigation bar belongs to the tab widget
	if (m_tabsSpaceSplitter->count() > 0) {
		if (!Application::instance()->useTopToolBar() && !m_fButton)
			setupFloatingButton();
		else if (Application::instance()->useTopToolBar() && m_fButton) {
			delete m_fButton;
			m_fButton = nullptr;
		}
	}

	m_tabsSpaceSplitter->loadSettings();

	loadWallpaperSettings();

	bool showBookmarksToolBar = settings.value(QLatin1String("ShowBookmarksToolBar"), true).toBool();
	m_titleBar->setShowBookmark(showBookmarksToolBar);
}

void BrowserWindow::loadWallpaperSettings()
{
	if (!m_backgroundTimer->isActive()) 
		m_backgroundTimer->start(100);

	Settings settings{};

	QString backgroundPath = settings.value(QLatin1String("Settings/backgroundPath"), QString()).toString();

#ifdef Q_OS_WIN 
	QSettings wallpaperSettings{"HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat};
	QString wallpaper{wallpaperSettings.value("WallPaper", QString()).toString()};
	wallpaper.replace("\\", "/");

		if (backgroundPath.isEmpty())
			backgroundPath = wallpaper;
#endif


	QImage newBackground{backgroundPath};
	
	// Themes can have default backgound. If the user don't have custom background, we apply it.
	// However, if the user have a custom background we override the default one
	if (!backgroundPath.isEmpty() && newBackground != m_currentBackground) {
		QString sss{};
		sss += "QMainWindow {";
		sss += "border-image: url(" + backgroundPath + ") 0 0 0 0 stretch stretch;";
		sss += "border-width: 0px;";

		//if (settings.value(QLatin1String("Settings/repeatBackground"), false).toBool())
		//	sss += "background-repeat: repeat;";
		//else
		//	sss += "background-repeat: no-repeat;";

		sss += "}";

		setStyleSheet(sss);

		m_currentBackground = newBackground;
		m_upd_ss = true; // Citorva will explain this.
	}
}

void BrowserWindow::setStartTab(WebTab* tab)
{
	m_startTab = tab;
}

void BrowserWindow::setStartPage(WebPage* page)
{
	m_startPage = page;
}

void BrowserWindow::restoreWindowState(const SavedWindow& window)
{
	// Restore basics informations
	restoreState(window.windowState);
	restoreGeometry(window.windowGeometry);

	for (int i{0}; i < window.tabsSpaces.count(); ++i)
		m_tabsSpaceSplitter->restoreTabsSpace(window.tabsSpaces[i]);

	m_tabsSpaceSplitter->autoResize();

	loadSettings();
}

void BrowserWindow::currentTabChanged(WebTab* tab)
{
	if (!tab || !tab->webView())
		return;

	setWindowTitle(tr("%1 - Sielo").arg(tab->title()));

	tab->webView()->setFocus();
}

void BrowserWindow::loadUrl(const QUrl& url)
{
	// We can't load url directly in pinned tabs
	if (tabWidget()->webTab()->isPinned()) {
		int index{tabWidget()->addView(url, Application::NTT_CleanSelectedTab)};
		tabWidget()->webTab(index)->webView()->setFocus();
	}
	else {
		tabWidget()->webTab()->webView()->setFocus();
		tabWidget()->webTab()->webView()->load(url);
	}
}

void BrowserWindow::loadUrlInNewTab(const QUrl& url)
{
	tabWidget()->addView(url);
}

TabWidget *BrowserWindow::tabWidget() const
{
	return m_tabsSpaceSplitter->tabWidget();
}

TabWidget *BrowserWindow::tabWidget(int index) const
{
	return m_tabsSpaceSplitter->tabWidget(index);
}

const QImage *BrowserWindow::background()
{
	return m_bg;
}

const QImage *BrowserWindow::processedBackground()
{
	return m_blur_bg;
}

void BrowserWindow::setWindowTitle(const QString& title)
{
	QString t{title};

	// We wan't to be sure to have an indication when the window is private
	if (Application::instance()->privateBrowsing())
		t.append(tr(" (Private Browsing)"));

	if (m_titleBar)
		m_titleBar->setTitle(t);

	QMainWindow::setWindowTitle(t);
}

void BrowserWindow::enterHtmlFullScreen()
{
	// Empty
}

void BrowserWindow::toggleFullScreen()
{
	if (isFullScreen())
		showNormal();
	else
		showFullScreen();
}

void BrowserWindow::bookmarkPage()
{
	TabbedWebView* view{tabWidget()->webTab()->webView()};

	BookmarksUtils::addBookmarkDialog(this, view->url(), view->title());
}

void BrowserWindow::bookmarkAllTabs()
{
	BookmarksUtils::bookmarkAllTabsDialog(this, tabWidget());
}

void BrowserWindow::addBookmark(const QUrl& url, const QString& title)
{
	BookmarksUtils::addBookmarkDialog(this, url, title);
}

void BrowserWindow::tabWidgetIndexChanged(TabWidget* tbWidget)
{
	if (tabWidget()->count() < 1 || tabWidget() == tbWidget)
		return;

	// Change the tabs space for the restore action
	if (m_restoreAction && m_tabsSpaceSplitter->tabWidget())
		disconnect(m_restoreAction, SIGNAL(triggered()), m_tabsSpaceSplitter->tabWidget(), SLOT(restoreClosedTab()));

	// Update the current tab widget
	m_tabsSpaceSplitter->currentTabWidgetChanged(tbWidget);

	emit tabWidgetChanged(tbWidget);

	connect(m_restoreAction, SIGNAL(triggered()), m_tabsSpaceSplitter->tabWidget(), SLOT(restoreClosedTab()));

	// Move the floating button to the new focused tabs space if the user wants
	if (m_fButton) {
		QRect tabWidgetRect = tbWidget->geometry();

		if (!tabWidgetRect.contains(tbWidget->mapFromGlobal(mapToGlobal(m_fButton->pos())))
			&& Application::instance()->floatingButtonFoloweMouse()) {
			m_fButton->tabWidgetChanged(tbWidget);
		}
	}
}

void BrowserWindow::shotBackground()
{
	// Citorva will explain this
	m_tabsSpaceSplitter->hide();
	if (m_fButton) m_fButton->hide();
	m_titleBar->hide();

	QPixmap *bg = new QPixmap(size());
	render(bg, QPoint(), QRect(0, 0, width(), height()));
	m_bg = new QImage(bg->toImage());
	m_tabsSpaceSplitter->show();
	m_titleBar->show();
	if (m_fButton) m_fButton->show();
	m_blur_bg = new QImage(applyBlur(m_bg, m_blur_radius));
}

QImage BrowserWindow::applyBlur(const QImage *src, qreal radius, bool quality, bool alphaOnly, int transposed)
{
	QPixmap ret(src->size());
	QPainter painter(&ret);
	{
		QPixmap big(QSize(src->width() + 2 * radius, src->height() + 2 * radius));
		QPainter big_painter(&big);

		big_painter.drawImage(QPoint(radius, radius), src->copy());

		{
			QPixmap	left(QSize(1, big.height())),
				right(QSize(1, big.height()));

			QPainter painter_left(&left),
				painter_right(&right);

			painter_left.drawImage(QPoint(0, radius), src->copy());
			painter_right.drawImage(QPoint(1 - src->width(), radius), src->copy());

			for (int i = 0; i < radius; i++) {
				big_painter.drawImage(QPoint(i, 0), left.toImage());
				big_painter.drawImage(QPoint(radius + src->width() + i, 0), right.toImage());
			}
		}

		{
			QPixmap top(QSize(big.width(), 1)),
				bottom(QSize(big.width(), 1));

			QPainter painter_top(&top),
				painter_bottom(&bottom);

			painter_top.drawImage(QPoint(0, -radius), big.toImage());
			painter_bottom.drawImage(QPoint(0, 1 + radius - big.height()), big.toImage());

			for (int i = 0; i < radius; i++) {
				big_painter.drawImage(QPoint(0, i), top.toImage());
				big_painter.drawImage(QPoint(0, radius + src->height() + i), bottom.toImage());
			}
		}

		QImage bgImage{big.toImage()};
		qt_blurImage(&big_painter, bgImage, radius, quality, alphaOnly, transposed);
		painter.drawImage(QPoint(-radius, -radius), big.toImage());
	}
	return ret.toImage();
}

void BrowserWindow::paintEvent(QPaintEvent* event)
{
	// Citorva will explain this
	QMainWindow::paintEvent(event);
	if (m_upd_ss) {
		m_upd_ss = false;
		shotBackground();
	}
}

void BrowserWindow::resizeEvent(QResizeEvent* event)
{
	// Move the floating button if need
	if (m_fButton) {
		if (m_fButton->pattern() != RootFloatingButton::Pattern::Floating)
			m_fButton->tabWidgetChanged(tabWidget());
		else {
			m_fButton->move(m_fButton->x() - (event->oldSize().width() - event->size().width()),
							m_fButton->y() - (event->oldSize().height() - event->size().height()));
		}
	}

	QMainWindow::resizeEvent(event);

	shotBackground();
}

void BrowserWindow::keyPressEvent(QKeyEvent* event)
{
	if (Application::instance()->plugins()->processKeyPress(Application::ON_BrowserWindow, this, event))
		return;

	QMainWindow::keyPressEvent(event);
}

void BrowserWindow::keyReleaseEvent(QKeyEvent* event)
{
	if (Application::instance()->plugins()->processKeyRelease(Application::ON_BrowserWindow, this, event))
		return;

	QMainWindow::keyReleaseEvent(event);
}

void BrowserWindow::mouseMoveEvent(QMouseEvent *e)
{
	if ((e->pos().x() >= pos().x() && e->pos().x() <= (pos().x() + width())) ||
		(e->pos().y() >= pos().y() && e->pos().y() <= (pos().y() + height())))
		emit mouseOver(true);
	else
		emit mouseOver(false);
	QMainWindow::mouseMoveEvent(e);
}

void BrowserWindow::addTab()
{
	tabWidget()->addView(QUrl(), Application::NTT_SelectedNewEmptyTab, true);
	tabWidget()->setCurrentTabFresh(true);
}

void BrowserWindow::postLaunch()
{
	bool addTab{true};
	QUrl startUrl{};

	Application::AfterLaunch launchAction = Application::instance()->afterLaunch();

	if (Application::instance()->isStartingAfterCrash()) {
		launchAction = Application::instance()->afterCrashLaunch();
	}

	switch (launchAction) {
	case Application::OpenBlankPage:
		startUrl = QUrl();
		break;
	case Application::OpenHomePage:
	case Application::OpenSavedSession:
	case Application::RestoreSession:
		startUrl = m_homePage;
		break;

	default:
		break;
	}

	switch (m_windowType) {
	case Application::WT_FirstAppWindow:
		// Check if we start after a crash or if we want to restore session
		if (Application::instance()->isStartingAfterCrash()) {
			if (Application::instance()->afterCrashLaunch() == Application::AfterLaunch::RestoreSession)
				addTab = m_tabsSpaceSplitter->count() <= 0;
		}
		else if (Application::instance()->afterLaunch() == Application::AfterLaunch::RestoreSession ||
				 Application::instance()->afterLaunch() == Application::AfterLaunch::OpenSavedSession) {
			addTab = m_tabsSpaceSplitter->count() <= 0;
		}
		break;
	case Application::WT_NewWindow:
		addTab = true;
		break;
	case Application::WT_OtherRestoredWindow:
		addTab = false;
		break;

	}

	show();

	if (!m_startUrl.isEmpty()) {
		startUrl = m_startUrl;
		addTab = true;
	}
	if (m_startTab) {
		addTab = false;
		
		m_tabsSpaceSplitter->createNewTabsSpace();

		tabWidget()->addView(m_startTab, Application::NTT_SelectedTabAtEnd);
	}
	if (m_startPage) {
		addTab = false;
		tabWidget()->addView(QUrl());
		tabWidget()->webTab()->webView()->setPage(m_startPage);
	}

	if (addTab) {
		m_tabsSpaceSplitter->createNewTabsSpace();

		tabWidget()->addView(startUrl, Application::NTT_CleanSelectedTabAtEnd);

		if (startUrl.isEmpty())
			tabWidget()->webTab()->addressBar()->setFocus();
	}

	if (tabWidget()->tabBar()->normalTabsCount() <= 0)
		tabWidget()->addView(m_homePage, Application::NTT_SelectedTabAtEnd);

	//TODO: emit main window created to plugins

	m_restoreAction = new QAction(tr("Restore Closed Tab"), this);
	m_restoreAction->setShortcut(QKeySequence("Ctrl+Shift+T"));
	this->addAction(m_restoreAction);
	connect(m_restoreAction, SIGNAL(triggered()), tabWidget(), SLOT(restoreClosedTab()));

	tabWidget()->tabBar()->ensureVisible();

	if (!Application::instance()->useTopToolBar() && !m_fButton)
		setupFloatingButton();
	else if (Application::instance()->useTopToolBar() && m_fButton) {
		delete m_fButton;
		m_fButton = nullptr;
	}

	if (m_fButton)
		m_fButton->tabWidgetChanged(tabWidget());

	Application::instance()->plugins()->emitMainWindowCreated(this);

	Settings settings{};

	// Show the "getting started" page if it's the first time Sielo is launch
	if (!settings.value("installed", false).toBool()) {
#ifndef QT_DEBUG
		Application::instance()->piwikTraker()->sendEvent("installation", "installation", "installation", "new installation");
#endif
		NavigationControlDialog* navigationControlDialog{new NavigationControlDialog(this)};
		navigationControlDialog->exec();

		settings.setValue("installed", true);
	}
}

void BrowserWindow::floatingButtonPatternChange(RootFloatingButton::Pattern pattern)
{
	if (pattern != RootFloatingButton::Pattern::Floating) {
		m_fButton->tabWidgetChanged(tabWidget());
	}
}

void BrowserWindow::newWindow()
{
	m_tabsSpaceSplitter->tabWidget(m_fButton->pos())->webTab()->sNewWindow();
}

void BrowserWindow::goHome()
{
	m_tabsSpaceSplitter->tabWidget(m_fButton->pos())->webTab()->sGoHome();
}

void BrowserWindow::forward()
{
	m_tabsSpaceSplitter->tabWidget(m_fButton->pos())->webTab()->webView()->forward();
}

void BrowserWindow::back()
{
	m_tabsSpaceSplitter->tabWidget(m_fButton->pos())->webTab()->webView()->back();
}

void BrowserWindow::newTab()
{
	LoadRequest request{};
	request.setUrl(m_tabsSpaceSplitter->tabWidget(m_fButton->pos())->urlOnNewTab());
	m_tabsSpaceSplitter->tabWidget(m_fButton->pos())->webTab()->webView()->loadInNewTab(request, Application::NTT_CleanSelectedTabAtEnd);
}

void BrowserWindow::setupUi()
{
	m_titleBar = new TitleBar(this);
	m_tabsSpaceSplitter = new TabsSpaceSplitter(this);

	QPalette palette{QToolTip::palette()};
	QColor color{palette.window().color()};

	color.setAlpha(0);
	palette.setColor(QPalette::Window, color);

	QToolTip::setPalette(palette);

	setMinimumWidth(300);
	//TODO: delete this line when settings will be implements
	resize(1200, 720);
	setCentralWidget(m_tabsSpaceSplitter);
}

void BrowserWindow::setupFloatingButton()
{
	m_fButton = new RootFloatingButton(this, this);

	// Get floating button state
	QFile fButtonDataFile{DataPaths::currentProfilePath() + QLatin1String("/fbutton.dat")};

	if (fButtonDataFile.exists()) {
		fButtonDataFile.open(QIODevice::ReadOnly);

		QDataStream fButtonData{&fButtonDataFile};
		int version{0};
		int pattern;
		QPoint lastPosition{};

		fButtonData >> version;

		if (version <= 0x0003) {
			int buttonCount{0};

			fButtonData >> buttonCount;

			for (int i{0}; i < buttonCount; ++i) {
				QString button{""};
				QString toolTip{"floating-button"};

				fButtonData >> button;

				if (version >= 0x0003) {
					fButtonData >> toolTip;
				}
				else {
					if (button == "fbutton-next")
						toolTip = tr("Go Forward");
					else if (button == "fbutton-back")
						toolTip = tr("Go Back");
					else if (button == "fbutton-home")
						toolTip = tr("Go Home");
					else if (button == "fbutton-add-bookmark")
						toolTip = tr("Add Bookmark");
					else if (button == "fbutton-view-bookmarks")
						toolTip = tr("Show all Bookmarks");
					else if (button == "fbutton-view-history")
						toolTip = tr("Show History");
					else if (button == "fbutton-new-window")
						toolTip = tr("Open New Window");
					else if (button == "fbutton-new-tab")
						toolTip = tr("Open New Tab");
				}

				m_fButton->addButton(button, toolTip);
			}

			if (version >= 0x0002) {
				fButtonData >> pattern;
				fButtonData >> lastPosition;

				m_fButton->setPattern(static_cast<RootFloatingButton::Pattern>(pattern));
			}
		}
	}
	else { // Load default floating button
		m_fButton->addButton("fbutton-next", tr("Go Forward"));
		m_fButton->addButton("fbutton-back", tr("Go Back"));
		m_fButton->addButton("fbutton-home", tr("Go Home"));
		m_fButton->addButton("fbutton-add-bookmark", tr("Add Bookmark"));
		m_fButton->addButton("fbutton-view-bookmarks", tr("Show all Bookmarks"));
		m_fButton->addButton("fbutton-view-history", tr("Show History"));
		m_fButton->addButton("fbutton-new-window", tr("Open New Window"));
		m_fButton->addButton("fbutton-new-tab", tr("Open New Tab"));
	}

	connect(m_fButton, &RootFloatingButton::statusChanged, this, &BrowserWindow::saveButtonState);
	connect(m_fButton, &RootFloatingButton::patternChanged, this, &BrowserWindow::floatingButtonPatternChange);

	connect(m_fButton->button("fbutton-view-bookmarks"), &FloatingButton::isClicked, tabWidget(),
			&TabWidget::openBookmarksDialog);
	connect(m_fButton->button("fbutton-view-history"), &FloatingButton::isClicked, tabWidget(),
			&TabWidget::openHistoryDialog);
	connect(m_fButton->button("fbutton-add-bookmark"), &FloatingButton::isClicked, this,
			&BrowserWindow::bookmarkPage);
	connect(m_fButton->button("fbutton-new-window"), &FloatingButton::isClicked, this, &BrowserWindow::newWindow);
	connect(m_fButton->button("fbutton-home"), &FloatingButton::isClicked, this, &BrowserWindow::goHome);
	connect(m_fButton->button("fbutton-next"), &FloatingButton::isClicked, this, &BrowserWindow::forward);
	connect(m_fButton->button("fbutton-back"), &FloatingButton::isClicked, this, &BrowserWindow::back);
	connect(m_fButton->button("fbutton-new-tab"), &FloatingButton::isClicked, this, &BrowserWindow::newTab);
}

void BrowserWindow::saveButtonState()
{
	QByteArray data{};
	QDataStream stream{&data, QIODevice::WriteOnly};

	stream << 0x0003;
	stream << m_fButton->buttons().size();

	for (int i{0}; i < m_fButton->buttons().size(); ++i) {
		foreach(FloatingButton* button, m_fButton->buttons())
		{
			if (button->index() == i) {
				stream << button->objectName();
				stream << button->toolTip();
				break;
			}
		}
	}

	stream << m_fButton->pattern();

	QFile fButtonFile{DataPaths::currentProfilePath() + QLatin1String("/fbutton.dat")};

	fButtonFile.open(QIODevice::WriteOnly);
	fButtonFile.write(data);
	fButtonFile.close();
}

void BrowserWindow::setCaption(const QWidget* widget)
{
	m_captionWidget = widget;
}

#ifdef Q_OS_WIN
bool BrowserWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	Q_UNUSED(eventType);
	const MSG* wMsg = reinterpret_cast<MSG*>(message);
	const UINT wMessage = wMsg->message;
	bool hasHandled = false;
	long res = 0;

	if (DWMEnabled())
		hasHandled = DwmDefWindowProc(wMsg->hwnd, wMessage, wMsg->wParam,
									  wMsg->lParam, reinterpret_cast<LRESULT*>(&res));

	if (wMessage == WM_NCCALCSIZE && wMsg->wParam == TRUE) {
		NCCALCSIZE_PARAMS& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(wMsg->lParam);
		adjust_maximized_client_rect(wMsg->hwnd, params.rgrc[0]);
		res = 0;
		hasHandled = true;
	}
	else if (wMessage == WM_NCHITTEST && res == 0) {
		res = ncHitTest(wMsg);

		if (res != HTNOWHERE)
			hasHandled = true;
	}
	//else if (wMessage == WM_NCPAINT)
	//	hasHandled = true;

	if (hasHandled)
		*result = res;
	return hasHandled;
}

long BrowserWindow::ncHitTest(const MSG* wMsg) const
{
	const long ncHitZone[3][4] = {
		{HTTOPLEFT, HTLEFT, HTLEFT, HTBOTTOMLEFT},
		{HTTOP, HTCAPTION, HTNOWHERE, HTBOTTOM},
		{HTTOPRIGHT, HTRIGHT, HTRIGHT, HTBOTTOMRIGHT}
	};

	const QPoint cursor(GET_X_LPARAM(wMsg->lParam), GET_Y_LPARAM(wMsg->lParam));
	const UINT8 borderSize = 2;
	UINT8 xPos = 1;
	UINT8 yPos = 2;

	RECT rcWin;
	GetWindowRect(wMsg->hwnd, &rcWin);

	if (m_captionWidget == QApplication::widgetAt(QCursor::pos()))
		return HTCAPTION;

	RECT rcFrame = {0};
	AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, NULL);

	if (cursor.y() >= rcWin.top && cursor.y() <= rcWin.top + borderSize)
		yPos = 0;
	else if (cursor.y() >= rcWin.top + borderSize && cursor.y() <= rcWin.top + borderSize)
		yPos = 1;
	else if (cursor.y() >= rcWin.bottom - borderSize && cursor.y() < rcWin.bottom)
		yPos = 3;

	if (cursor.x() >= rcWin.left && cursor.x() < rcWin.left + borderSize)
		xPos = 0;
	else if (cursor.x() >= rcWin.right - borderSize && cursor.x() < rcWin.right)
		xPos = 2;

	return ncHitZone[xPos][yPos];
}
#endif
}
