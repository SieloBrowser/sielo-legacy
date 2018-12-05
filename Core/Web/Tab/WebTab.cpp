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

#include "Web/Tab/WebTab.hpp"

#include <QColor>
#include <QLineEdit>

#include "BrowserWindow.hpp"

#include "Utils/Settings.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Web/WebPage.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/FloatingButton.hpp"
#include "Widgets/SearchToolBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/TabIcon.hpp"
#include "Widgets/Tab/MainTabBar.hpp"
#include "Widgets/AddressBar/AddressBar.hpp"

namespace Sn
{

static const int SAVED_TAB_VERSION = 2;
static WebTab::AddChildBehavior s_addChildBehavior = WebTab::AppendChild;

WebTab::AddChildBehavior WebTab::addChildBehavior()
{
	return s_addChildBehavior;
}

void WebTab::setAddChildBehavior(AddChildBehavior behavior)
{
	s_addChildBehavior = behavior;
}

WebTab::SavedTab::SavedTab() :
	isPinned(false)
{
	Settings settings{};
	int defaultZoomLevel{settings.value("Web-Settings/defaultZoomLevel", WebView::zoomLevels().indexOf(100)).toInt()};

	zoomLevel = defaultZoomLevel;
}

WebTab::SavedTab::SavedTab(WebTab* webTab)
{
	title = webTab->title();
	url = webTab->url();
	icon = webTab->icon(false);
	history = webTab->historyData();
	isPinned = webTab->isPinned();
	zoomLevel = webTab->zoomLevel();
	parentTab = webTab->parentTab() ? webTab->parentTab()->tabIndex() : -1;

	const auto children = webTab->childTabs();
	childTabs.reserve(children.count());

	for (WebTab* child : children)
		childTabs.append(child->tabIndex());

	sessionData = webTab->sessionData();
}

bool WebTab::SavedTab::isValide() const
{
	return !url.isEmpty() || !history.isEmpty();
}

void WebTab::SavedTab::clear()
{
	Settings settings{};
	int defaultZoomLevel{settings.value("Web-Settings/defaultZoomLevel", WebView::zoomLevels().indexOf(100)).toInt()};

	title.clear();
	url.clear();
	icon = QIcon();
	history.clear();
	isPinned = false;
	zoomLevel = defaultZoomLevel;
	parentTab = -1;
	childTabs.clear();
	sessionData.clear();
}

QDataStream& operator<<(QDataStream& stream, const WebTab::SavedTab& tab)
{
	stream << SAVED_TAB_VERSION;
	stream << tab.title;
	stream << tab.url;
//	stream << tab.icon.pixmap(16);
	stream << tab.history;
	stream << tab.isPinned;
	stream << tab.zoomLevel;
	stream << tab.parentTab;
	stream << tab.childTabs;
	stream << tab.sessionData;

	return stream;

}

QDataStream& operator>>(QDataStream& stream, WebTab::SavedTab& tab)
{
	int version;
	stream >> version;

	if (version < 1)
		return stream;

	/* [NOTE FOR ME]
	 * Example of version usage:
	 *
	 * stream >> xxx
	 * stream >> xxx
	 *
	 * if (version >= X)
	 *     stream >> xxx
	 */

	QPixmap pixmap{};

	stream >> tab.title;
	stream >> tab.url;
//	stream >> pixmap;
	stream >> tab.history;
	stream >> tab.isPinned;
	stream >> tab.zoomLevel;

	if (version >= 2) {
		stream >> tab.parentTab;
		stream >> tab.childTabs;
		stream >> tab.sessionData;
	}

	tab.icon = Application::getAppIcon("webpage");

	return stream;
}

WebTab::WebTab(TabWidget* tabWidget) :
	QWidget(),
	m_tabWidget(tabWidget),
	m_isPinned(false)
{
	setObjectName(QLatin1String("webtab"));
	//setStyleSheet("#webtab {background-color: white;}");

	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_webView = new TabbedWebView(this);
	m_webView->setTabWidget(tabWidget);
	m_webView->setWebPage(new WebPage);
	m_webView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	m_tabIcon = new TabIcon(this);
	m_tabIcon->setWebTab(this);

	m_menuForward = new QMenu(this);

//	m_fButton->setPattern(FloatingButton::Toolbar);

	m_addressBar = new AddressBar(tabWidget);
	m_addressBar->setWebView(m_webView);

	m_layout->addWidget(m_webView);

	QWidget* viewWidget{new QWidget(this)};
	viewWidget->setLayout(m_layout);

	m_splitter = new QSplitter(Qt::Vertical, this);
	m_splitter->setObjectName("webtab-container");
	m_splitter->setChildrenCollapsible(false);
	m_splitter->addWidget(viewWidget);
	//m_splitter->setStyleSheet("background: white");

	connect(m_webView, &TabbedWebView::showNotification, this, &WebTab::showNotification);
	connect(m_webView, &TabbedWebView::loadStarted, this, &WebTab::loadStarted);
	connect(m_webView, &TabbedWebView::loadFinished, this, &WebTab::loadFinished);
	connect(m_webView, &TabbedWebView::titleChanged, this, &WebTab::titleWasChanged);
	connect(m_webView, &TabbedWebView::titleChanged, this, &WebTab::titleChanged);
	connect(m_webView, &TabbedWebView::iconChanged, this, &WebTab::iconChanged);
	connect(m_webView, &TabbedWebView::backgroundActivityChanged, this, &WebTab::backgroundActivityChanged);
	connect(m_webView, &TabbedWebView::loadStarted, this, std::bind(&WebTab::loadingChanged, this, true));
	connect(m_webView, &TabbedWebView::loadFinished, this, std::bind(&WebTab::loadingChanged, this, false));

	auto pageChanged = [this](WebPage *page)
	{
		connect(page, &WebPage::audioMutedChanged, this, &WebTab::playingChanged);
		connect(page, &WebPage::recentlyAudibleChanged, this, &WebTab::mutedChanged);
	};

	pageChanged(m_webView->page());

	connect(m_webView, &TabbedWebView::pageChanged, this, pageChanged);

	connect(m_tabIcon, &TabIcon::resized, this, [this]()
	{
		if (m_tabWidget->tabBar())
			m_tabWidget->tabBar()->setTabButton(tabIndex(), m_tabWidget->tabBar()->iconButtonPosition(), m_tabIcon);
	});

	m_notificationWidget = new QWidget(this);
	m_notificationWidget->setAutoFillBackground(true);
	QPalette palette{m_notificationWidget->palette()};
	palette.setColor(QPalette::Background, palette.window().color().darker(110));
	m_notificationWidget->setPalette(palette);

	QVBoxLayout* notificationLayout{new QVBoxLayout(m_notificationWidget)};
	notificationLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	notificationLayout->setContentsMargins(0, 0, 0, 0);
	notificationLayout->setSpacing(1);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(m_addressBar);
	layout->addWidget(m_splitter);

	setLayout(layout);

	setFocusProxy(m_webView);
	
	Application::instance()->plugins()->emitWebTabCreated(this);
}



WebTab::~WebTab()
{
	Application::instance()->plugins()->emitWebTabDeleted(this);

	if (m_application) {
		m_application->disconnect();
		delete m_application;
	}
}

void WebTab::takeAddressBar()
{
	m_addressBar->setParent(this);
	m_layout->insertWidget(0, m_addressBar);
	m_addressBar->show();
}

void WebTab::setParentTab(WebTab* tab)
{
	if (m_isPinned || m_parentTab == tab)
		return;

	if (tab && tab->isPinned())
		return;

	if (m_parentTab) {
		const int index{m_parentTab->m_childTabs.indexOf(this)};
		if (index >= 0) {
			m_parentTab->m_childTabs.removeAt(index);
			emit m_parentTab->childTabRemoved(this, index);
		}
	}

	m_parentTab = tab;

	if (tab) {
		m_parentTab = nullptr;
		tab->addChildTab(this);
	}
	else
		emit parentTabChanged(m_parentTab);
}

void WebTab::addChildTab(WebTab* tab, int index)
{
	if (m_isPinned || !tab || tab->isPinned())
		return;

	WebTab* oldParent{tab->m_parentTab};
	tab->m_parentTab = this;

	if (oldParent) {
		const int index{oldParent->m_childTabs.indexOf(tab)};

		if (index >= 0) {
			oldParent->m_childTabs.removeAt(index);
			emit oldParent->childTabRemoved(tab, index);
		}
	}

	if (index < 0 || index > m_childTabs.size()) {
		index = 0;

		if (addChildBehavior() == AppendChild)
			index = m_childTabs.size();
		else if (addChildBehavior() == PrependChild)
			index = 0;
	}

	m_childTabs.insert(index, tab);

	emit childTabAdded(tab, index);
	emit tab->parentTabChanged(this);
}

void WebTab::setSessionData(const QString& key, const QVariant& value)
{
	m_sessionData[key] = value;
}

QUrl WebTab::url() const
{
	if (isRestored()) {
		if (m_webView->url().isEmpty() && m_webView->isLoading())
			return m_webView->page()->requestedUrl();

		return m_webView->url();
	}
	else
		return m_savedTab.url;
}

QString WebTab::title() const
{
	if (m_application)
		return m_application->windowTitle();

	if (isRestored())
		return m_webView->title();
	else
		return m_savedTab.title;
}

QIcon WebTab::icon(bool allowNull) const
{
	if (m_application)
		return m_application->windowIcon();

	if (isRestored())
		return m_webView->icon(allowNull);

	if (allowNull || !m_savedTab.icon.isNull())
		return m_savedTab.icon;

	return Application::getAppIcon("webpage");
}

QWebEngineHistory* WebTab::history() const
{
	return m_webView->history();
}

int WebTab::zoomLevel() const
{
	return m_webView->zoomLevel();
}

void WebTab::setZoomLevel(int level)
{
	m_webView->setZoomLevel(level);
}

void WebTab::detach()
{
	Q_ASSERT(m_tabWidget);
	Q_ASSERT(m_tabWidget->tabBar());

	removeFromTabTree();
	
	m_tabWidget->tabBar()->setTabButton(tabIndex(), m_tabWidget->tabBar()->iconButtonPosition(), nullptr);
	m_tabIcon->setParent(nullptr);

	m_tabWidget->removeTab(tabIndex());
	setParent(nullptr);

	m_addressBar->setParent(this);
	m_webView->setTabWidget(nullptr);

	if (m_isCurrentTab) {
		m_isCurrentTab = false;
		emit currentTabChanged(m_isCurrentTab);
	}

	m_tabWidget->tabBar()->disconnect(this);

	m_tabWidget = nullptr;
}

void WebTab::attach(TabWidget* tabWidget)
{
	m_tabWidget = tabWidget;

	m_webView->setTabWidget(tabWidget);
	m_addressBar->setTabWidget(tabWidget);
	m_tabWidget->tabBar()->setTabButton(tabIndex(), m_tabWidget->tabBar()->iconButtonPosition(), m_tabIcon);
	m_tabWidget->tabBar()->setTabText(tabIndex(), title());
	m_tabIcon->updateIcon();

	auto currentChanged = [this](int index) {
		const bool wasCurrent{m_isCurrentTab};
		m_isCurrentTab = index == tabIndex();

		if (wasCurrent != m_isCurrentTab)
			emit currentTabChanged(m_isCurrentTab);
	};

	currentChanged(m_tabWidget->tabBar()->currentIndex());
	connect(m_tabWidget->tabBar(), &MainTabBar::currentChanged, this, currentChanged);
}

void WebTab::addToolBar(QToolBar* toolBar)
{
	m_layout->insertWidget(0, toolBar);
}

void WebTab::removeToolBar(QToolBar* toolBar)
{
	m_layout->removeWidget(toolBar);
}

QByteArray WebTab::historyData() const
{
	if (isRestored()) {
		QByteArray historyArray{};
		QDataStream historyStream{&historyArray, QIODevice::WriteOnly};

		historyStream << *m_webView->history();

		return historyArray;
	}
	else
		return m_savedTab.history;
}

void WebTab::setHistoryData(const QByteArray& data)
{
	m_webView->restoreHistory(data);
}

void WebTab::stop()
{
	m_webView->stop();
}

void WebTab::reload()
{
	m_webView->reload();
}

void WebTab::load(const LoadRequest& request)
{
	if (!isRestored()) {
		tabActivated();
		QTimer::singleShot(0, this, std::bind(&WebTab::load, this, request));
	}
	else
		m_webView->load(request);
}

void WebTab::loadApplication(QWidget* application)
{
	m_savedTab = SavedTab(this);

	emit restoredChanged(isRestored());

	m_webView->setPage(new WebPage());

	m_application = application;
	m_application->setParent(nullptr);

	m_layout->removeWidget(m_webView);
	m_layout->addWidget(m_application);

	connect(m_application, &QWidget::destroyed, this, &WebTab::closeTab);
	connect(m_application, &QWidget::windowTitleChanged, this, &WebTab::titleWasChanged);
}

void WebTab::unload()
{
	m_savedTab = SavedTab(this);
	
	emit restoredChanged(isRestored());

	m_webView->setPage(new WebPage());
	m_webView->setFocus();
}

bool WebTab::isLoading() const
{
	return m_webView->isLoading();
}

bool WebTab::isPinned() const
{
	return m_isPinned;
}

void WebTab::setPinned(bool state)
{
	if (m_isPinned == state)
		return;

	if (state)
		removeFromTabTree();

	m_isPinned = state;
	emit pinnedChanged(m_isPinned);
}

void WebTab::togglePinned()
{
	Q_ASSERT(m_tabWidget);
	Q_ASSERT(m_tabWidget->tabBar());

	m_isPinned = !m_isPinned;

	setPinned(!isPinned());
	m_tabWidget->pinUnPinTab(tabIndex(), title());
}

bool WebTab::isMuted() const
{
	return m_webView->page()->isAudioMuted();
}

void WebTab::setMuted(bool muted)
{
	m_webView->page()->setAudioMuted(muted);
}

void WebTab::toggleMuted()
{
	setMuted(!isMuted());
}

bool WebTab::backgroundActivity() const
{
	return m_webView->backgroundActivity();
}

int WebTab::tabIndex() const
{
	return m_tabWidget ? m_tabWidget->indexOf(const_cast<WebTab*>(this)) : -1;
}

void WebTab::makeCurrentTab() const
{
	if (m_tabWidget)
		m_tabWidget->setCurrentIndex(tabIndex());
}

void WebTab::closeTab() const
{
	if (m_tabWidget)
		m_tabWidget->closeTab(tabIndex());
}

void WebTab::moveTab(int to) const
{
	if (m_tabWidget)
		m_tabWidget->moveTab(tabIndex(), to);
}

bool WebTab::haveInspector() const
{
	return m_inspector;
}

void WebTab::showWebInspector(bool inspectElement)
{
	if (m_inspector)
		return;

	m_inspector = new WebInspector(this);
	m_inspector->setView(m_webView, inspectElement);

	m_splitter->addWidget(m_inspector);

	m_splitter->setSizes({2 * (m_splitter->height() / 3), m_splitter->height() / 3});
}

void WebTab::toggleWebInspector()
{
	if (!m_inspector)
		showWebInspector();
	else
		m_inspector->close();
}

void WebTab::showSearchToolBar()
{
	int offset = Application::instance()->useTopToolBar();

	const int index{2 - offset};
	SearchToolBar* toolBar{nullptr};

	if (m_layout->count() == 2 - offset) {
		toolBar = new SearchToolBar(m_webView, this);
		m_layout->insertWidget(index, toolBar);
	}
	else if (m_layout->count() == 3 - offset) {
		Q_ASSERT(qobject_cast<SearchToolBar*>(m_layout->itemAt(index)->widget()));
		toolBar = static_cast<SearchToolBar*>(m_layout->itemAt(index)->widget());
	}

	Q_ASSERT(toolBar);
	toolBar->focusSearchLine();
}

bool WebTab::isRestored() const
{
	return !m_savedTab.isValide();
}

void WebTab::restoreTab(const SavedTab& tab)
{
	Q_ASSERT(m_tabWidget->tabBar());

	Settings settings{};

	m_isPinned = tab.isPinned;
	m_sessionData = tab.sessionData;

	if (!isPinned() && settings.value("Web-Settings/LoadTabsOnActivation", true).toBool()) {
		m_savedTab = tab;

		emit restoredChanged(isRestored());

		int index = tabIndex();

		m_tabWidget->tabBar()->setTabText(index, tab.title);
		m_addressBar->showUrl(tab.url);
		m_tabIcon->updateIcon();

		if (!tab.url.isEmpty()) {
			QColor color{m_tabWidget->tabBar()->palette().text().color()};
			QColor newColor{color.lighter(250)};

			if (color == Qt::black || color == Qt::white)
				newColor = Qt::gray;

			m_tabWidget->tabBar()->overrideTabTextColor(index, newColor);

		}
	}
	else {
		QTimer::singleShot(1000, this, [=]()
		{
			p_restoreTab(tab);
		});
	}
}

void WebTab::p_restoreTab(const SavedTab& tab)
{
	p_restoreTab(tab.url, tab.history, tab.zoomLevel);
}

void WebTab::p_restoreTab(const QUrl& url, const QByteArray& history, int zoomLevel)
{
	m_webView->load(url);

	const QStringList blacklisedSchemes = {"view-source", "chrome"};

	if (!blacklisedSchemes.contains(url.scheme())) 
		m_webView->restoreHistory(history);
	
	m_webView->setZoomLevel(zoomLevel);
	m_webView->setFocus();
}

void WebTab::tabActivated()
{
	if (isRestored() || m_application)
		return;

	QTimer::singleShot(0, this, [this]() {
		if (isRestored())
			return;

		p_restoreTab(m_savedTab);
		m_savedTab.clear();

		emit restoredChanged(isRestored());
	});
}

void WebTab::sNewWindow()
{
	Application::instance()->createWindow(Application::WT_NewWindow, m_tabWidget->homeUrl());
}

void WebTab::sNewTab()
{
	LoadRequest request{};
	request.setUrl(m_tabWidget->urlOnNewTab());
	m_webView->loadInNewTab(request, Application::NTT_CleanSelectedTabAtEnd);
}

void WebTab::sGoHome()
{
	m_webView->load(m_tabWidget->homeUrl());
}

void WebTab::showNotification(QWidget* notif)
{
	m_notificationWidget->setParent(this);
	m_notificationWidget->raise();
	m_notificationWidget->setFixedWidth(width());
	m_notificationWidget->layout()->addWidget(notif);
	m_notificationWidget->show();

	notif->show();
}

void WebTab::loadStarted()
{
	if (m_application) {
		m_application->disconnect();

		m_layout->removeWidget(m_application);
		m_layout->addWidget(m_webView);

		m_webView->restoreHistory(m_savedTab.history);
		m_webView->setZoomLevel(m_savedTab.zoomLevel);
		m_webView->setFocus();

		delete m_application;
		m_application = nullptr;
		m_savedTab.clear();
	}
	else {
		if (m_tabWidget->tabBar() && m_webView->isTitleEmpty())
			m_tabWidget->tabBar()->setTabText(tabIndex(), tr("Loading..."));
	}
}

void WebTab::loadFinished()
{
	titleChanged(m_webView->title());
}

void WebTab::titleWasChanged(const QString& title)
{
	if (!m_tabWidget->tabBar() || !m_tabWidget || title.isEmpty())
		return;

	if (m_isCurrentTab)
		m_tabWidget->window()->setWindowTitle(tr("%1 - Sielo").arg(title));

	m_tabWidget->tabBar()->setTabText(tabIndex(), title);
}

void WebTab::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	m_notificationWidget->setFixedWidth(width());
}

void WebTab::removeFromTabTree()
{
	WebTab* parentTab{m_parentTab};
	const int parentIndex{parentTab ? parentTab->m_childTabs.indexOf(this) : -1};

	setParentTab(nullptr);

	int i{0};
	while (!m_childTabs.isEmpty()) {
		WebTab* child{m_childTabs[0]};
		child->setParentTab(nullptr);

		if (parentTab)
			parentTab->addChildTab(child, parentIndex + i++);
	}
}
}
