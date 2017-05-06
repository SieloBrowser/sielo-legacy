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

#include "Widgets/Tab/TabWidget.hpp"

#include <QWindow>
#include <QScreen>

#include <QRect>

#include <QDataStream>
#include <QSettings>

#include <QKeyEvent>
#include <QClipboard>

#include "Application.hpp"
#include "BrowserWindow.hpp"

#include "History/HistoryManager.hpp"
#include "History/HistoryDialog.hpp"

#include "Bookmarks/BookmarkManager.hpp"
#include "Bookmarks/BookmarksDialog.hpp"
#include "Bookmarks/AddBookmarkDialog.hpp"

#include "Download/DownloadManager.hpp"

#include "Utils/ClosedTabsManager.hpp"
#include "Utils/ToolButton.hpp"
#include "Utils/AutoSaver.hpp"

#include "Web/WebPage.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/AddressBar.hpp"
#include "Widgets/FloatingButton.hpp"
#include "Widgets/Preferences/PreferencesDialog.hpp"
#include "Widgets/Tab/MainTabBar.hpp"
#include "Widgets/Tab/TabIcon.hpp"
#include "Widgets/Tab/MenuTabs.hpp"
#include "Widgets/Tab/AddTabButton.hpp"

namespace Sn {

TabWidget::TabWidget(BrowserWindow* window, QWidget* parent) :
	TabStackedWidget(parent),
	m_saveTimer(new AutoSaver(this)),
	m_window(window),
	m_lastTabIndex(-1),
	m_lastBackgroundTabIndex(-1)
{
	setObjectName(QLatin1String("tabwidget"));

	m_closedTabsManager = new ClosedTabsManager;

	m_tabBar = new MainTabBar(m_window, this);
	m_menuTabs = new MenuTabs(this);
	m_menuClosedTabs = new QMenu(this);

	m_buttonAddTab = new AddTabButton(this, m_tabBar);
	m_buttonAddTab->setIcon(QIcon(":icons/tabs/tabbar-addtab.png"));
	m_buttonAddTab->setFixedWidth(24);

	m_buttonAddTab2 = new AddTabButton(this, m_tabBar);
	m_buttonAddTab2->setProperty("outside-tabbar", true);
	m_buttonAddTab2->setIcon(QIcon(":icons/tabs/tabbar-addtab.png"));
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
	m_buttonListTabs->setIcon(QIcon(":icons/tabs/tabbar-tabslist.png"));
	m_buttonListTabs->hide();

	m_buttonPreferences = new ToolButton(m_tabBar);
	m_buttonPreferences->setObjectName(QLatin1String("tabwidget-button-preferences"));
	m_buttonPreferences->setToolButtonStyle(Qt::ToolButtonIconOnly);
	m_buttonPreferences->setToolBarButtonLook(true);
	m_buttonPreferences->setIcon(QIcon(QLatin1String(":icons/preferences/preferences.png")));

	m_tabBar->addCornerWidget(m_buttonAddTab2, Qt::TopRightCorner);
	m_tabBar->addCornerWidget(m_buttonClosedTabs, Qt::TopRightCorner);
	m_tabBar->addCornerWidget(m_buttonListTabs, Qt::TopRightCorner);
	m_tabBar->addCornerWidget(m_buttonPreferences, Qt::TopRightCorner);

	if (Application::instance()->useTopToolBar()) {
		m_topToolBar = new QToolBar(this);

		m_fButtonAddBookmark = new FloatingButton(this);
		m_fButtonAddBookmark->setObjectName("fbutton-add-bookmark");
		m_fButtonAddBookmark->setMovable(false);
		m_fButtonAddBookmark->setFixedSize(QSize(32, 32));

		m_fButtonViewBookmarks = new FloatingButton(this);
		m_fButtonViewBookmarks->setObjectName("fbutton-view-bookmarks");
		m_fButtonViewBookmarks->setMovable(false);
		m_fButtonViewBookmarks->setFixedSize(QSize(32, 32));

		m_fButtonViewHistory = new FloatingButton(this);
		m_fButtonViewHistory->setObjectName("fbutton-view-history");
		m_fButtonViewHistory->setMovable(false);
		m_fButtonViewHistory->setFixedSize(QSize(32, 32));

		m_fButtonNewWindow = new FloatingButton(this);
		m_fButtonNewWindow->setObjectName("fbutton-new-window");
		m_fButtonNewWindow->setMovable(false);
		m_fButtonNewWindow->setFixedSize(QSize(32, 32));

		m_fButtonHome = new FloatingButton(this);
		m_fButtonHome->setObjectName("fbutton-home");
		m_fButtonHome->setMovable(false);
		m_fButtonHome->setFixedSize(QSize(32, 32));

		m_fButtonNext = new FloatingButton(this);
		m_fButtonNext->setObjectName("fbutton-next");
		m_fButtonNext->setMovable(false);
		m_fButtonNext->setFixedSize(QSize(32, 32));
//	m_fButtonNext->setMenu(m_menuForward);

		m_fButtonBack = new FloatingButton(this);
		m_fButtonBack->setObjectName("fbutton-back");
		m_fButtonBack->setMovable(false);
		m_fButtonBack->setFixedSize(QSize(32, 32));

		m_fButtonNewTab = new FloatingButton(this);
		m_fButtonNewTab->setObjectName("fbutton-new-tab");
		m_fButtonNewTab->setMovable(false);
		m_fButtonNewTab->setFixedSize(QSize(32, 32));

		m_actionBack = m_topToolBar->addWidget(m_fButtonBack);
		m_actionNext = m_topToolBar->addWidget(m_fButtonNext);
		m_actionHome = m_topToolBar->addWidget(m_fButtonHome);
		m_actionAddBookmark = m_topToolBar->addWidget(m_fButtonAddBookmark);
		m_actionViewBookmarks = m_topToolBar->addWidget(m_fButtonViewBookmarks);
		m_actionViewHistory = m_topToolBar->addWidget(m_fButtonViewHistory);
		m_topToolBar->addSeparator();
		m_actionNewTab = m_topToolBar->addWidget(m_fButtonNewTab);
		m_actionNewWindow = m_topToolBar->addWidget(m_fButtonNewWindow);
	}
	else {
		m_fButton = new FloatingButton(this, FloatingButton::Root);
		m_fButton->setObjectName("fbutton-root");

		m_fButtonAddBookmark = new FloatingButton(this);
		m_fButtonAddBookmark->setObjectName("fbutton-add-bookmark");

		m_fButtonViewBookmarks = new FloatingButton(this);
		m_fButtonViewBookmarks->setObjectName("fbutton-view-bookmarks");

		m_fButtonViewHistory = new FloatingButton(this);
		m_fButtonViewHistory->setObjectName("fbutton-view-history");

		m_fButtonNewWindow = new FloatingButton(this);
		m_fButtonNewWindow->setObjectName("fbutton-new-window");

		m_fButtonHome = new FloatingButton(this);
		m_fButtonHome->setObjectName("fbutton-home");

		m_fButtonNext = new FloatingButton(this);
		m_fButtonNext->setObjectName("fbutton-next");
//	m_fButtonNext->setMenu(m_menuForward);

		m_fButtonBack = new FloatingButton(this);
		m_fButtonBack->setObjectName("fbutton-back");

		m_fButtonNewTab = new FloatingButton(this);
		m_fButtonNewTab->setObjectName("fbutton-new-tab");

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

		connect(m_fButton, &FloatingButton::statusChanged, this, &TabWidget::saveButtonState);
	}

	//TODO: History connection
	connect(this, &TabWidget::changed, m_saveTimer, &AutoSaver::changeOccurred);
	connect(this, &TabStackedWidget::pinStateChanged, this, &TabWidget::changed);

	connect(m_tabBar, &MainTabBar::tabCloseRequested, this, &TabWidget::requestCloseTab);
	connect(m_tabBar, SIGNAL(reloadTab(int)), this, SLOT(reloadTab(int)));
	connect(m_tabBar, SIGNAL(stopTab(int)), this, SLOT(stopTab(int)));
	connect(m_tabBar, SIGNAL(closeAllButCurrent(int)), this, SLOT(closeAllButCurrent(int)));
	connect(m_tabBar, SIGNAL(closeToRight(int)), this, SLOT(closeToRight(int)));
	connect(m_tabBar, SIGNAL(closeToLeft(int)), this, SLOT(closeToLeft(int)));
	connect(m_tabBar, SIGNAL(duplicateTab(int)), this, SLOT(duplicateTab(int)));
	connect(m_tabBar, SIGNAL(detachTab(int)), this, SLOT(detachTab(int)));
	connect(m_tabBar, &MainTabBar::tabMoved, this, &TabWidget::tabMoved);
	connect(m_tabBar, &MainTabBar::moveAddTabButton, this, &TabWidget::moveAddTabButton);
	connect(m_tabBar, &MainTabBar::overFlowChanged, this, &TabWidget::tabBarOverFlowChanged);

	connect(m_menuTabs, &MenuTabs::closeTab, this, &TabWidget::requestCloseTab);

	connect(m_buttonAddTab, SIGNAL(clicked()), m_window, SLOT(addTab()));
	connect(m_buttonAddTab2, SIGNAL(clicked()), m_window, SLOT(addTab()));
	connect(m_buttonClosedTabs, &ToolButton::aboutToShowMenu, this, &TabWidget::aboutToShowClosedTabsMenu);
	connect(m_buttonListTabs, &ToolButton::aboutToShowMenu, this, &TabWidget::aboutToShowTabsMenu);
	connect(m_buttonPreferences, &ToolButton::clicked, this, &TabWidget::openPreferencesDialog);

	connect(m_fButtonAddBookmark, &FloatingButton::isClicked, this, &TabWidget::openAddBookmarkDialog);
	connect(m_fButtonViewBookmarks, &FloatingButton::isClicked, this, &TabWidget::openBookmarkDialog);
	connect(m_fButtonViewHistory, &FloatingButton::isClicked, this, &TabWidget::openHistoryDialog);

	setTabBar(m_tabBar);
	loadSettings();
}

TabWidget::~TabWidget()
{
	m_saveTimer->saveIfNeccessary();
	delete m_closedTabsManager;
}

void TabWidget::loadSettings()
{
	QSettings settings{};

	settings.beginGroup("Tabs-Settings");

	m_dontCloseWithOneTab = settings.value("dontCloseWidthOneTab", false).toBool();
	m_showClosedTabsButton = settings.value("showClosedTabsButton", false).toBool();
	m_newTabAfterActive = settings.value("newTabAfterActive", true).toBool();
	m_newEmptyTabAfterActive = settings.value("newEmptyTabsAfterActive", false).toBool();

	settings.endGroup();

	settings.beginGroup("Web-Settings");

	//TODO: Modify for a custom Sielo start page
	m_urlOnNewTab = settings.value("urlOnNewTab", "https://ecosia.org").toUrl();

	settings.endGroup();

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

	int tabListCount = tabList.count();
	stream << tabListCount;

		foreach (const WebTab::SavedTab& tab, tabList) stream << tab;

	int intCurrentIndex = currentIndex();
	stream << currentIndex();

	return data;
}

void TabWidget::saveButtonState()
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

void TabWidget::save()
{
	Application::instance()->saveSession();

	saveButtonState();
}

bool TabWidget::restoreState(const QVector<WebTab::SavedTab>& tabs, int currentTab)
{
	for (WebTab::SavedTab tab : tabs) {
		int index{addView(QUrl(), Application::NTT_CleanSelectedTab, false, tab.isPinned)};
		weTab(index)->restoreTab(tab);

		if (tab.isPinned)
			m_tabBar->updatePinnedTabCloseButton(index);
	}

	setCurrentIndex(currentTab);
	QTimer::singleShot(0, m_tabBar, SLOT(ensureVisible()));

	weTab()->hide();
	weTab()->show();

	return true;
}

void TabWidget::closeRecoveryTab()
{
		foreach (WebTab* tab, allTabs(false)) {
			if (tab->url().toString() == QLatin1String("sielo:restore"))
				closeTab(tab->tabIndex());
		}
}

void TabWidget::setCurrentIndex(int index)
{
	m_lastTabIndex = currentIndex();

	TabStackedWidget::setCurrentIndex(index);
}

void TabWidget::nextTab()
{
	QKeyEvent fakeEvent{QKeyEvent::KeyPress, Qt::Key_Tab, Qt::ControlModifier};
	keyPressEvent(&fakeEvent);
}
void TabWidget::previousTab()
{
	QKeyEvent fakeEvent
		{QKeyEvent::KeyPress, Qt::Key_Backtab, QFlags<Qt::KeyboardModifier>(Qt::ControlModifier + Qt::ShiftModifier)};
	keyPressEvent(&fakeEvent);
}

void TabWidget::currentTabChanged(int index)
{
	if (!validIndex(index))
		return;

	WebTab* currentTab{weTab(index)};
	WebTab* oldTab{weTab()};

	disconnect(oldTab->webView()->page()->profile(),
			   &QWebEngineProfile::downloadRequested,
			   this,
			   &TabWidget::downloadRequested);
	disconnect(oldTab->webView()->page(), &WebPage::fullScreenRequested, this, &TabWidget::fullScreenRequested);
	connect(currentTab->webView()->page()->profile(),
			&QWebEngineProfile::downloadRequested,
			this,
			&TabWidget::downloadRequested);
	connect(currentTab->webView()->page(), &WebPage::fullScreenRequested, this, &TabWidget::fullScreenRequested);

	updateFloatingButton(index);

	m_lastBackgroundTabIndex = -1;
	m_lastTabIndex = index;
	m_currentTabFresh = false;

	m_window->currentTabChanged();

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

int TabWidget::lastTabIndex() const
{
	return m_lastTabIndex;
}

int TabWidget::extraReservedWidth() const
{
	return m_buttonAddTab->width();
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
	m_lastTabIndex = currentIndex();
	m_currentTabFresh = false;

	if (url.isEmpty() && !(openFlags & Application::NTT_CleanTab))
		url = m_urlOnNewTab;

	bool openAfterActive{m_newTabAfterActive && !(openFlags & Application::NTT_TabAtEnd)};

	if (openFlags == Application::NTT_SelectedNewEmptyTab && m_newEmptyTabAfterActive)
		openAfterActive = true;

	if (openAfterActive && position == -1) {
		if (openFlags & Application::NTT_NotSelectedTab && m_lastBackgroundTabIndex != -1)
			position = m_lastBackgroundTabIndex;
		else
			position = qMax(currentIndex() + 1, m_tabBar->pinnedTabsCount());
	}

	WebTab* webTab{new WebTab(m_window)};
	webTab->addressBar()->showUrl(url);
	int index{insertTab(position == -1 ? count() : position, webTab, QString(), pinned)};

	webTab->attach(m_window);

	if (!title.isEmpty())
		m_tabBar->setTabText(index, title);

	if (openFlags & Application::NTT_SelectedTab)
		setCurrentIndex(index);
	else
		m_lastBackgroundTabIndex = index;

	connect(webTab->webView(), &TabbedWebView::wantsCloseTab, this, &TabWidget::closeTab);
	connect(webTab->webView(), SIGNAL(urlChanged(QUrl)), this, SIGNAL(changed()));
	connect(webTab->webView(), &WebView::urlChanged, this, [this](const QUrl& url)
	{
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

	if (!(openFlags & Application::NTT_SelectedTab))
		m_tabBar->ensureVisible(index);

	emit changed();
	return index;
}

int TabWidget::addView(WebTab* tab)
{
	int index{addTab(tab, QString())};
	tab->attach(m_window);

	connect(tab->webView(), &TabbedWebView::wantsCloseTab, this, &TabWidget::closeTab);
	connect(tab->webView(), SIGNAL(urlChanged(QUrl)), this, SIGNAL(changed()));

	return index;
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

	TabbedWebView* webView{webTab->webView()};

	if (webView->url().toString() != QLatin1String("sielo:restore"))
		m_closedTabsManager->saveTab(webTab, index);

	disconnect(webView, &TabbedWebView::wantsCloseTab, this, &TabWidget::closeTab);
	disconnect(webView, SIGNAL(urlChanged(QUrl)), this, SIGNAL(changed()));

	m_lastBackgroundTabIndex = -1;

	if (m_menuTabs->isVisible()) {
		QAction* labelAction{m_menuTabs->actions().last()};
		labelAction->setText(tr("Currently you have %n opened tab(s)", "", count() - 1));
	}

	removeTab(index);
	webTab->deleteLater();

	updateClosedTabsButton();

	emit changed();
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

	if (count() == 1) {
		if (m_dontCloseWithOneTab) {
			if (webView->url() == m_urlOnNewTab)
				m_closedTabsManager->takeLastClosedTab();
			webView->load(m_urlOnNewTab);
			return;
		}
		m_window->close();
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

	if (webTab->isPinned() || count() == 1)
		return;

	disconnect(webTab->webView(), &TabbedWebView::wantsCloseTab, this, &TabWidget::closeTab);
	disconnect(webTab->webView(), SIGNAL(urlChanged(QUrl)), this, SIGNAL(changed()));

	webTab->detach();

	BrowserWindow* window{Application::instance()->createWindow(Application::WT_NewWindow)};
	window->setStartTab(webTab);
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

void TabWidget::fullScreenRequested(QWebEngineFullScreenRequest request)
{
	WebPage* webPage = qobject_cast<WebPage*>(sender());

	if (request.toggleOn()) {
		if (!m_fullScreenView) {
			m_fullScreenView = new QWebEngineView();

			QAction* exitFullScreenAction = new QAction(m_fullScreenView);
			exitFullScreenAction->setShortcut(Qt::Key_Escape);

			connect(exitFullScreenAction, &QAction::triggered, [webPage]
			{
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

void TabWidget::downloadRequested(QWebEngineDownloadItem* download)
{
	Application::instance()->downloadManager()->downlaod(download);
	Application::instance()->downloadManager()->show();
	download->accept();
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

void TabWidget::tabMoved(int before, int after)
{
	Q_UNUSED(before);
	Q_UNUSED(after);

	m_lastBackgroundTabIndex = -1;
	m_lastTabIndex = before;

	emit changed();
}

WebTab* TabWidget::weTab()
{
	return weTab(currentIndex());
}

WebTab* TabWidget::weTab(int index)
{
	return qobject_cast<WebTab*>(widget(index));
}

TabIcon* TabWidget::tabIcon(int index)
{
	return weTab(index)->tabIcon();
}

void TabWidget::openAddBookmarkDialog()
{
	QString url = weTab()->url().toString();
	QString title = weTab()->title();

	AddBookmarkDialog* dialog{new AddBookmarkDialog(url, title)};
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	dialog->show();
}

void TabWidget::openBookmarkDialog()
{
	BookmarksDialog* dialog{new BookmarksDialog(this)};
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	connect(dialog, SIGNAL(openUrl(
							   const QUrl&)), this, SLOT(addView(
															 const QUrl&)));

	dialog->show();
}

void TabWidget::openHistoryDialog()
{
	HistoryDialog* dialog(new HistoryDialog(this));
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	connect(dialog, SIGNAL(openUrl(
							   const QUrl&)), this, SLOT(addView(
															 const QUrl&)));

	dialog->show();
}

void TabWidget::openPreferencesDialog()
{
	PreferencesDialog* dialog{new PreferencesDialog(this)};
	dialog->show();
}

bool TabWidget::validIndex(int index) const
{
	return (index >= 0 && index < count());
}

void TabWidget::updateClosedTabsButton()
{
	if (!m_showClosedTabsButton)
		m_buttonClosedTabs->hide();

	m_buttonClosedTabs->setEnabled(canRestoreTab());
}

void TabWidget::updateFloatingButton(int index)
{
	WebTab* tab{weTab(index)};
	WebTab* lastTab{weTab(m_lastTabIndex)};

	if (Application::instance()->useTopToolBar()) {
		if (lastTab) {
			m_topToolBar->removeAction(m_actionUrl);
			lastTab->removeToolBar(m_topToolBar);
		}

		tab->addressBar()->setVisible(true);
		tab->addToolBar(m_topToolBar);
		m_actionUrl = m_topToolBar->insertWidget(m_actionAddBookmark, tab->addressBar());

	}

	if (lastTab) {
		disconnect(m_fButtonNewWindow, &FloatingButton::isClicked, lastTab, &WebTab::sNewWindow);
		disconnect(m_fButtonHome, &FloatingButton::isClicked, lastTab, &WebTab::sGoHome);
		disconnect(m_fButtonNext, &FloatingButton::isClicked, lastTab->webView(), &TabbedWebView::forward);
		disconnect(m_fButtonBack, &FloatingButton::isClicked, lastTab->webView(), &TabbedWebView::back);
		disconnect(m_fButtonNewTab, &FloatingButton::isClicked, lastTab, &WebTab::sNewTab);
	}

	connect(m_fButtonNewWindow, &FloatingButton::isClicked, tab, &WebTab::sNewWindow);
	connect(m_fButtonHome, &FloatingButton::isClicked, tab, &WebTab::sGoHome);
	connect(m_fButtonNext, &FloatingButton::isClicked, tab->webView(), &TabbedWebView::forward);
	connect(m_fButtonBack, &FloatingButton::isClicked, tab->webView(), &TabbedWebView::back);
	connect(m_fButtonNewTab, &FloatingButton::isClicked, tab, &WebTab::sNewTab);

}

}