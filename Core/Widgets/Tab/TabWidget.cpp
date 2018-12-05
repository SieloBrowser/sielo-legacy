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

#include "Widgets/Tab/TabWidget.hpp"

#include <QWindow>
#include <QScreen>

#include <QMenuBar>

#include <QClipboard>
#include <QShortcut>

#include "BrowserWindow.hpp"

//#include "History/HistoryDialog.hpp"
#include "History/HistoryManager.hpp"

#include "Bookmarks/BookmarksManager.hpp"

#include "Download/DownloadManager.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Utils/ClosedTabsManager.hpp"
#include "Utils/AutoSaver.hpp"
#include "Utils/Settings.hpp"
#include "Utils/SideBarManager.hpp"

#include "Web/WebPage.hpp"
#include "Web/WebView.hpp"
#include "Web/WebInspector.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/StatusBarMessage.hpp"
#include "Widgets/NavigationBar.hpp"
#include "Widgets/MainMenu.hpp"
#include "Widgets/AddressBar/AddressBar.hpp"
#include "Widgets/Preferences/PreferencesDialog.hpp"
#include "Widgets/SideBar/SideBar.hpp"
#include "Widgets/SideBar/SideBar.hpp"
#include "Widgets/Tab/MainTabBar.hpp"
#include "Widgets/Tab/TabIcon.hpp"
#include "Widgets/Tab/MenuTabs.hpp"
#include "Widgets/Tab/AddTabButton.hpp"

//#include "Widgets/Tab/TabLabel.hpp"

namespace Sn
{
TabWidget::TabWidget(BrowserWindow* window, Application::TabsSpaceType type, QWidget* parent) :
	TabStackedWidget(parent),
	m_saveTimer(new AutoSaver(this)),
	m_window(window),
	m_tabsSpaceType(type),
	m_statusBarMessage(new StatusBarMessage(this))
{
	setObjectName(QLatin1String("tabwidget"));

	m_closedTabsManager = new ClosedTabsManager;
	
	m_tabBar = new MainTabBar(this);
	m_menuTabs = new MenuTabs(this);
	m_menuClosedTabs = new QMenu(this);

	m_buttonAddTab = new AddTabButton(this, m_tabBar);
	m_buttonAddTab->setProperty("outside-tabbar", false);
	m_buttonAddTab->setIcon(Application::getAppIcon("tabbar-addtab", "tabs"));

	m_buttonAddTab2 = new AddTabButton(this, m_tabBar);
	m_buttonAddTab2->setProperty("outside-tabbar", true);
	m_buttonAddTab2->setIcon(Application::getAppIcon("tabbar-addtab", "tabs"));
	m_buttonAddTab2->hide();

	m_buttonClosedTabs = new ToolButton(m_tabBar);
	m_buttonClosedTabs->setObjectName(QLatin1String("tabwidget-button-closedtabs"));
	m_buttonClosedTabs->setMenu(m_menuClosedTabs);
	m_buttonClosedTabs->setPopupMode(QToolButton::InstantPopup);
	m_buttonClosedTabs->setToolTip(tr("Closed tabs"));
	m_buttonClosedTabs->setAutoRaise(true);
	m_buttonClosedTabs->setFocusPolicy(Qt::NoFocus);
	m_buttonClosedTabs->setShowMenuInside(true);

	m_buttonListTabs = new ToolButton(m_tabBar);
	m_buttonListTabs->setObjectName(QLatin1String("tabwidget-button-listtabs"));
	m_buttonListTabs->setMenu(m_menuTabs);
	m_buttonListTabs->setPopupMode(QToolButton::InstantPopup);
	m_buttonListTabs->setToolTip(tr("List of Tabs"));
	m_buttonListTabs->setAutoRaise(true);
	m_buttonListTabs->setFocusPolicy(Qt::NoFocus);
	m_buttonListTabs->setShowMenuInside(true);
	m_buttonListTabs->setIcon(Application::getAppIcon("tabbar-tabslist", "tabs"));
	m_buttonListTabs->hide();

	m_buttonMainMenu = new ToolButton(m_tabBar);
	m_buttonMainMenu->setObjectName(QLatin1String("tabwidget-button-mainmenu"));
	m_buttonMainMenu->setMenu(new MainMenu(this, this));
	m_buttonMainMenu->setPopupMode(QToolButton::InstantPopup);
	m_buttonMainMenu->setToolTip(tr("Preferences"));
	m_buttonMainMenu->setAutoRaise(true);
	m_buttonMainMenu->setFocusPolicy(Qt::NoFocus);
	m_buttonMainMenu->setIcon(Application::getAppIcon("preferences", "preferences"));
	m_buttonMainMenu->setShowMenuInside(true);

#ifdef Q_OS_MACOS
	static MainMenu* macMainMenu = 0;

	if (!macMainMenu) {
		macMainMenu = new MainMenu(this, nullptr);
		macMainMenu->initMenuBar(new QMenuBar(nullptr));

		auto windowChanged = [this](BrowserWindow* window) {
			macMainMenu->disconnect(macMainMenu->tabWidget());
			connect(window, &BrowserWindow::tabWidgetChanged, macMainMenu, &MainMenu::setTabWidget);

			macMainMenu->setTabWidget(window->tabWidget());
		};

		connect(m_window, &BrowserWindow::tabWidgetChanged, macMainMenu, &MainMenu::setTabWidget);
		connect(Application::instance(), &Application::activeWindowChanged, this, windowChanged);
	}
	else {
		macMainMenu->setTabWidget(this);
	}
#endif

	m_tabBar->addCornerWidget(m_buttonAddTab2, Qt::TopRightCorner);
	m_tabBar->addCornerWidget(m_buttonClosedTabs, Qt::TopRightCorner);
	m_tabBar->addCornerWidget(m_buttonListTabs, Qt::TopRightCorner);
	m_tabBar->addCornerWidget(m_buttonMainMenu, Qt::TopRightCorner);

	connect(this, &TabWidget::changed, m_saveTimer, &AutoSaver::changeOccurred);
	connect(this, &TabStackedWidget::pinStateChanged, this, &TabWidget::changed);

	connect(m_tabBar, &MainTabBar::tabCloseRequested, this, &TabWidget::requestCloseTab);
	connect(m_tabBar, &MainTabBar::tabMoved, this, &TabWidget::tabMoved);
	connect(m_tabBar, &MainTabBar::moveAddTabButton, this, &TabWidget::moveAddTabButton);
	connect(m_tabBar, &MainTabBar::overFlowChanged, this, &TabWidget::tabBarOverFlowChanged);

	connect(m_menuTabs, &MenuTabs::closeTab, this, &TabWidget::requestCloseTab);

	connect(m_buttonAddTab, SIGNAL(clicked()), this, SLOT(addTab()));
	connect(m_buttonAddTab2, SIGNAL(clicked()), this, SLOT(addTab()));
	connect(m_buttonClosedTabs, &ToolButton::aboutToShowMenu, this, &TabWidget::aboutToShowClosedTabsMenu);
	connect(m_buttonListTabs, &ToolButton::aboutToShowMenu, this, &TabWidget::aboutToShowTabsMenu);

	///**** Shortcuts ****///
	QShortcut* reloadBypassCacheAction{new QShortcut(QKeySequence("Ctrl+F5"), this)};
	QShortcut* reloadBypassCacheAction2{new QShortcut(QKeySequence("Ctrl+Shift+R"), this)};
	QShortcut* reloadAction{new QShortcut(QKeySequence("F5"), this)};
	QShortcut* reloadAction2{new QShortcut(QKeySequence("Ctrl+R"), this)};
	QShortcut* closeTabAction{new QShortcut(QKeySequence("Ctrl+F4"), this)};
	QShortcut* closeTabAction2{new QShortcut(QKeySequence("Ctrl+W"), this)};
	QShortcut* windowInFullScreenAction{new QShortcut(QKeySequence("F11"), this)};
	QShortcut* tabsSpaceInFullScreenAction{new QShortcut(QKeySequence("Ctrl+F11"), this)};
	QShortcut* reorganizeTabsSpaces{new QShortcut(QKeySequence("F10"), this)};
	QShortcut* showInspectorAction{new QShortcut(QKeySequence("F12"), this)};
	QShortcut* showSourceAction{new QShortcut(QKeySequence("Ctrl+U"), this)};

	reloadBypassCacheAction->setContext(Qt::WidgetWithChildrenShortcut);
	reloadBypassCacheAction2->setContext(Qt::WidgetWithChildrenShortcut);
	reloadAction->setContext(Qt::WidgetWithChildrenShortcut);
	reloadAction2->setContext(Qt::WidgetWithChildrenShortcut);
	closeTabAction->setContext(Qt::WidgetWithChildrenShortcut);
	closeTabAction2->setContext(Qt::WidgetWithChildrenShortcut);
	windowInFullScreenAction->setContext(Qt::WidgetWithChildrenShortcut);
	tabsSpaceInFullScreenAction->setContext(Qt::WidgetWithChildrenShortcut);
	reorganizeTabsSpaces->setContext(Qt::WidgetWithChildrenShortcut);
	showInspectorAction->setContext(Qt::WidgetWithChildrenShortcut);
	showSourceAction->setContext(Qt::WidgetWithChildrenShortcut);

	connect(reloadBypassCacheAction, &QShortcut::activated, this, &TabWidget::reloadBypassCache);
	connect(reloadBypassCacheAction2, &QShortcut::activated, this, &TabWidget::reloadBypassCache);
	connect(reloadAction, &QShortcut::activated, this, &TabWidget::reload);
	connect(reloadAction2, &QShortcut::activated, this, &TabWidget::reload);
	connect(closeTabAction, SIGNAL(activated()), this, SLOT(requestCloseTab()));
	connect(closeTabAction2, SIGNAL(activated()), this, SLOT(requestCloseTab()));
	connect(windowInFullScreenAction, &QShortcut::activated, m_window, &BrowserWindow::toggleFullScreen);
	connect(tabsSpaceInFullScreenAction, &QShortcut::activated, this, &TabWidget::toggleFullScreen);
	connect(reorganizeTabsSpaces, &QShortcut::activated, m_window->tabsSpaceSplitter(), &TabsSpaceSplitter::autoResize);
	connect(showInspectorAction, SIGNAL(activated()), this, SLOT(showInspector()));
	connect(showSourceAction, &QShortcut::activated, this, &TabWidget::showSource);
	///**** End of shortcuts ****///

	setTabBar(m_tabBar);
	loadSettings();

	Application::instance()->plugins()->emitTabsSpaceCreated(this);
}

TabWidget::~TabWidget()
{
	Application::instance()->plugins()->emitTabsSpaceDeleted(this);
	delete m_closedTabsManager;
}

void TabWidget::loadSettings()
{
	Settings settings{};

	settings.beginGroup("Tabs-Settings");

	m_dontCloseWithOneTab = settings.value("dontCloseWidthOneTab", false).toBool();
	m_showClosedTabsButton = settings.value("showClosedTabsButton", false).toBool();
	m_newTabAfterActive = settings.value("newTabAfterActive", true).toBool();
	m_newEmptyTabAfterActive = settings.value("newEmptyTabsAfterActive", false).toBool();

	settings.endGroup();

	settings.beginGroup("SideBars");

	QString activeSideBar{settings.value("Active", QString()).toString()};

	if (activeSideBar.isEmpty() && sideBar())
		sideBar()->close();
	else
		sideBarManager()->showSideBar(activeSideBar, false);

	if (m_tabsSpaceType == Application::TST_Web) {
		settings.beginGroup("Web-Settings");

		//TODO: Modify for a custom Sielo start page
		m_urlOnNewTab = settings.value("urlOnNewTab", "https://doosearch.sielo.app/").toUrl();
		if (m_homeUrl.isEmpty())
			m_homeUrl = m_window->homePageUrl();

		settings.endGroup();
	}

	if (Application::instance()->useTopToolBar()) {
		setupNavigationBar();
	}
	else if (!Application::instance()->useTopToolBar() && m_navigationToolBar) {
		for (int i{0}; i < count(); ++i) {
			WebTab* tab{ weTab(i) };
			m_addressBars->removeWidget(tab->addressBar());
			tab->takeAddressBar();
		}
		delete m_addressBars;
		delete m_navigationToolBar;

		m_addressBars = nullptr;
		m_navigationToolBar = nullptr;
	}

	updateClosedTabsButton();
}

QByteArray TabWidget::saveState()
{
	QVector<WebTab::SavedTab> tabList;

	for (int i{0}; i < count(); ++i) {
		WebTab* webTab{weTab(i)};

		if (!webTab)
			continue;

		WebTab::SavedTab tab{webTab};
		tabList.append(tab);
	}

	QByteArray data;
	QDataStream stream{&data, QIODevice::WriteOnly};

	stream << m_homeUrl;

	int tabListCount = tabList.count();
	stream << tabListCount;

	foreach (const WebTab::SavedTab& tab, tabList) stream << tab;

	stream << currentIndex();

	return data;
}

void TabWidget::save()
{
	Application::instance()->saveSession();
}

bool TabWidget::restoreState(const QVector<WebTab::SavedTab>& tabs, int currentTab, const QUrl& homeUrl)
{
	if (tabs.isEmpty())
		return false;

	m_homeUrl = homeUrl;

	if (m_homeUrl.isEmpty())
		m_homeUrl = m_window->homePageUrl();

	for (WebTab::SavedTab tab : tabs) {
		int index{addView(QUrl(), Application::NTT_CleanSelectedTab, false, tab.isPinned)};
		weTab(index)->restoreTab(tab);
	}

	setCurrentIndex(currentTab);
	QTimer::singleShot(0, m_tabBar, SLOT(ensureVisible()));

	weTab()->hide();
	weTab()->show();

	return true;
}

void TabWidget::setCurrentIndex(int index)
{
	TabStackedWidget::setCurrentIndex(index);
}

void TabWidget::goToApplication(QWidget* w)
{
	int index{-1};

	for (int i{0}; i < count(); ++i) {
		if (w == qobject_cast<WebTab*>(widget(i))->application()) {
			index = i;
			break;
		}
	}

	if (index != -1)
		TabStackedWidget::setCurrentIndex(index);
}

void TabWidget::nextTab()
{
	setCurrentIndex((currentIndex() + 1) % count());
}

void TabWidget::previousTab()
{
	setCurrentIndex(currentIndex() == 0 ? count() - 1 : currentIndex() - 1);
}

void TabWidget::currentTabChanged(int index)
{
	if (!validIndex(index))
		return;

	WebTab* currentTab{weTab(index)};
	currentTab->tabActivated();
	WebTab* oldTab{weTab()};

	disconnect(oldTab->webView()->page(), &WebPage::fullScreenRequested, this, &TabWidget::fullScreenRequested);
	connect(currentTab->webView()->page(), &WebPage::fullScreenRequested, this, &TabWidget::fullScreenRequested);

	if (Application::instance()->useTopToolBar()) {
		AddressBar* addressBar = currentTab->addressBar();
		if (addressBar && m_addressBars->indexOf(addressBar) != -1)
			m_addressBars->setCurrentWidget(addressBar);
	}

	m_lastBackgroundTab = nullptr;
	m_currentTabFresh = false;

	m_window->currentTabChanged(currentTab);

	if (m_tabsSpaceType != Application::TST_Web && currentTab->addressBar()) {
		/*currentTab->addressBar()->setEnabled(false);
		currentTab->addressBar()->setText("sielo:inspector");
		currentTab->setUpdatesEnabled(false);*/
		currentTab->addressBar()->deleteLater();
	}

	emit changed();
}

int TabWidget::normalTabsCount() const
{
	return m_tabBar->normalTabsCount();
}

int TabWidget::pinnedTabsCount() const
{
	return m_tabBar->pinnedTabsCount();
}

int TabWidget::extraReservedWidth() const
{
	return m_buttonAddTab->width();
}

WebTab* TabWidget::webTab(int index) const
{
	return index < 0 ? weTab() : weTab(index);
}

QList<WebTab*> TabWidget::allTabs(bool withPinned)
{
	QList<WebTab*> allTabs;

	for (int i{0}; i < count(); ++i) {
		WebTab* webTab{weTab(i)};

		if (!webTab || (!withPinned && webTab->isPinned()))
			continue;

		allTabs.append(webTab);
	}

	return allTabs;
}

bool TabWidget::canRestoreTab() const
{
	return m_closedTabsManager->isClosedTabAvailable();
}

void TabWidget::setCurrentTabFresh(bool currentTabFresh)
{
	m_currentTabFresh = currentTabFresh;
}

void TabWidget::setHomeUrl(const QString& newUrl)
{
	m_homeUrl = QUrl(newUrl);
}

void TabWidget::moveTab(int from, int to)
{
	WebTab* tab{webTab(from)};
	
	if (!tab)
		return;

	if ((tab->isPinned() && to >= pinnedTabsCount()) || (!tab->isPinned() && to < pinnedTabsCount()))
		tab->togglePinned();

	TabStackedWidget::moveTab(from, to);
}

int TabWidget::pinUnPinTab(int index, const QString& title)
{
	const int newIndex{TabStackedWidget::pinUnPinTab(index, title)};

	if (index != newIndex)
		emit tabMoved(index, newIndex);

	return newIndex;
}

void TabWidget::detachTab(WebTab* tab)
{
	Q_ASSERT(tab);

	if (count() == 1 && m_window->tabsSpaceSplitter()->count() == 1 && Application::instance()->windowCount() == 1)
		return;

	if (Application::instance()->useTopToolBar())
		m_addressBars->removeWidget(tab->addressBar());

	disconnect(tab->webView(), &TabbedWebView::wantsCloseTab, this, &TabWidget::closeTab);
	disconnect(tab->webView(), SIGNAL(urlChanged(QUrl)), this, SIGNAL(changed()));

	const int index{tab->tabIndex()};

	tab->detach();
	tab->setPinned(false);

	emit tabRemoved(index);

	if (count() == 0) {
		if (m_window->tabsSpaceSplitter()->count() > 1)
			m_window->tabsSpaceSplitter()->removeTabsSpace(this);
		else
			m_window->close();
	}
}

void TabWidget::addTab()
{
	addView(QUrl(), Application::NTT_SelectedNewEmptyTab, true);
	setCurrentTabFresh(true);
}

int TabWidget::addView(const QUrl& url)
{
	LoadRequest request{};
	request.setUrl(url);

	return addView(request);
}

int TabWidget::addView(const LoadRequest& request, const Application::NewTabTypeFlags& openFlags, bool selectLine,
                       bool pinned)
{
	return addView(request, QString(), openFlags, selectLine, -1, pinned);
}

int TabWidget::addView(const LoadRequest& request, const QString& title, const Application::NewTabTypeFlags& openFlags,
                       bool selectLine, int position, bool pinned)
{
	QUrl url{request.url()};
	m_currentTabFresh = false;

	if (url.isEmpty() && !(openFlags & Application::NTT_CleanTab))
		url = m_urlOnNewTab;

	bool openAfterActive{m_newTabAfterActive && !(openFlags & Application::NTT_TabAtEnd)};

	if (openFlags == Application::NTT_SelectedNewEmptyTab && m_newEmptyTabAfterActive)
		openAfterActive = true;

	if (openAfterActive && position == -1) {
		if (openFlags & Application::NTT_NotSelectedTab && m_lastBackgroundTab)
			position = m_lastBackgroundTab->tabIndex() + 1;
		else
			position = qMax(currentIndex() + 1, m_tabBar->pinnedTabsCount());
	}

	WebTab* webTab{new WebTab(this)};
	webTab->setPinned(pinned);
	webTab->addressBar()->showUrl(url);
	if (Application::instance()->useTopToolBar())
		m_addressBars->addWidget(webTab->addressBar());

	int index{insertTab(position == -1 ? count() : position, webTab, QString(), pinned)};

	webTab->attach(this);

	if (!title.isEmpty())
		m_tabBar->setTabText(index, title);

	if (openFlags & Application::NTT_SelectedTab)
		setCurrentIndex(index);
	else
		m_lastBackgroundTab = webTab;

	connect(webTab->webView(), &TabbedWebView::wantsCloseTab, this, &TabWidget::closeTab);
	connect(webTab->webView(), SIGNAL(urlChanged(QUrl)), this, SIGNAL(changed()));
	connect(webTab->webView(), &WebView::urlChanged, this, [this](const QUrl& url) {
		if (url != m_urlOnNewTab)
			m_currentTabFresh = false;
	});

	if (url.isValid() && url != request.url()) {
		LoadRequest req{request};
		req.setUrl(url);
		webTab->webView()->load(req);
	}
	else if (request.url().isValid())
		webTab->webView()->load(request);

	if (selectLine && weTab()->addressBar()->text().isEmpty())
		weTab()->addressBar()->setFocus();

	if (!(openFlags & Application::NTT_SelectedTab))
		m_tabBar->ensureVisible(index);

	emit changed();
	emit tabInserted(index);

	return index;
}

int TabWidget::addView(WebTab* tab, const Application::NewTabTypeFlags& openFlags)
{
	return insertView(count() + 1, tab, openFlags);
}

int TabWidget::insertView(int index, WebTab* tab, const Application::NewTabTypeFlags& openFlags)
{
	if (Application::instance()->useTopToolBar())
		m_addressBars->addWidget(tab->addressBar());

	int newIndex{insertTab(index, tab, QString(), tab->isPinned())};

	tab->attach(this);

	if (openFlags.testFlag(Application::NTT_SelectedTab))
		setCurrentIndex(newIndex);
	else
		m_lastBackgroundTab = tab;

	connect(tab->webView(), &TabbedWebView::wantsCloseTab, this, &TabWidget::closeTab);
	connect(tab->webView(), SIGNAL(urlChanged(QUrl)), this, SIGNAL(changed()));

	if (!(openFlags & Application::NTT_SelectedTab))
		m_tabBar->ensureVisible();

	emit changed();
	emit tabInserted(newIndex);

	return newIndex;
}

int TabWidget::addApplication(QWidget* application)
{
	WebTab* tab{new WebTab(this)};
	tab->loadApplication(application);

	int index{addView(tab, Application::NTT_SelectedTabAtEnd)};

	return index;

}
int TabWidget::insertApplication(int index, QWidget* application)
{
	return -1;
}

void TabWidget::addTabFromClipboard()
{
	QString selectionClipboard{QApplication::clipboard()->text(QClipboard::Selection)};
	QUrl guessedUrl{QUrl::fromUserInput(selectionClipboard)};

	if (!guessedUrl.isEmpty())
		addView(guessedUrl, Application::NTT_SelectedNewEmptyTab);
}

int TabWidget::duplicateTab(int index)
{
	if (!validIndex(index))
		return -1;

	WebTab* webTab{weTab(index)};

	int id{addView(QUrl(), webTab->title(), Application::NTT_CleanNotSelectedTab)};
	weTab(id)->p_restoreTab(webTab->url(), webTab->historyData(), webTab->zoomLevel());

	return id;
}

void TabWidget::closeTab(int index)
{
	if (index == -1)
		index = currentIndex();

	WebTab* webTab{weTab(index)};
	if (!webTab || !validIndex(index))
		return;

	if (count() <= 1) {
		requestCloseTab(index);
		return;
	}

	m_closedTabsManager->saveTab(webTab, index);

	TabbedWebView* webView{webTab->webView()};
	
	if (Application::instance()->useTopToolBar())
		m_addressBars->removeWidget(webView->webTab()->addressBar());

	disconnect(webView, &TabbedWebView::wantsCloseTab, this, &TabWidget::closeTab);
	disconnect(webView, SIGNAL(urlChanged(QUrl)), this, SIGNAL(changed()));

	m_lastBackgroundTab = nullptr;

	if (m_menuTabs->isVisible()) {
		QAction* labelAction{m_menuTabs->actions().last()};
		labelAction->setText(tr("Currently you have %n opened tab(s)", "", count() - 1));
	}

	webTab->detach();
	webTab->deleteLater();

	updateClosedTabsButton();

	emit changed();
	emit tabRemoved(index);
}

void TabWidget::requestCloseTab(int index)
{
	if (index == -1)
		index = currentIndex();

	WebTab* webTab{weTab(index)};
	if (!webTab || !validIndex(index))
		return;

	TabbedWebView* webView{webTab->webView()};

	//TODO: block close of restore tab

	if (count() <= 1 && m_window->tabsSpaceSplitter()->count() == 1) {
		if (m_dontCloseWithOneTab) {
			if (webView->url() == m_urlOnNewTab)
				m_closedTabsManager->takeLastClosedTab();
			webView->load(m_urlOnNewTab);
			return;
		}
		m_window->close();
		return;
	}
	else if (count() == 1) {
		m_window->tabsSpaceSplitter()->removeTabsSpace(this);
		return;
	}

	webView->triggerPageAction(QWebEnginePage::RequestClose);
}

void TabWidget::reloadTab(int index)
{
	if (!validIndex(index))
		return;

	weTab(index)->reload();
}

void TabWidget::reloadAllTabs()
{
	for (int i{0}; i < count(); ++i)
		reloadTab(i);
}

void TabWidget::reload()
{
	weTab()->reload();
}

void TabWidget::reloadBypassCache()
{
	weTab()->webView()->reloadBypassCache();
}

void TabWidget::stopTab(int index)
{
	if (!validIndex(index))
		return;

	weTab(index)->stop();
}

void TabWidget::closeAllButCurrent(int index)
{
	if (!validIndex(index))
		return;

	WebTab* akt{weTab(index)};

	foreach (WebTab* tab, allTabs(false)) {
		int tabIndex{tab->tabIndex()};

		if (akt == widget(tabIndex))
			continue;

		requestCloseTab(tabIndex);
	}
}

void TabWidget::closeToRight(int index)
{
	if (!validIndex(index))
		return;

	foreach (WebTab* tab, allTabs(false)) {
		int tabIndex{tab->tabIndex()};

		if (index >= tabIndex)
			continue;

		requestCloseTab(tabIndex);
	}
}

void TabWidget::closeToLeft(int index)
{
	if (!validIndex(index))
		return;

	foreach (WebTab* tab, allTabs(false)) {
		int tabIndex{tab->tabIndex()};

		if (index <= tabIndex)
			continue;

		requestCloseTab(tabIndex);
	}
}

void TabWidget::detachTab(int index)
{
	WebTab* webTab{weTab(index)};
	Q_ASSERT(webTab);

	if (count() == 1 && m_window->tabsSpaceSplitter()->count() == 1 && Application::instance()->windowCount() == 1)
		return;

	detachTab(webTab);

	BrowserWindow* window{Application::instance()->createWindow(Application::WT_NewWindow)};
	window->setStartTab(webTab);
}

void TabWidget::loadTab(int index)
{
	if (!validIndex(index))
		return;

	weTab(index)->tabActivated();
}

void TabWidget::unloadTab(int index)
{
	if (!validIndex(index))
		return;

	weTab(index)->unload();
}

void TabWidget::restoreClosedTab(QObject* obj)
{
	if (!obj)
		obj = sender();

	if (!m_closedTabsManager->isClosedTabAvailable())
		return;

	ClosedTabsManager::Tab tab;
	QAction* action = qobject_cast<QAction*>(obj);

	if (action && action->data().toInt() != 0)
		tab = m_closedTabsManager->takeTabAt(action->data().toInt());
	else
		tab = m_closedTabsManager->takeLastClosedTab();

	if (tab.position < 0)
		return;

	int index{addView(QUrl(), tab.title, Application::NTT_CleanSelectedTab, false, tab.position)};
	WebTab* webTab{weTab(index)};
	webTab->p_restoreTab(tab.url, tab.history, tab.zoomLevel);

	updateClosedTabsButton();
}

void TabWidget::restoreAllClosedTabs()
{
	if (!m_closedTabsManager->isClosedTabAvailable())
		return;

	const QLinkedList<ClosedTabsManager::Tab>& closedTabs = m_closedTabsManager->allClosedTab();

	foreach (const ClosedTabsManager::Tab& tab, closedTabs) {
		int index{addView(QUrl(), tab.title, Application::NTT_CleanSelectedTab)};
		WebTab* webTab{weTab(index)};
		webTab->p_restoreTab(tab.url, tab.history, tab.zoomLevel);
	}

	clearClosedTabsList();
}

void TabWidget::clearClosedTabsList()
{
	m_closedTabsManager->clearList();
	updateClosedTabsButton();
}

void TabWidget::showSource()
{
	weTab()->webView()->showSource();
}

void TabWidget::showInspector(WebTab* webTab)
{
	if (!webTab)
		webTab = weTab();

	webTab->toggleWebInspector();
}

void TabWidget::fullScreenRequested(QWebEngineFullScreenRequest request)
{
	WebPage* webPage = qobject_cast<WebPage*>(sender());

	if (request.toggleOn()) {
		if (!m_fullScreenView) {
			m_fullScreenView = new QWebEngineView();

			QAction* exitFullScreenAction = new QAction(m_fullScreenView);
			exitFullScreenAction->setShortcut(Qt::Key_Escape);

			connect(exitFullScreenAction, &QAction::triggered, [webPage] {
				webPage->triggerAction(QWebEnginePage::ExitFullScreen);
			});

			m_fullScreenView->addAction(exitFullScreenAction);
		}

		webPage->setView(m_fullScreenView);

		request.accept();

		QScreen* screen{m_window->windowHandle()->screen()};

		m_fullScreenView->move(screen->geometry().x(), screen->geometry().y());
		m_fullScreenView->resize(screen->geometry().width(), screen->geometry().height());

		m_fullScreenView->showFullScreen();
		m_fullScreenView->raise();
	}
	else {
		if (!m_fullScreenView)
			return;

		TabbedWebView* oldWebView{weTab()->webView()};
		webPage->setView(oldWebView);

		request.accept();

		delete m_fullScreenView;
		m_fullScreenView = nullptr;
	}
}

void TabWidget::toggleFullScreen()
{
	m_window->tabsSpaceSplitter()->tabsSpaceInFullView(this);
}

void TabWidget::moveAddTabButton(int posX)
{
	int posY{(m_tabBar->height() - m_buttonAddTab->height()) / 2};

	if (QApplication::layoutDirection() == Qt::RightToLeft)
		posX = qMax(posX - m_buttonAddTab->width(), 0);
	else
		posX = qMin(posX, m_tabBar->width() - m_buttonAddTab->width());

	m_buttonAddTab->move(posX, posY);
}

void TabWidget::updateShowBookmarksBarText(bool visible)
{
	MainMenu* mainMenu = qobject_cast<MainMenu*>(m_buttonMainMenu->menu());
	mainMenu->updateShowBookmarksBarText(visible);
}

void TabWidget::tabBarOverFlowChanged(bool overflowed)
{
	m_buttonAddTab->setVisible(!overflowed);

	m_buttonAddTab2->setVisible(overflowed);
	m_buttonListTabs->setVisible(overflowed);
	m_buttonClosedTabs->setVisible(m_showClosedTabsButton);
}

void TabWidget::aboutToShowTabsMenu()
{
	m_menuTabs->clear();

	for (int i{0}; i < count(); ++i) {
		WebTab* webTab = weTab(i);
		if (!webTab || webTab->isPinned())
			continue;

		QAction* action{new QAction(this)};
		action->setIcon(webTab->icon());

		if (i == currentIndex()) {
			QFont font{action->font()};
			font.setBold(true);
			action->setFont(font);
		}

		QString title{webTab->title()};
		title.replace(QLatin1Char('&'), QLatin1String("&&"));

		if (title.length() > 40)
			title = title.left(40) + QLatin1String("...");

		action->setText(title);
		action->setData(QVariant::fromValue(qobject_cast<WebTab*>(webTab)));

		connect(action, &QAction::triggered, this, &TabWidget::actionChangeIndex);

		m_menuTabs->addAction(action);
	}
}

void TabWidget::aboutToShowClosedTabsMenu()
{
	m_menuClosedTabs->clear();

	int i{0};
	const QLinkedList<ClosedTabsManager::Tab> closedTabs = closedTabsManager()->allClosedTab();

	foreach (const ClosedTabsManager::Tab& tab, closedTabs) {
		const QString title{tab.title.length() > 40 ? tab.title.left(40) + QLatin1String("...") : tab.title};
		QAction* action{m_menuClosedTabs->addAction(tab.icon, title, this, SLOT(restoreClosedTab()))};
		action->setData(++i);
	}

	if (m_menuClosedTabs->isEmpty())
		m_menuClosedTabs->addAction(tr("Empty"))->setEnabled(false);
	else {
		m_menuClosedTabs->addSeparator();
		m_menuClosedTabs->addAction(tr("Restore All Closed Tabs"), this, &TabWidget::restoreAllClosedTabs);
		m_menuClosedTabs->addAction(tr("Clear List"), this, &TabWidget::clearClosedTabsList);
	}
}

void TabWidget::actionChangeIndex()
{
	if (QAction* action = qobject_cast<QAction*>(sender())) {
		WebTab* webTab = qobject_cast<WebTab*>(qvariant_cast<QWidget*>(action->data()));
		if (webTab) {
			m_tabBar->ensureVisible(webTab->tabIndex());
			setCurrentIndex(webTab->tabIndex());
		}
	}
}

void TabWidget::tabWasMoved(int before, int after)
{
	m_lastBackgroundTab = nullptr;

	emit changed();
	emit tabMoved(before, after);
}

WebTab *TabWidget::weTab() const
{
	return weTab(currentIndex());
}

WebTab *TabWidget::weTab(int index) const
{
	return qobject_cast<WebTab*>(widget(index));
}

TabIcon *TabWidget::tabIcon(int index) const
{
	return weTab(index)->tabIcon();
}

QAction *TabWidget::action(const QString& name) const
{
	return static_cast<MainMenu*>(m_buttonMainMenu->menu())->action(name);
}

void TabWidget::openBookmarksDialog()
{
	BookmarksManager* dialog{new BookmarksManager(m_window, m_window)};
	addApplication(dialog);
}

void TabWidget::openHistoryDialog()
{
	/*HistoryDialog* dialog(new HistoryDialog(this));
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	connect(dialog, SIGNAL(openUrl(
							   const QUrl&)), this, SLOT(addView(
															 const QUrl&)));

	dialog->show();*/
	HistoryManager* dialog{new HistoryManager(m_window, m_window)};
	addApplication(dialog);
}

bool TabWidget::validIndex(int index) const
{
	return (index >= 0 && index < count() && m_window->tabsSpaceSplitter()->count() > 0);
}

void TabWidget::updateClosedTabsButton()
{
	if (!m_showClosedTabsButton)
		m_buttonClosedTabs->hide();

	m_buttonClosedTabs->setEnabled(canRestoreTab());
}

void TabWidget::setupNavigationBar()
{
	if (m_navigationToolBar) {
		if (Application::instance()->hideBookmarksHistoryActions())
			m_navigationToolBar->hideBookmarksHistory();
		else
			m_navigationToolBar->showBookmarksHistory();

		return;
	}

	m_addressBars = new QStackedWidget(this);
	m_navigationToolBar = new NavigationToolBar(this);

	if (Application::instance()->hideBookmarksHistoryActions())
		m_navigationToolBar->hideBookmarksHistory();
	else
		m_navigationToolBar->showBookmarksHistory();

	setNavigationToolBar(m_navigationToolBar);

	if (count() <= 0)
		return;

	for (int i{ 0 }; i < count(); ++i) {
		WebTab* tab = weTab(i);
		if (tab->addressBar())
			m_addressBars->addWidget(tab->addressBar());	
	}

	AddressBar* addressBar = weTab()->addressBar();
	if (addressBar && m_addressBars->indexOf(addressBar) != -1)
		m_addressBars->setCurrentWidget(addressBar);
}

void TabWidget::keyPressEvent(QKeyEvent* event)
{
	if (Application::instance()->plugins()->processKeyPress(Application::ON_TabWidget, this, event))
		return;

	TabStackedWidget::keyPressEvent(event);
}

void TabWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (Application::instance()->plugins()->processKeyRelease(Application::ON_TabWidget, this, event))
		return;

	TabStackedWidget::keyReleaseEvent(event);
}
}
