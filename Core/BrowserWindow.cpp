/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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

#include <QSettings>

#include <QAction>

#include <QTimer>
#include <QMessageBox>

#include "Bookmarks/AddBookmarkDialog.hpp"

#include "Web/LoadRequest.hpp"
#include "Web/WebPage.hpp"
#include "Web/Tab/WebTab.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/AddressBar.hpp"
#include "Widgets/FloatingButton.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"

namespace Sn {

BrowserWindow::BrowserWindow(Application::WindowType type, const QUrl& url) :
	QMainWindow(nullptr),
	m_startUrl(url),
	m_windowType(type)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_DontCreateNativeAncestors);

	setObjectName(QLatin1String("mainwindow"));
	setWindowTitle(tr("Sielo"));
	setProperty("private", Application::instance()->privateBrowsing());

	setupUi();
	if (!Application::instance()->useTopToolBar())
		setupFloatingButton();
	loadSettings();

	QTimer::singleShot(10, this, &BrowserWindow::postLaunch);

}

BrowserWindow::~BrowserWindow()
{
	//TODO: emit window deleted to plugins
}

void BrowserWindow::loadSettings()
{
	QSettings settings{};

	m_homePage = settings.value(QLatin1String("Web-Settings/homePage"), QUrl("http://doosearch.esy.es/")).toUrl();

	settings.endGroup();

		foreach (TabWidget* tabWidget, m_tabWidgets) tabWidget->loadSettings();

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

void BrowserWindow::restoreTabs()
{

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
}

void BrowserWindow::currentTabChanged(WebTab*)
{
	TabbedWebView* view{webView()};

	if (!view)
		return;

	setWindowTitle(tr("%1 - Sielo").arg(view->webTab()->title()));

	view->setFocus();
}

void BrowserWindow::createNewTabsSpace(TabsSpacePosition position, WebTab* tab)
{
	QWidget* widgetTabWidget{createWidgetTabWidget(tab)};
	insertTabsSpace(position, widgetTabWidget);
}

void BrowserWindow::createNewTabsSpace(TabsSpacePosition position, Application::TabsSpaceType type, WebTab* tab)
{
	// TODO: manage applications
}

void BrowserWindow::insertTabsSpace(TabsSpacePosition position, QWidget* widgetTabWidget)
{
	if (position == BrowserWindow::TSP_Left || position == BrowserWindow::TSP_Right) {
		QSplitter* verticalSplitter{new QSplitter(Qt::Vertical, this)};
		if (position == BrowserWindow::TSP_Left) {
			verticalSplitter->addWidget(widgetTabWidget);

			m_mainSplitter->insertWidget(m_mainSplitter
											 ->indexOf(static_cast<QSplitter*>(m_tabWidgets[m_currentTabWidget]
												 ->parent()->parent())), verticalSplitter);
		}
		else {
			verticalSplitter->addWidget(widgetTabWidget);

			m_mainSplitter->insertWidget(
				m_mainSplitter->indexOf(static_cast<QSplitter*>(m_tabWidgets[m_currentTabWidget]->parent()->parent()))
				+ 1, verticalSplitter);
		}

		QList<int> size;

		for (int i{0}; i < m_mainSplitter->count(); ++i)
			size.append(m_mainSplitter->width() / m_mainSplitter->count());

		m_mainSplitter->setSizes(size);
	}
	else {
		QSplitter* verticalSplitter = static_cast<QSplitter*>(tabWidget()->parent()->parent());
		if (position == BrowserWindow::TSP_Top)
			verticalSplitter->insertWidget(verticalSplitter->indexOf(static_cast<QWidget*>(tabWidget()->parent())),
										   widgetTabWidget);
		else if (position == BrowserWindow::TSP_Bottom)
			verticalSplitter->insertWidget(verticalSplitter->indexOf(static_cast<QWidget*>(tabWidget()->parent())) + 1,
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

	QMainWindow::setWindowTitle(t);
}

void BrowserWindow::enterHtmlFullScreen()
{
	// Empty
}

void BrowserWindow::bookmarkAllTabs()
{
	// Empty
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

	switch (Application::instance()->afterLaunch()) {
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
		if ((Application::instance()->afterLaunch() == Application::RestoreSession
			 || Application::instance()->afterLaunch() == Application::OpenSavedSession)
			&& Application::instance()->restoreManager()) {
			addTab = !Application::instance()
				->restoreSession(this, Application::instance()->restoreManager()->restoreData());
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
}

void BrowserWindow::tabWidgetIndexChanged(TabWidget* tbWidget)
{
	if (m_currentTabWidget == m_tabWidgets.indexOf(tbWidget))
		return;

	disconnect(m_restoreAction, SIGNAL(triggered()), m_tabWidgets[m_currentTabWidget], SLOT(restoreClosedTab()));

	m_currentTabWidget = m_tabWidgets.indexOf(tbWidget);

	connect(m_restoreAction, SIGNAL(triggered()), m_tabWidgets[m_currentTabWidget], SLOT(restoreClosedTab()));

	if (m_fButton) {
		m_fButton->setTabWidget(tabWidget());
		QRect tabWidgetRect = tabWidget()->geometry();

		if (!tabWidgetRect.contains(tabWidget()->mapFromGlobal(mapToGlobal(m_fButton->pos())))
			&& Application::instance()->floatingButtonFoloweMouse()) {
			if (m_fButton->childrenExpanded())
				m_fButton->hideChildren();

			QPoint newFloatingButtonPos
				{QPoint(tbWidget->mapTo(this, tabWidget()->pos()).x(), tbWidget->mapTo(this, tabWidget()->pos()).y())};
			//newFloatingButtonPos.setX(mapFromGlobal(QCursor::pos()).x() + 15);

			m_fButton->move(newFloatingButtonPos);
		}
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

void BrowserWindow::openAddBookmarkDialog()
{
		foreach (TabWidget* tabWidget, m_tabWidgets) {
			QRect tabWidgetRect = tabWidget->geometry();

			if (tabWidgetRect.contains(tabWidget->mapFromGlobal(mapToGlobal(m_fButton->pos())))) {
				m_currentTabWidget = m_tabWidgets.indexOf(tabWidget);
				QString url = tabWidget->weTab()->url().toString();
				QString title = tabWidget->weTab()->title();

				AddBookmarkDialog* dialog{new AddBookmarkDialog(url, title)};
				dialog->setAttribute(Qt::WA_DeleteOnClose);

				dialog->show();
				return;
			}
		}
}

void BrowserWindow::setupUi()
{
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
	m_fButton = new FloatingButton(this, FloatingButton::Root);
	m_fButton->setObjectName("fbutton-root");

	m_fButtonAddBookmark = new FloatingButton(this);
	m_fButtonAddBookmark->setObjectName("fbutton-add-bookmark");
	m_fButtonAddBookmark->setToolTip(tr("Add This Page to Bookmarks"));

	m_fButtonViewBookmarks = new FloatingButton(this);
	m_fButtonViewBookmarks->setObjectName("fbutton-view-bookmarks");
	m_fButtonViewBookmarks->setToolTip(tr("View Bookmarks"));

	m_fButtonViewHistory = new FloatingButton(this);
	m_fButtonViewHistory->setObjectName("fbutton-view-history");
	m_fButtonViewHistory->setToolTip(tr("View History"));

	m_fButtonNewWindow = new FloatingButton(this);
	m_fButtonNewWindow->setObjectName("fbutton-new-window");
	m_fButtonNewWindow->setToolTip(tr("New Window"));

	m_fButtonHome = new FloatingButton(this);
	m_fButtonHome->setObjectName("fbutton-home");
	m_fButtonHome->setToolTip(tr("Go Home"));

	m_fButtonNext = new FloatingButton(this);
	m_fButtonNext->setObjectName("fbutton-next");
	m_fButtonNext->setToolTip(tr("Go Forward"));
//	m_fButtonNext->setMenu(m_menuForward);

	m_fButtonBack = new FloatingButton(this);
	m_fButtonBack->setObjectName("fbutton-back");
	m_fButtonBack->setToolTip(tr("Go Back"));

	m_fButtonNewTab = new FloatingButton(this);
	m_fButtonNewTab->setObjectName("fbutton-new-tab");
	m_fButtonNewTab->setToolTip(tr("Add New Tab"));

	QFile fButtonDataFile{Application::instance()->paths()[Application::P_Data] + QLatin1String("/fbutton.dat")};

	if (fButtonDataFile.exists()) {

		fButtonDataFile.open(QIODevice::ReadOnly);

		QDataStream fButtonData{&fButtonDataFile};
		int version{0};

		fButtonData >> version;

		if (version == 0x0001) {
			int buttonCount{0};

			fButtonData >> buttonCount;

			for (int i{0}; i < buttonCount; ++i) {
				QString button{""};

				fButtonData >> button;

				if (button == m_fButtonBack->objectName())
					m_fButton->addChild(m_fButtonBack);
				else if (button == m_fButtonNext->objectName())
					m_fButton->addChild(m_fButtonNext);
				else if (button == m_fButtonHome->objectName())
					m_fButton->addChild(m_fButtonHome);
				else if (button == m_fButtonAddBookmark->objectName())
					m_fButton->addChild(m_fButtonAddBookmark);
				else if (button == m_fButtonViewBookmarks->objectName())
					m_fButton->addChild(m_fButtonViewBookmarks);
				else if (button == m_fButtonViewHistory->objectName())
					m_fButton->addChild(m_fButtonViewHistory);
				else if (button == m_fButtonNewWindow->objectName())
					m_fButton->addChild(m_fButtonNewWindow);
				else if (button == m_fButtonNewTab->objectName())
					m_fButton->addChild(m_fButtonNewTab);
			}
		}
	}
	else {
		m_fButton->addChild(m_fButtonBack);
		m_fButton->addChild(m_fButtonNext);
		m_fButton->addChild(m_fButtonHome);
		m_fButton->addChild(m_fButtonAddBookmark);
		m_fButton->addChild(m_fButtonViewBookmarks);
		m_fButton->addChild(m_fButtonViewHistory);
		m_fButton->addChild(m_fButtonNewWindow);
		m_fButton->addChild(m_fButtonNewTab);
	}

	m_fButton->setTabWidget(tabWidget());

	connect(m_fButton, &FloatingButton::statusChanged, this, &BrowserWindow::saveButtonState);

	connect(m_fButtonViewBookmarks, &FloatingButton::isClicked, tabWidget(), &TabWidget::openBookmarkDialog);
	connect(m_fButtonViewHistory, &FloatingButton::isClicked, tabWidget(), &TabWidget::openHistoryDialog);
	connect(m_fButtonAddBookmark, &FloatingButton::isClicked, this, &BrowserWindow::openAddBookmarkDialog);
	connect(m_fButtonNewWindow, &FloatingButton::isClicked, this, &BrowserWindow::newWindow);
	connect(m_fButtonHome, &FloatingButton::isClicked, this, &BrowserWindow::goHome);
	connect(m_fButtonNext, &FloatingButton::isClicked, this, &BrowserWindow::forward);
	connect(m_fButtonBack, &FloatingButton::isClicked, this, &BrowserWindow::back);
	connect(m_fButtonNewTab, &FloatingButton::isClicked, this, &BrowserWindow::newTab);

}

void BrowserWindow::saveButtonState()
{
	QByteArray data{};
	QDataStream stream{&data, QIODevice::WriteOnly};

	stream << 0x0001;
	stream << m_fButton->children().count();

		foreach (FloatingButton* button, m_fButton->children()) stream << button->objectName();

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