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

#include "Web/Tab/TabbedWebView.hpp"

#include <QStatusBar>

#include <QMimeData>

#include "BrowserWindow.hpp"

#include "History/History.hpp"

#include "Widgets/NavigationBar.hpp"
#include "Widgets/StatusBarMessage.hpp"
#include "Widgets/AddressBar/AddressBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"
#include "Widgets/Tab/TabBar.hpp"

#include "Web/WebPage.hpp"
#include "Web/Scripts.hpp"
#include "Web/WebHitTestResult.hpp"

namespace Sn
{
TabbedWebView::TabbedWebView(WebTab* tab) :
	WebView(tab),
	m_tabWidget(nullptr),
	m_webTab(tab),
	m_menu(new QMenu(this)),
	m_cursorIn(false)
{
	setAcceptDrops(true);

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
#ifdef EXP_TRANSPARENT_BG
	connect(page, &WebPage::pageRendering, this, &TabbedWebView::sPageRendering);
#endif // EXP_TRANSPARENT_BG
}

void TabbedWebView::setTabWidget(TabWidget* tabWidget)
{
	m_tabWidget = tabWidget;
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
	if (m_tabWidget) {
		int index{m_tabWidget->addView(QUrl(), tabType)};
		m_tabWidget->webTab(index)->addressBar()->showUrl(request.url());
		m_tabWidget->webTab(index)->load(request);
	}
}

bool TabbedWebView::isFullScreen()
{
	return (m_tabWidget->window() && m_tabWidget->window()->isFullScreen());
}

void TabbedWebView::requestFullScreen(bool enable)
{
	if (!m_tabWidget->window())
		return;

	if (enable)
		m_tabWidget->window()->enterHtmlFullScreen();
	else
		m_tabWidget->window()->showNormal();
}

void TabbedWebView::setAsCurrentTab()
{
	if (m_tabWidget)
		m_tabWidget->setCurrentWidget(m_webTab);
}

void TabbedWebView::userLoadAction(const LoadRequest& request)
{
	load(request);
}

#ifdef EXP_TRANSPARENT_BG

void TabbedWebView::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	if (m_tabWidget->window()->background() != nullptr && isTransparent()) {
		QPoint global_position = mapTo(m_tabWidget->window(), QPoint(0, 0));
		QRect shot_rect(global_position.x(), global_position.y(), width(), height());
		painter.drawImage(QPoint(), *m_tabWidget->window()->processedBackground(), shot_rect);
	}
	WebView::paintEvent(event);
}

#endif // EXP_TRANSPARENT_BG

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

#ifdef EXP_TRANSPARENT_BG
void TabbedWebView::sPageRendering()
{
	page()->runJavaScript(Scripts::getAllMetaAttributes(), QWebEngineScript::ApplicationWorld, [this](const QVariant &res)
	{
		const QVariantList& list = res.toList();

		page()->setBackgroundColor(Qt::white);

		foreach(const QVariant& val, list)
		{
			const QVariantMap& meta = val.toMap();
			QString name = meta.value(QStringLiteral("name")).toString();
			QString content = meta.value(QStringLiteral("content")).toString();

			if (name == "sielo-transparent-background" && content.toLower() != "false") {
				page()->setBackgroundColor(Qt::transparent);
			}
		}

		repaint();
	});
}
#endif // EXP_TRANSPARENT_BG

void TabbedWebView::urlChanged(const QUrl& url)
{
	if (Application::instance()->useTopToolBar() && (m_webTab->isCurrentTab() && m_webTab->tabWidget()))
		m_webTab->tabWidget()->navigationToolBar()->refreshBackForwardButtons();
}

void TabbedWebView::linkHovered(const QString& link)
{
	if (m_webTab->isCurrentTab() && m_tabWidget->window()) {
		if (link.isEmpty()) {
			m_tabWidget->statusBarMessage()->clearMessage();
		}
		else {
			m_tabWidget->statusBarMessage()->showMessage(link);
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

void TabbedWebView::inspectElement()
{
	if (m_webTab->haveInspector())
		triggerPageAction(QWebEnginePage::InspectElement);
	else
		m_webTab->showWebInspector(true);
}

void TabbedWebView::newContextMenuEvent(QContextMenuEvent* event)
{
	m_menu->clear();

	WebHitTestResult hitTest{page()->hitTestContent(event->pos())};
	createContextMenu(m_menu, hitTest);
	m_menu->addSeparator();
	m_menu->addAction(Application::getAppIcon("text-html"), tr("Show so&urce code"), this, &WebView::showSource);
	m_menu->addAction(tr("Inspect Element"), this, &TabbedWebView::inspectElement);

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

void TabbedWebView::dragEnterEvent(QDragEnterEvent* event)
{
	const QMimeData* mime{event->mimeData()};

	if (mime->hasFormat("application/sielo.tabbar.tab")) {
		event->acceptProposedAction();
		if (!m_highlightedFrame) {
			m_highlightedFrame = new QFrame(this);
			m_highlightedFrame->setObjectName(QLatin1String("highlighted-new-tabsspace"));
			m_highlightedFrame->setStyleSheet(
				"#highlighted-new-tabsspace{background: rgba(66, 134, 244, 0.5);}" + styleSheet());
			m_highlightedFrame->setAttribute(Qt::WA_TransparentForMouseEvents);
			m_highlightedFrame->show();
		}
		return;
	}

	WebView::dragEnterEvent(event);
}

void TabbedWebView::dragMoveEvent(QDragMoveEvent* event)
{
	if (event->mimeData()->hasFormat("application/sielo.tabbar.tab")) {
		QRect topRect(x(), y(), width(), height() / 3);
		QRect bottomRect(x(), (y() + height()) - height() / 3, width(), height() / 3);
		QRect leftRect(x(), y(), width() / 2, height());
		QRect rightRect((x() + width()) - width() / 2, y(), width() / 2, height());

		if (topRect.contains(event->pos())) {
			m_highlightedFrame->move(topRect.topLeft());
			m_highlightedFrame->setFixedSize(width(), height() / 3);
		}
		else if (bottomRect.contains(event->pos())) {
			m_highlightedFrame->move(bottomRect.topLeft());
			m_highlightedFrame->setFixedSize(width(), height() / 3);
		}
		else if (leftRect.contains(event->pos())) {
			m_highlightedFrame->move(leftRect.topLeft());
			m_highlightedFrame->setFixedSize(width() / 2, height());
		}
		else if (rightRect.contains(event->pos())) {
			m_highlightedFrame->move(rightRect.topLeft());
			m_highlightedFrame->setFixedSize(width() / 2, height());
		}

		event->setDropAction(Qt::MoveAction);
		event->accept();
		return;
	}

	WebView::dragMoveEvent(event);
}

void TabbedWebView::dragLeaveEvent(QDragLeaveEvent* event)
{
	if (!m_cursorIn) {
		m_highlightedFrame->deleteLater();
		m_highlightedFrame = nullptr;
	}

	WebView::dragLeaveEvent(event);
}

void TabbedWebView::dropEvent(QDropEvent* event)
{
	const QMimeData* mime{event->mimeData()};

	if (mime->hasFormat("application/sielo.tabbar.tab")) {
		TabWidget* newTabWidget{nullptr};

		QRect topRect(x(), y(), width(), height() / 3);
		QRect bottomRect(x(), (y() + height()) - height() / 3, width(), height() / 3);
		QRect leftRect(x(), y(), width() / 2, height());
		QRect rightRect((x() + width()) - width() / 2, y(), width() / 2, height());

		if (topRect.contains(event->pos())) {
			newTabWidget = m_tabWidget->window()->tabsSpaceSplitter()->createNewTabsSpace(TabsSpaceSplitter::TSP_Top, m_tabWidget);
		}
		else if (bottomRect.contains(event->pos())) {
			newTabWidget = m_tabWidget->window()->tabsSpaceSplitter()->createNewTabsSpace(TabsSpaceSplitter::TSP_Bottom, m_tabWidget);
		}
		else if (leftRect.contains(event->pos())) {
			newTabWidget = m_tabWidget->window()->tabsSpaceSplitter()->createNewTabsSpace(TabsSpaceSplitter::TSP_Left, m_tabWidget);
		}
		else if (rightRect.contains(event->pos())) {
			newTabWidget = m_tabWidget->window()->tabsSpaceSplitter()->createNewTabsSpace(TabsSpaceSplitter::TSP_Right, m_tabWidget);
		}

		if (newTabWidget)
			newTabWidget->tabBar()->dropEvent(event);

		m_highlightedFrame->deleteLater();
		m_highlightedFrame = nullptr;

		return;
	}
	/*if (mime->hasFormat("application/sielo.tabbar.tab")) {
		MainTabBar* mainTabBar{qobject_cast<MainTabBar*>(qobject_cast<TabBar*>(event->source())->comboTabBar())};
		QByteArray tabData{event->mimeData()->data("sielo/tabdata")};
		QDataStream dataStream{&tabData, QIODevice::ReadOnly};

		int index{-1};
		dataStream >> index;

		TabWidget* sourceTabWidget{mainTabBar->tabWidget()};
		WebTab* webTab{sourceTabWidget->webTab(index)};
		int tabCount = sourceTabWidget->normalTabsCount();
		QRect topRect(x(), y(), width(), height() / 3);
		QRect bottomRect(x(), (y() + height()) - height() / 3, width(), height() / 3);
		QRect leftRect(x(), y(), width() / 2, height());
		QRect rightRect((x() + width()) - width() / 2, y(), width() / 2, height());

		if (topRect.contains(event->pos())) {
			m_tabWidget->window()->createNewTabsSpace(BrowserWindow::TSP_Top, webTab, m_webTab->tabBar()->tabWidget());
		}
		else if (bottomRect.contains(event->pos())) {
			m_tabWidget->window()->createNewTabsSpace(BrowserWindow::TSP_Bottom, webTab, m_webTab->tabBar()->tabWidget());
		}
		else if (leftRect.contains(event->pos())) {
			m_tabWidget->window()->createNewTabsSpace(BrowserWindow::TSP_Left, webTab, m_webTab->tabBar()->tabWidget());
		}
		else if (rightRect.contains(event->pos())) {
			m_tabWidget->window()->createNewTabsSpace(BrowserWindow::TSP_Right, webTab, m_webTab->tabBar()->tabWidget());
		}

		m_highlightedFrame->deleteLater();
		m_highlightedFrame = nullptr;

		if (tabCount <= 1) {
			if (sourceTabWidget->window()->tabWidgetsCount() <= 1) {
				QTimer::singleShot(100, this, [=]() {
					sourceTabWidget->window()->close();
				});
			}
			else {
				QTimer::singleShot(100, this, [=]() {
					sourceTabWidget->window()->closeTabsSpace(sourceTabWidget);
				});
			}
		}

		event->accept();
		mainTabBar->resetDragState();

		return;
	}
*/
	WebView::dropEvent(event);
}

void TabbedWebView::enterEvent(QEvent* event)
{
	event->accept();

	m_cursorIn = true;

	if (m_webTab->tabWidget())
		emit m_webTab->tabWidget()->focusIn(m_webTab->tabWidget());
}

void TabbedWebView::leaveEvent(QEvent* event)
{
	event->accept();

	m_cursorIn = false;
}
}
