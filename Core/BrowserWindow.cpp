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
#include <QSettings>

#include <QAction>

#include <QTimer>
#include <QMessageBox>

#include "Bookmarks/BookmarksUtils.hpp"

#include "Mockup/MockupItem.hpp"

#include "Web/LoadRequest.hpp"
#include "Web/WebPage.hpp"
#include "Web/Tab/WebTab.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/StatusBarMessage.hpp"
#include "Widgets/TitleBar.hpp"
#include "Widgets/AddressBar/AddressBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"

namespace Sn {

BrowserWindow::BrowserWindow(Application::WindowType type, const QUrl& url) :
		QMainWindow(nullptr),
		m_startUrl(url),
		m_windowType(type),
		m_statusBarMessage(new StatusBarMessage(this))
{
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_DontCreateNativeAncestors);
	setAcceptDrops(true);

#ifdef Q_OS_WIN
	setWindowFlags(Qt::CustomizeWindowHint);
#endif

	setObjectName(QLatin1String("mainwindow"));
	setWindowTitle(tr("Sielo"));
	setProperty("private", Application::instance()->privateBrowsing());

	statusBar()->hide();

	setupUi();
	if (!Application::instance()->useTopToolBar())
		setupFloatingButton();
	loadSettings();

	QTimer::singleShot(10, this, &BrowserWindow::postLaunch);

}

BrowserWindow::BrowserWindow(MockupItem* mockup) :
	BrowserWindow(Application::WT_OtherRestoredWindow)
{	
	Q_ASSERT(mockup->tabsSpaces().count() >= 1);
	Q_ASSERT(mockup->tabsSpaces()[0]->tabs.count() >= 1);

	int selectedTab{ 0 };
	for (int i{0}; i < mockup->tabsSpaces()[0]->tabs.count(); ++i) {
		MockupItem::Tab* tab = mockup->tabsSpaces()[0]->tabs[i];

		tabWidget(0)->addView(tab->url);

		if (tab->selected)
			selectedTab = i;
	}

	tabWidget(0)->setCurrentIndex(selectedTab);

	int workingVerticalIndex{ 0 };
	for (int i{1}; i < mockup->tabsSpaces().count(); ++i) {
		MockupItem::TabsSpace* tabsSpace = mockup->tabsSpaces()[i];

		if (tabsSpace->verticalIndex == workingVerticalIndex) 
			createNewTabsSpace(BrowserWindow::TSP_Bottom, nullptr);
		else {
			createNewTabsSpace(BrowserWindow::TSP_Right, nullptr);
			workingVerticalIndex = tabsSpace->verticalIndex;
		}

		for (int j{0}; j < tabsSpace->tabs.count(); ++j) {
			MockupItem::Tab* tab = tabsSpace->tabs[j];

			tabWidget(i)->addView(tab->url);

			if (tab->selected)
				selectedTab = j;
		}

		tabWidget(i)->setCurrentIndex(selectedTab);
	}
}


BrowserWindow::~BrowserWindow()
{
	//TODO: emit window deleted to plugins
}

void BrowserWindow::loadSettings()
{
	QSettings settings{};

	m_homePage = settings.value(QLatin1String("Web-Settings/homePage"), QUrl("https://doosearch.sielo.app/")).toUrl();

	m_spaceBetweenTabsSpaces = settings.value(QLatin1String("Settings/tabsSpacesPadding"), 7).toInt();

			foreach (TabWidget* tabWidget, m_tabWidgets) {
			tabWidget->loadSettings();
			if (tabWidget->parentWidget())
				tabWidget->parentWidget()->setContentsMargins(m_spaceBetweenTabsSpaces, m_spaceBetweenTabsSpaces,
															  m_spaceBetweenTabsSpaces, m_spaceBetweenTabsSpaces);
		}

	QString backgroundPath = settings.value(QLatin1String("Settings/backgroundPath"), "").toString();

	if (!backgroundPath.isEmpty()) {
		QString sss = styleSheet();
		sss += "QMainWindow {";
		sss += "background-image: url(" + backgroundPath + ");";
		sss += "background-attachment: fixed;";
		sss += "background-position: top;";

		if (settings.value(QLatin1String("Settings/repeatBackground"), false).toBool())
			sss += "background-repeat: repeat;";
		else
			sss += "background-repeat: no-repeat;";

		sss += "}";

		setStyleSheet(sss);
	}
	m_upd_ss = true;

	bool showBookmarksToolBar = settings.value(QLatin1String("ShowBookmarksToolBar"), true).toBool();
	m_titleBar->setShowBookmark(showBookmarksToolBar);
	foreach (TabWidget* tbWidget, m_tabWidgets) {
		tbWidget->updateShowBookmarksBarText(showBookmarksToolBar);
	}
}

QByteArray BrowserWindow::saveTabs()
{
	QByteArray data{};
	QDataStream stream{&data, QIODevice::WriteOnly};

	stream << m_mainSplitter->count();

	for (int i{0}; i < m_mainSplitter->count(); ++i) {
		QSplitter* verticalSplitter = static_cast<QSplitter*>(m_mainSplitter->widget(i));

		stream << verticalSplitter->count();

		for (int j{0}; j < verticalSplitter->count(); ++j)
			stream << static_cast<TabWidget*>(verticalSplitter->widget(j)->findChild<TabWidget*>(QString("tabwidget")))
					->saveState();

	}

	return data;
}

void BrowserWindow::setStartTab(WebTab* tab)
{
	m_startTab = tab;
}

void BrowserWindow::setStartPage(WebPage* page)
{
	m_startPage = page;
}

void BrowserWindow::restoreWindowState(const RestoreManager::WindowData& data)
{
	restoreState(data.windowState);
	restoreGeometry(data.windowGeometry);
	int mainSplitterCount{data.spaceTabsCount[0]};
	int tabWidgetToRestore{0};

	for (int i{0}; i < mainSplitterCount; ++i) {
		int verticalSplitterCount{data.spaceTabsCount[i + 1]};
		QSplitter* verticalSplitter{new QSplitter(Qt::Vertical, this)};
		verticalSplitter->setObjectName("vertical-splitter");

		for (int j{0}; j < verticalSplitterCount; ++j) {
			if (i == 0 && j == 0) {
				m_tabWidgets[0]->restoreState(data.tabsState[0], data.currentTabs[0], data.homeUrls[0]);
				verticalSplitter->addWidget(m_tabWidgets[0]->parentWidget());
				m_mainSplitter->widget(0)->deleteLater();
				++tabWidgetToRestore;
			}
			else {
				QWidget* widget{new QWidget(this)};
				QVBoxLayout* layout{new QVBoxLayout(widget)};
				TabWidget* tabWidget{new TabWidget(this, Application::TST_Web, widget)};

				layout->setSpacing(0);
				layout->setContentsMargins(0, 0, 0, 0);

				m_tabWidgets.append(tabWidget);
				m_currentTabWidget = tabWidgetToRestore;

				tabWidget->restoreState(data.tabsState[tabWidgetToRestore],
										data.currentTabs[tabWidgetToRestore],
										data.homeUrls[tabWidgetToRestore]);
				tabWidget->tabBar()->show();

				layout->addWidget(tabWidget->tabBar());
				layout->addWidget(tabWidget);

				connect(tabWidget, &TabWidget::focusIn, this, &BrowserWindow::tabWidgetIndexChanged);

				verticalSplitter->addWidget(widget);

				++tabWidgetToRestore;
			}

		}
		m_mainSplitter->addWidget(verticalSplitter);
	}

	autoResizeTabsSpace();
	loadSettings();
}

void BrowserWindow::currentTabChanged(WebTab*)
{
	TabbedWebView* view{webView()};

	if (!view)
		return;

	setWindowTitle(tr("%1 - Sielo").arg(view->webTab()->title()));

	view->setFocus();
}

void BrowserWindow::createNewTabsSpace(TabsSpacePosition position, WebTab* tab, TabWidget* from)
{
	if (!from)
		from = tabWidget();

	QWidget* widgetTabWidget{createWidgetTabWidget(tab)};
	insertTabsSpace(position, widgetTabWidget, from);
}

void BrowserWindow::createNewTabsSpace(TabsSpacePosition position, Application::TabsSpaceType type, WebTab* tab)
{
	Q_UNUSED(position);
	Q_UNUSED(type);
	Q_UNUSED(tab);
	// TODO: manage applications
}

void BrowserWindow::insertTabsSpace(TabsSpacePosition position, QWidget* widgetTabWidget, TabWidget* from)
{
	widgetTabWidget->setContentsMargins(m_spaceBetweenTabsSpaces, m_spaceBetweenTabsSpaces, m_spaceBetweenTabsSpaces,
										m_spaceBetweenTabsSpaces);

	if (position == BrowserWindow::TSP_Left || position == BrowserWindow::TSP_Right) {
		QSplitter* verticalSplitter{new QSplitter(Qt::Vertical, this)};
		if (position == BrowserWindow::TSP_Left) {
			verticalSplitter->addWidget(widgetTabWidget);

			m_mainSplitter->insertWidget(m_mainSplitter
												 ->indexOf(static_cast<QSplitter*>(from
														 ->parent()->parent())), verticalSplitter);
		}
		else {
			verticalSplitter->addWidget(widgetTabWidget);

			m_mainSplitter->insertWidget(
					m_mainSplitter->indexOf(static_cast<QSplitter*>(from->parent()->parent()))
					+ 1, verticalSplitter);
		}

		QList<int> size;

		for (int i{0}; i < m_mainSplitter->count(); ++i)
			size.append(m_mainSplitter->width() / m_mainSplitter->count());

		m_mainSplitter->setSizes(size);
	}
	else {
		QSplitter* verticalSplitter = static_cast<QSplitter*>(from->parent()->parent());
		if (position == BrowserWindow::TSP_Top)
			verticalSplitter->insertWidget(verticalSplitter->indexOf(static_cast<QWidget*>(from->parent())),
										   widgetTabWidget);
		else if (position == BrowserWindow::TSP_Bottom)
			verticalSplitter->insertWidget(verticalSplitter->indexOf(static_cast<QWidget*>(from->parent())) + 1,
										   widgetTabWidget);

		QList<int> size;

		for (int i{0}; i < verticalSplitter->count(); ++i)
			size.append(verticalSplitter->height() / verticalSplitter->count());

		verticalSplitter->setSizes(size);
	}
}

void BrowserWindow::closeTabsSpace(TabWidget* tabWidget)
{
	QWidget* widgetTabWidget = static_cast<QWidget*>(tabWidget->parent());
	QSplitter* verticalSplitter = static_cast<QSplitter*>(tabWidget->parent()->parent());

	m_tabWidgets.removeOne(tabWidget);

	if (verticalSplitter->count() <= 1)
		verticalSplitter->deleteLater();
	else
		widgetTabWidget->deleteLater();

	m_currentTabWidget = 0;
}

void BrowserWindow::autoResizeTabsSpace()
{
	QList<int> mainSizes;
	QList<int> verticalSizes;
	for (int i{0}; i < m_mainSplitter->count(); ++i)
		mainSizes.append(m_mainSplitter->width() / m_mainSplitter->count());

	m_mainSplitter->setSizes(mainSizes);

	for (int i{0}; i < m_mainSplitter->count(); ++i) {
		mainSizes.clear();

		QSplitter* verticalSplitter = static_cast<QSplitter*>(m_mainSplitter->widget(i));

		for (int j{0}; j < verticalSplitter->count(); ++j)
			verticalSizes.append(verticalSplitter->height() / verticalSplitter->count());

		verticalSplitter->setSizes(verticalSizes);
	}
}

void BrowserWindow::loadUrl(const QUrl& url)
{
	if (webView()->webTab()->isPinned()) {
		int index{tabWidget()->addView(url, Application::NTT_CleanSelectedTab)};
		webView(index)->setFocus();
	}
	else {
		webView()->setFocus();
		webView()->load(url);
	}
}

void BrowserWindow::loadUrlInNewTab(const QUrl& url)
{
	tabWidget()->addView(url);
}

MockupItem* BrowserWindow::mockupItem() const
{
	MockupItem* item{ new MockupItem(tr("Session"), true) };
	item->clear();

	foreach (TabWidget* tabWidget, m_tabWidgets)
	{
		MockupItem::TabsSpace* tabsSpace{ new MockupItem::TabsSpace() };

		foreach (WebTab* tab, tabWidget->allTabs())
		{
			MockupItem::Tab* mockupTab{ new MockupItem::Tab() };
			mockupTab->icon = tab->icon();
			mockupTab->title = tab->title();
			mockupTab->url = tab->url();
			mockupTab->selected = tabWidget->weTab() == tab;
			mockupTab->parent = tabsSpace;

			tabsSpace->tabs.append(mockupTab);
		}

		QSplitter* splitter{ qobject_cast<QSplitter*>(tabWidget->parent()->parent()) };

		tabsSpace->verticalIndex = m_mainSplitter->indexOf(splitter);
		tabsSpace->parent = item;

		item->addTabsSpace(tabsSpace);
	}

	return item;
}

TabbedWebView* BrowserWindow::webView() const
{
	return webView(tabWidget()->currentIndex());
}

TabbedWebView* BrowserWindow::webView(int index) const
{
	WebTab* webTab = qobject_cast<WebTab*>(tabWidget()->widget(index));
	if (!webTab)
		return 0;

	return webTab->webView();
}

TabWidget* BrowserWindow::tabWidget() const
{
	return m_tabWidgets[m_currentTabWidget];
}

TabWidget* BrowserWindow::tabWidget(int index) const
{
	return m_tabWidgets[index];
}

int BrowserWindow::tabWidgetsCount() const
{
	return m_tabWidgets.count();
}

void BrowserWindow::setWindowTitle(const QString& title)
{
	QString t{title};

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

void BrowserWindow::tabsSpaceInFullView(QWidget* widget)
{
	QList<int> sizes{};
	for (int i{0}; i < m_mainSplitter->count(); ++i) {
		QSplitter* splitter{qobject_cast<QSplitter*>(m_mainSplitter->widget(i))};

		QList<int> verticalSizes{};
		bool containTabsSpace{false};
		for (int j{0}; j < splitter->count(); ++j) {
			if (splitter->widget(j) != widget)
				verticalSizes.append(0);
			else {
				containTabsSpace = true;
				verticalSizes.append(widget->height());
			}
		}


		if (!containTabsSpace)
			sizes.append(0);
		else {
			splitter->setSizes(verticalSizes);
			sizes.append(splitter->width());
		}
	}

	m_mainSplitter->setSizes(sizes);
}

void BrowserWindow::arrangeTabsSpaces()
{
	QList<int> sizes{};

	for (int i{0}; i < m_mainSplitter->count(); ++i) {
		QSplitter* splitter{qobject_cast<QSplitter*>(m_mainSplitter->widget(i))};
		QList<int> verticalSizes{};

		for (int j{0}; j < splitter->count(); ++j)
			verticalSizes.append(height() / splitter->count());

		splitter->setSizes(verticalSizes);
		sizes.append(width() / m_mainSplitter->count());
	}

	m_mainSplitter->setSizes(sizes);
}

void BrowserWindow::bookmarkPage()
{
	TabbedWebView* view{ webView() };

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

void BrowserWindow::shotBackground()
{
	m_mainSplitter->hide();
	m_bg = new QPixmap(size());
	render(m_bg, QPoint(), QRect(0, 0, width(), height()));
	m_mainSplitter->show();
}

void BrowserWindow::paintEvent(QPaintEvent* event)
{
	QMainWindow::paintEvent(event);
	if (m_upd_ss) {
		m_upd_ss = false;
		shotBackground();
	}
}

void BrowserWindow::resizeEvent(QResizeEvent* event)
{
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

const QPixmap* BrowserWindow::getBackground() {
	return m_bg;
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
		if ((Application::instance()->isStartingAfterCrash() &&
			 Application::instance()->afterCrashLaunch() == Application::AfterLaunch::RestoreSession) ||
			(Application::instance()->afterLaunch() == Application::RestoreSession
			 || Application::instance()->afterLaunch() == Application::OpenSavedSession)
			&& Application::instance()->restoreManager()) {
			if (!(Application::instance()->isStartingAfterCrash() && Application::instance()->afterCrashLaunch() == Application::AfterLaunch::OpenHomePage)) {
				addTab = !Application::instance()
						->restoreSession(this, Application::instance()->restoreManager()->restoreData());
			}
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
		tabWidget()->addView(m_startTab);
	}
	if (m_startPage) {
		addTab = false;
		tabWidget()->addView(QUrl());
		webView()->setPage(m_startPage);
	}

	if (addTab) {
		tabWidget()->addView(startUrl, Application::NTT_CleanSelectedTabAtEnd);

		if (startUrl.isEmpty())
			webView()->webTab()->addressBar()->setFocus();
	}

	if (tabWidget()->tabBar()->normalTabsCount() <= 0)
		tabWidget()->addView(m_homePage, Application::NTT_SelectedTabAtEnd);

	//TODO: emit main window created to plugins

	m_restoreAction = new QAction(tr("Restore Closed Tab"), this);
	m_restoreAction->setShortcut(QKeySequence("Ctrl+Shift+T"));
	this->addAction(m_restoreAction);
	connect(m_restoreAction, SIGNAL(triggered()), tabWidget(), SLOT(restoreClosedTab()));

	tabWidget()->tabBar()->ensureVisible();

	if (m_fButton)
		m_fButton->tabWidgetChanged(tabWidget());
}

void BrowserWindow::tabWidgetIndexChanged(TabWidget* tbWidget)
{
	if (m_currentTabWidget == m_tabWidgets.indexOf(tbWidget))
		return;

	if (m_restoreAction && m_tabWidgets[m_currentTabWidget])
		disconnect(m_restoreAction, SIGNAL(triggered()), m_tabWidgets[m_currentTabWidget], SLOT(restoreClosedTab()));

	m_currentTabWidget = m_tabWidgets.indexOf(tbWidget);

	connect(m_restoreAction, SIGNAL(triggered()), m_tabWidgets[m_currentTabWidget], SLOT(restoreClosedTab()));

	if (m_fButton) {
		QRect tabWidgetRect = tabWidget()->geometry();

		if (!tabWidgetRect.contains(tabWidget()->mapFromGlobal(mapToGlobal(m_fButton->pos())))
			&& Application::instance()->floatingButtonFoloweMouse()) {
			m_fButton->tabWidgetChanged(tabWidget());
		}
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
			foreach (TabWidget* tabWidget, m_tabWidgets) {
			QRect tabWidgetRect = tabWidget->geometry();

			if (tabWidgetRect.contains(tabWidget->mapFromGlobal(mapToGlobal(m_fButton->pos())))) {
				m_currentTabWidget = m_tabWidgets.indexOf(tabWidget);
				tabWidget->weTab()->sNewWindow();
				return;
			}
		}
}

void BrowserWindow::goHome()
{
			foreach (TabWidget* tabWidget, m_tabWidgets) {
			QRect tabWidgetRect = tabWidget->geometry();

			if (tabWidgetRect.contains(tabWidget->mapFromGlobal(mapToGlobal(m_fButton->pos())))) {
				m_currentTabWidget = m_tabWidgets.indexOf(tabWidget);
				tabWidget->weTab()->sGoHome();
				return;
			}
		}
}

void BrowserWindow::forward()
{
			foreach (TabWidget* tabWidget, m_tabWidgets) {
			QRect tabWidgetRect = tabWidget->geometry();

			if (tabWidgetRect.contains(tabWidget->mapFromGlobal(mapToGlobal(m_fButton->pos())))) {
				m_currentTabWidget = m_tabWidgets.indexOf(tabWidget);
				tabWidget->weTab()->webView()->forward();
				return;
			}
		}
}

void BrowserWindow::back()
{
			foreach (TabWidget* tabWidget, m_tabWidgets) {
			QRect tabWidgetRect = tabWidget->geometry();

			if (tabWidgetRect.contains(tabWidget->mapFromGlobal(mapToGlobal(m_fButton->pos())))) {
				m_currentTabWidget = m_tabWidgets.indexOf(tabWidget);
				tabWidget->weTab()->webView()->back();
				return;
			}
		}
}

void BrowserWindow::newTab()
{
			foreach (TabWidget* tabWidget, m_tabWidgets) {
			QRect tabWidgetRect = tabWidget->geometry();

			if (tabWidgetRect.contains(tabWidget->mapFromGlobal(mapToGlobal(m_fButton->pos())))) {
				m_currentTabWidget = m_tabWidgets.indexOf(tabWidget);
				LoadRequest request{};
				request.setUrl(tabWidget->urlOnNewTab());
				tabWidget->weTab()->webView()->loadInNewTab(request, Application::NTT_CleanSelectedTabAtEnd);
				return;
			}
		}
}

void BrowserWindow::setupUi()
{
	m_titleBar = new TitleBar(this);

	QWidget* widget{new QWidget(this)};

	m_layout = new QVBoxLayout(widget);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_mainSplitter = new QSplitter(this);
	m_mainSplitter->setContentsMargins(0, 0, 0, 0);

	QWidget* widgetTabWidget{createWidgetTabWidget(nullptr, Application::TST_Web)};
	QSplitter* verticalSplitter{new QSplitter(Qt::Vertical, this)};
	verticalSplitter->setObjectName("vertical-splitter");
	verticalSplitter->setContentsMargins(0, 0, 0, 0);

	widget->setCursor(Qt::ArrowCursor);

	verticalSplitter->addWidget(widgetTabWidget);

	m_mainSplitter->addWidget(verticalSplitter);

	m_layout->addWidget(m_mainSplitter);

	QPalette palette{QToolTip::palette()};
	QColor color{palette.window().color()};

	color.setAlpha(0);
	palette.setColor(QPalette::Window, color);

	QToolTip::setPalette(palette);

	setMinimumWidth(300);
	//TODO: delete this line when settings will be implements
	resize(1200, 720);
	setCentralWidget(widget);

}

void BrowserWindow::setupFloatingButton()
{
	m_fButton = new RootFloatingButton(this, this);

	QFile fButtonDataFile{Application::instance()->paths()[Application::P_Data] + QLatin1String("/fbutton.dat")};

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
	else {
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
				foreach (FloatingButton* button, m_fButton->buttons()) {
				if (button->index() == i) {
					stream << button->objectName();
					stream << button->toolTip();
					break;
				}
			}
	}

	stream << m_fButton->pattern();

	QFile fButtonFile{Application::instance()->paths()[Application::P_Data] + QLatin1String("/fbutton.dat")};

	fButtonFile.open(QIODevice::WriteOnly);
	fButtonFile.write(data);
	fButtonFile.close();
}

QWidget* BrowserWindow::createWidgetTabWidget(WebTab* tab, Application::TabsSpaceType type)
{
	QWidget* widget{new QWidget(this)};
	QVBoxLayout* layout{new QVBoxLayout(widget)};
	TabWidget* tabWidget{new TabWidget(this, type, widget)};

	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

	m_tabWidgets.append(tabWidget);

	if (tab) {
		int previousCurrentTabWidget{m_currentTabWidget};
		m_currentTabWidget = m_tabWidgets.count() - 1;

		tab->detach();
		tabWidget->addView(tab);

		m_currentTabWidget = previousCurrentTabWidget;
	}

	tabWidget->tabBar()->show();

	layout->addWidget(tabWidget->tabBar());
	layout->addWidget(tabWidget);

	connect(tabWidget, &TabWidget::focusIn, this, &BrowserWindow::tabWidgetIndexChanged);
	connect(m_titleBar, &TitleBar::toggleBookmarksBar, tabWidget,
			&TabWidget::updateShowBookmarksBarText);

	return widget;
}

QWidget* BrowserWindow::createWidgetTabWidget(TabWidget* tabWidget, WebTab* tab)
{
	QWidget* widget{new QWidget(this)};
	QVBoxLayout* layout{new QVBoxLayout(widget)};
	tabWidget->setParent(widget);

	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

	m_tabWidgets.append(tabWidget);

	if (tab) {
		int previousCurrentTabWidget{m_currentTabWidget};
		m_currentTabWidget = m_tabWidgets.count() - 1;

		tab->detach();
		tabWidget->addView(tab);

		m_currentTabWidget = previousCurrentTabWidget;
	}

	tabWidget->tabBar()->show();

	layout->addWidget(tabWidget->tabBar());
	layout->addWidget(tabWidget);

	connect(tabWidget, &TabWidget::focusIn, this, &BrowserWindow::tabWidgetIndexChanged);

	return widget;
}

}
