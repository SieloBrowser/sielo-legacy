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

#include "Web/Tab/WebTab.hpp"

#include <QSettings>

#include <QColor>
#include <QtWidgets/QLineEdit>

#include "BrowserWindow.hpp"

#include "Web/WebPage.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Bookmarks/BookmarkManager.hpp"

#include "History/HistoryManager.hpp"

#include "Widgets/FloatingButton.hpp"
#include "Widgets/AddressBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/TabIcon.hpp"
#include "Widgets/Tab/MainTabBar.hpp"

namespace Sn {

static const int SAVED_TAB_VERSION = 1;

bool WebTab::s_pinningTab = false;

WebTab::SavedTab::SavedTab() :
	isPinned(false)
{
	QSettings settings{};
	int defaultZoomLevel{settings.value("Preferences/ZoomLevel", WebView::zoomLevels().indexOf(100)).toInt()};

	zoomLevel = defaultZoomLevel;
}

WebTab::SavedTab::SavedTab(WebTab* webTab)
{
	title = webTab->title();
	url = webTab->url();
	icon = webTab->icon(true);
	history = webTab->historyData();
	isPinned = webTab->isPinned();
	zoomLevel = webTab->zoomLevel();
}

bool WebTab::SavedTab::isValide() const
{
	return !url.isEmpty();
}

void WebTab::SavedTab::clear()
{
	QSettings settings{};
	int defaultZoomLevel{settings.value("Preferences/ZoomLevel", WebView::zoomLevels().indexOf(100)).toInt()};

	title.clear();
	url.clear();
	icon = QIcon();
	isPinned = false;
	zoomLevel = defaultZoomLevel;
}

QDataStream& operator<<(QDataStream& stream, const WebTab::SavedTab& tab)
{
	stream << SAVED_TAB_VERSION;
	stream << tab.title;
	stream << tab.url;
	stream << tab.icon.pixmap(16);
	stream << tab.history;
	stream << tab.isPinned;
	stream << tab.zoomLevel;

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
	stream >> pixmap;
	stream >> tab.history;
	stream >> tab.isPinned;
	stream >> tab.zoomLevel;

	tab.icon = QIcon(pixmap);

	return stream;
}

WebTab::WebTab(BrowserWindow* window) :
	QWidget(),
	m_window(window),
	m_isPinned(false)
{
	setObjectName(QLatin1String("webtab"));
	setStyleSheet("#webtab {background-color: white;}");

	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_webView = new TabbedWebView(this);
	m_webView->setBrowserWindow(m_window);
	m_webView->setWebPage(new WebPage);
	m_webView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	m_splitter = new QSplitter(this);
	m_splitter->setChildrenCollapsible(false);
	m_splitter->addWidget(m_webView);

	m_tabIcon = new TabIcon(this);
	m_tabIcon->setWebTab(this);

	m_menuForward = new QMenu(this);

//	m_fButton->setPattern(FloatingButton::Toolbar);

	m_addressBar = new AddressBar(m_window);
	m_addressBar->setWebView(m_webView);
	m_addressBar->setText("https://ecosia.org");


	m_layout->addWidget(m_addressBar);
	m_layout->addWidget(m_splitter);

	connect(m_webView, &TabbedWebView::showNotification, this, &WebTab::showNotification);
	connect(m_webView, &TabbedWebView::loadStarted, this, &WebTab::loadStarted);
	connect(m_webView, &TabbedWebView::loadFinished, this, &WebTab::loadFinished);
	connect(m_webView, &TabbedWebView::titleChanged, this, &WebTab::titleChanged);
	connect(m_tabIcon, &TabIcon::resized, this, [this]()
	{
		if (m_tabBar)
			m_tabBar->setTabButton(tabIndex(), m_tabBar->iconButtonPosition(), m_tabIcon);
	});

	setLayout(m_layout);

}

QUrl WebTab::url() const
{
	if (isRestored())
		return m_webView->url();
	else
		return m_savedTab.url;
}

QString WebTab::title() const
{
	if (isRestored())
		return m_webView->title();
	else
		return m_savedTab.title;
}

QIcon WebTab::icon(bool allowNull) const
{
	if (isRestored())
		return m_webView->icon(allowNull);

	if (allowNull || !m_savedTab.icon.isNull())
		return m_savedTab.icon;

	return QIcon(":icons/other/webpage.png");
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
	Q_ASSERT(m_tabBar);

	m_tabBar->setTabButton(tabIndex(), m_tabBar->iconButtonPosition(), nullptr);
	setParent(nullptr);
	m_webView->setBrowserWindow(nullptr);

	m_window = nullptr;
	m_tabBar = nullptr;
}

void WebTab::attach(BrowserWindow* window)
{
	m_window = window;
	m_tabBar = m_window->tabWidget()->tabBar();

	m_webView->setBrowserWindow(m_window);
	m_tabBar->setTabButton(tabIndex(), m_tabBar->iconButtonPosition(), m_tabIcon);
	m_tabBar->setTabText(tabIndex(), title());
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
	m_isPinned = state;
}

void WebTab::togglePinned()
{
	Q_ASSERT(m_window);
	Q_ASSERT(m_tabBar);

	m_isPinned = !m_isPinned;

	s_pinningTab = true;
	m_window->tabWidget()->pinUnPinTab(tabIndex(), title());
	s_pinningTab = false;
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

int WebTab::tabIndex() const
{
	Q_ASSERT(m_tabBar);

	return m_tabBar->tabWidget()->indexOf(const_cast<WebTab*>(this));
}

bool WebTab::isCurrentTab() const
{
	return m_tabBar && tabIndex() == m_tabBar->currentIndex();
}

bool WebTab::isRestored() const
{
	return !m_savedTab.isValide();
}

void WebTab::restoreTab(const SavedTab& tab)
{
	Q_ASSERT(m_tabBar);

	QSettings settings{};

	m_isPinned = tab.isPinned;

	if (!isPinned() && settings.value("Web-Sielo-Settings/LoadTabsOnActivation", true).toBool()) {
		m_savedTab = tab;
		int index = tabIndex();

		m_tabBar->setTabText(index, tab.title);
		m_addressBar->showUrl(tab.url);
		m_tabIcon->updateIcon();

		if (!tab.url.isEmpty()) {
			QColor color{m_tabBar->palette().text().color()};
			QColor newColor{color.lighter(250)};

			if (color == Qt::black || color == Qt::white)
				newColor = Qt::gray;

			m_tabBar->overrideTabTextColor(index, newColor);

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
	m_webView->restoreHistory(history);
	m_webView->setZoomLevel(zoomLevel);
	m_webView->setFocus();
}

void WebTab::sNewWindow()
{
	Application::instance()->createWindow(Application::WT_NewWindow, QUrl("https://ecosia.org"));
}

void WebTab::sNewTab()
{
	LoadRequest request{};
	request.setUrl(QUrl("https://ecosia.org"));
	m_webView->loadInNewTab(request, Application::NTT_CleanSelectedTabAtEnd);
}

void WebTab::sGoHome()
{
	m_webView->load(QUrl("https://ecosia.org"));
}

void WebTab::showNotification(QWidget* notif)
{
	const int notifPos{0};

	if (m_layout->count() > notifPos + 1)
		delete m_layout->itemAt(notifPos)->widget();

	m_layout->insertWidget(notifPos, notif);
	notif->show();
}

void WebTab::loadStarted()
{
	if (m_tabBar && m_webView->isTitleEmpty())
		m_tabBar->setTabText(tabIndex(), tr("Loading..."));
}

void WebTab::loadFinished()
{
	titleChanged(m_webView->title());
}

void WebTab::titleChanged(const QString& title)
{
	if (!m_tabBar || !m_window || title.isEmpty())
		return;

	if (isCurrentTab())
		m_window->setWindowTitle(tr("%1 - Sielo").arg(title));

	m_tabBar->setTabText(tabIndex(), title);

}

void WebTab::sRestore()
{
	Q_ASSERT(m_tabBar);

	p_restoreTab(m_savedTab);
	m_savedTab.clear();

	m_tabBar->restoreTabTextColor(tabIndex());
}

/*void WebTab::aboutToShowHistoryNextMenu()
{
	if (!m_menuForward || !m_webView)
		return;

	m_menuForward->clear();

	QWebEngineHistory* history{m_webView->history()};
	int currentIndex{history->currentItemIndex()};
	int count{0};

	for (int i{currentIndex + 1}; i < history->count(); ++i) {
		QWebEngineHistoryItem item{history->itemAt(i)};

		if (item.isValid()) {
			QString title{item.title()};
			if (title.isEmpty())
				title = item.url().toString(QUrl::RemoveFragment);

			if (title.isEmpty())
				title = tr("Empty Page");
			else if (title.length() > 40)
				title = title.left(40) + QLatin1String("...");

		}
	}
}*/

void WebTab::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
}

}