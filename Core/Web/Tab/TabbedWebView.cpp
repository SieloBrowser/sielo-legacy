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

#include "Web/Tab/TabbedWebView.hpp"

#include <QStatusBar>

#include "BrowserWindow.hpp"

#include "History/HistoryManager.hpp"

#include "Widgets/AddressBar.hpp"
#include "Widgets/FloatingButton.hpp"
#include "Widgets/NavigationBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"

#include "Web/WebPage.hpp"
#include "Web/LoadRequest.hpp"
#include "Web/WebHitTestResult.hpp"
#include "Web/Tab/WebTab.hpp"

namespace Sn {
TabbedWebView::TabbedWebView(WebTab* tab) :
	WebView(tab),
	m_window(nullptr),
	m_webTab(tab),
	m_menu(new QMenu(this))
{
	connect(this, &WebView::loadStarted, this, &TabbedWebView::sLoadStarted);
	connect(this, &WebView::loadProgress, this, &TabbedWebView::sLoadProgress);
	connect(this, &WebView::loadFinished, this, &TabbedWebView::sLoadFinished);
	connect(this, &WebView::urlChanged, this, &TabbedWebView::urlChanged);
}

void TabbedWebView::setWebPage(WebPage* page)
{
	page->setParent(this);
	setPage(page);

	connect(page, &WebPage::linkHovered, this, &TabbedWebView::linkHovered);
}

void TabbedWebView::setBrowserWindow(BrowserWindow* window)
{
	m_window = window;
}

int TabbedWebView::tabIndex() const
{
	return m_webTab->tabIndex();
}

QWidget* TabbedWebView::overlayWidget()
{
	return m_webTab;
}

void TabbedWebView::closeView()
{
	emit wantsCloseTab(tabIndex());
}

void TabbedWebView::loadInNewTab(const LoadRequest& request, Application::NewTabType tabType)
{
	if (m_window) {
		int index{m_window->tabWidget()->addView(QUrl(), tabType)};
		m_window->webView(index)->webTab()->addressBar()->showUrl(request.url());
		m_window->webView(index)->load(request);
	}
}

bool TabbedWebView::isFullScreen()
{
	return (m_window && m_window->isFullScreen());
}

void TabbedWebView::requestFullScreen(bool enable)
{
	if (!m_window)
		return;

	if (enable)
		m_window->enterHtmlFullScreen();
	else
		m_window->showNormal();
}

void TabbedWebView::setAsCurrentTab()
{
	if (m_window)
		m_window->tabWidget()->setCurrentWidget(m_webTab);
}

void TabbedWebView::userLoadAction(const LoadRequest& request)
{
	load(request);
}

void TabbedWebView::sLoadStarted()
{
	m_currentIp.clear();
}

void TabbedWebView::sLoadProgress(int progress)
{
	Q_UNUSED(progress)
}

void TabbedWebView::sLoadFinished()
{
	QHostInfo::lookupHost(url().host(), this, SLOT(setIp(QHostInfo)));
}

void TabbedWebView::urlChanged(const QUrl& url)
{
	if (m_webTab->isCurrentTab() && url.isValid()) {
		HistoryManager* manager = Application::instance()->historyManager();
		manager->addHistoryEntry(url.toString());
	}

	if (Application::instance()->useTopToolBar() && (m_webTab->isCurrentTab() && m_webTab->tabBar()->tabWidget()))
		m_webTab->tabBar()->tabWidget()->navigationToolBar()->refreshBackForwardButtons();

}

void TabbedWebView::linkHovered(const QString& link)
{
	if (m_webTab->isCurrentTab() && m_window) {
		if (link.isEmpty()) {
			m_window->statusBar()->clearMessage();
			//m_window->statusBar()->hide();
		}
		else {
			m_window->statusBar()->showMessage(link);
			m_window->statusBar()->show();
		}
	}
}

void TabbedWebView::setIp(const QHostInfo& info)
{
	if (info.addresses().isEmpty())
		return;

	m_currentIp = QString("%1 (%2)").arg(info.hostName(), info.addresses()[0].toString());

	if (m_webTab->isCurrentTab())
		emit ipChanged(m_currentIp);
}

void TabbedWebView::newContextMenuEvent(QContextMenuEvent* event)
{
	m_menu->clear();

	WebHitTestResult hitTest{page()->hitTestContent(event->pos())};
	createContextMenu(m_menu, hitTest);
	m_menu->addSeparator();
	m_menu->addAction(tr("Show Inspector"), m_webTab, &WebTab::showInspector);

	if (!m_menu->isEmpty()) {
		const QPoint pos{event->globalPos()};
		QPoint p{pos.x(), pos.y() + 1};

		m_menu->popup(p);
		return;
	}

	WebView::newContextMenuEvent(event);
}

void TabbedWebView::newMousePressEvent(QMouseEvent* event)
{
//	if (m_webTab->floatingButton()->pattern() != FloatingButton::Toolbar) {
//		m_webTab->floatingButton()->hideChildren();
//	}

	WebView::newMousePressEvent(event);
}

void TabbedWebView::newMouseMoveEvent(QMouseEvent* event)
{
	WebView::newMouseMoveEvent(event);
}

void TabbedWebView::enterEvent(QEvent* event)
{
	event->accept();

	emit m_webTab->tabBar()->tabWidget()->focusIn(m_webTab->tabBar()->tabWidget());
}
}