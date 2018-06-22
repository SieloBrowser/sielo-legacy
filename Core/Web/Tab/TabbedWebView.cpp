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
#include "Web/WebHitTestResult.hpp"

QT_BEGIN_NAMESPACE
extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

namespace Sn {
TabbedWebView::TabbedWebView(WebTab* tab) :
		WebView(tab),
		m_window(nullptr),
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

QImage TabbedWebView::applyBlur(QImage src, qreal radius, bool quality, bool alphaOnly, int transposed)
{
	QPixmap ret(src.size());
	QPainter painter(&ret);
	{
		QPixmap big(QSize(src.width() + 2 * radius, src.height() + 2 * radius));
		QPainter big_painter(&big);

		big_painter.drawImage(QPoint(radius, radius), src);

		{
			QPixmap	left(QSize(1, big.height())),
				   right(QSize(1, big.height()));

			QPainter painter_left(&left),
				    painter_right(&right);

			painter_left.drawImage(QPoint(0, radius), src);
			painter_right.drawImage(QPoint(1 - src.width(), radius), src);

			for (int i = 0; i < radius; i++) {
				big_painter.drawImage(QPoint(i, 0), left.toImage());
				big_painter.drawImage(QPoint(radius + src.width() + i, 0), right.toImage());
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
				big_painter.drawImage(QPoint(0, radius + src.height() + i), bottom.toImage());
			}
		}

		QImage bgImage = big.toImage();
		qt_blurImage(&big_painter, bgImage, radius, quality, alphaOnly, transposed);
		painter.drawImage(QPoint(-radius, -radius), big.toImage());
	}
	return ret.toImage();
}

void TabbedWebView::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	if (m_window->background() != nullptr && isTransparent()) {
		QPoint global_position = mapTo(m_window, QPoint(0, 0));
		QRect shot_rect(global_position.x(), global_position.y(), width(), height());
		if (m_processed_bg == nullptr)
		{
			m_processed_bg = new QImage(applyBlur(m_window->background()->toImage(), 100));
		}
		else if (m_processed_bg->size() != m_window->size())
		{
			m_processed_bg = new QImage(applyBlur(m_window->background()->toImage(), 100));
		}
		painter.drawImage(QPoint(), *m_processed_bg, shot_rect);
	}
	WebView::paintEvent(event);
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
	if (Application::instance()->useTopToolBar() && (m_webTab->isCurrentTab() && m_webTab->tabBar()->tabWidget()))
		m_webTab->tabBar()->tabWidget()->navigationToolBar()->refreshBackForwardButtons();

}

void TabbedWebView::linkHovered(const QString& link)
{
	if (m_webTab->isCurrentTab() && m_window) {
		if (link.isEmpty()) {
			m_window->statusBarMessage()->clearMessage();
		}
		else {
			m_window->statusBarMessage()->showMessage(link);
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
	m_menu->addAction(Application::getAppIcon("text-html"), tr("Show so&urce code"), this, &WebView::showSource);
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

void TabbedWebView::dragEnterEvent(QDragEnterEvent* event)
{
	const QMimeData* mime{event->mimeData()};

	if (mime->hasFormat("sielo/tabdata")) {
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
	if (event->mimeData()->hasFormat("sielo/tabdata")) {
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

	if (mime->hasFormat("sielo/tabdata")) {
		MainTabBar* mainTabBar{qobject_cast<MainTabBar*>(qobject_cast<TabBar*>(event->source())->comboTabBar())};
		QByteArray tabData{event->mimeData()->data("sielo/tabdata")};
		QDataStream dataStream{&tabData, QIODevice::ReadOnly};

		int index{-1};
		dataStream >> index;

		TabWidget* sourceTabWidget{mainTabBar->tabWidget()};
		WebTab* webTab{sourceTabWidget->weTab(index)};
		int tabCount = sourceTabWidget->normalTabsCount();
		QRect topRect(x(), y(), width(), height() / 3);
		QRect bottomRect(x(), (y() + height()) - height() / 3, width(), height() / 3);
		QRect leftRect(x(), y(), width() / 2, height());
		QRect rightRect((x() + width()) - width() / 2, y(), width() / 2, height());

		if (topRect.contains(event->pos())) {
			m_window->createNewTabsSpace(BrowserWindow::TSP_Top, webTab, m_webTab->tabBar()->tabWidget());
		}
		else if (bottomRect.contains(event->pos())) {
			m_window->createNewTabsSpace(BrowserWindow::TSP_Bottom, webTab, m_webTab->tabBar()->tabWidget());
		}
		else if (leftRect.contains(event->pos())) {
			m_window->createNewTabsSpace(BrowserWindow::TSP_Left, webTab, m_webTab->tabBar()->tabWidget());
		}
		else if (rightRect.contains(event->pos())) {
			m_window->createNewTabsSpace(BrowserWindow::TSP_Right, webTab, m_webTab->tabBar()->tabWidget());
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

	WebView::dropEvent(event);
}

void TabbedWebView::enterEvent(QEvent* event)
{
	event->accept();

	m_cursorIn = true;
	emit m_webTab->tabBar()->tabWidget()->focusIn(m_webTab->tabBar()->tabWidget());
}

void TabbedWebView::leaveEvent(QEvent* event)
{
	event->accept();

	m_cursorIn = false;
}
}