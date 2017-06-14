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

#include <QtWidgets/QLabel>
#include "Widgets/Tab/MainTabBar.hpp"

#include <QSettings>

#include <QMimeData>

#include <QTimer>

#include <QStyle>
#include <QMenu>
#include <QAbstractButton>
#include <QMessageBox>

#include "BrowserWindow.hpp"
#include "Application.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Web/Tab/WebTab.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/AddTabButton.hpp"
#include "Widgets/Tab/TabCloseButton.hpp"

namespace Sn {

MainTabBar::MainTabBar(BrowserWindow* window, TabWidget* tabWidget) :
	ComboTabBar(),
	m_window(window),
	m_tabWidget(tabWidget),
	m_hideTabBarWithOneTab(false),
	m_showCloseOnInactive(0),
	m_clickedTab(0),
	m_normalTabWidth(0),
	m_activeTabWidth(0),
	m_forceHidden(false)
{
	setElideMode(Qt::ElideRight);
	setFocusPolicy(Qt::NoFocus);
	setTabsClosable(false);
	setMouseTracking(true);
	setDocumentMode(true);
	setAcceptDrops(true);
	setDrawBase(false);
	setMovable(true);

	setUsesScrollButtons(true);
	setCloseButtonsToolTip(BrowserWindow::tr("Close Tab"));

	if (Application::instance()->privateBrowsing()) {
		//TODO: QLabel* privateBrowsing{new QLabel(this)};
	}

	setFocusPolicy(Qt::WheelFocus);

	connect(this, &MainTabBar::currentChanged, this, &MainTabBar::currentTabChanged);
	connect(this, &MainTabBar::overflowChanged, this, &MainTabBar::overflowChanged);
}

void MainTabBar::loadSettings()
{
	QSettings settings{};

	settings.beginGroup("Tabs-Settings");

	bool activateLastTab = settings.value("activateLastTabWhenClosingActual", true).toBool();
	m_hideTabBarWithOneTab = settings.value("hideTabsWithOneTab", false).toBool();
	m_showCloseOnInactive = settings.value("showCloseOnInactiveTabs", 0).toInt(0);

	settings.endGroup();

	setSelectionBehaviorOnRemove(activateLastTab ? QTabBar::SelectPreviousTab : QTabBar::SelectRightTab);
	setVisible(!(count() == 1 && m_hideTabBarWithOneTab));

	setUpLayout();

}

void MainTabBar::setVisible(bool visible)
{
	if (m_forceHidden) {
		ComboTabBar::setVisible(false);
		return;
	}

	if (visible)
		visible = !(count() == 1 && m_hideTabBarWithOneTab);

	ComboTabBar::setVisible(visible);
}

void MainTabBar::setForceHidden(bool hidden)
{
	m_forceHidden = hidden;
	setVisible(!m_forceHidden);
}

void MainTabBar::overrideTabTextColor(int index, QColor color)
{
	if (!m_originalTabTextColor.isValid())
		m_originalTabTextColor = tabTextColor(index);

	setTabTextColor(index, color);
}

void MainTabBar::restoreTabTextColor(int index)
{
	setTabTextColor(index, m_originalTabTextColor);
}

void MainTabBar::setTabText(int index, const QString& text)
{
	QString tabText = text;

	tabText.replace(QLatin1Char('&'), QLatin1String("&&"));

	if (WebTab* tab = webTab(index)) {
		if (tab->isPinned()) {
			tabText.clear();
		}
	}

	setTabToolTip(index, text);
	ComboTabBar::setTabText(index, tabText);
}

void MainTabBar::updatePinnedTabCloseButton(int index)
{
	if (!validIndex(index))
		return;

	WebTab* webTab = qobject_cast<WebTab*>(m_tabWidget->widget(index));
	QAbstractButton* button = qobject_cast<QAbstractButton*>(tabButton(index, closeButtonPosition()));

	bool pinned{webTab && webTab->isPinned()};

	if (pinned) {
		if (button)
			button->hide();
	}
	else {
		if (button)
			button->show();
		else
			showCloseButton(index);
	}
}

void MainTabBar::wheelEvent(QWheelEvent* event)
{
	if (Application::instance()->plugins()->processWheelEvent(Application::ON_TabBar, this, event))
		return;

	ComboTabBar::wheelEvent(event);
}

void MainTabBar::currentTabChanged(int index)
{
	if (!validIndex(index))
		return;

	if (m_dragStartPosition.isNull()) {
		showCloseButton(index);
		hideCloseButton(m_tabWidget->lastTabIndex());

		QTimer::singleShot(100, this, [this]()
		{ ensureVisible(); });
	}

	m_tabWidget->currentTabChanged(index);
}

void MainTabBar::overflowChanged(bool overflowed)
{
	if (overflowed && m_showCloseOnInactive != 1) {
		setTabsClosable(false);
		showCloseButton(currentIndex());
	}
}

void MainTabBar::pinTab()
{
	WebTab* webTab = qobject_cast<WebTab*>(m_tabWidget->widget(m_clickedTab));

	if (webTab)
		webTab->togglePinned();
}

void MainTabBar::muteTab()
{
	WebTab* webTab = qobject_cast<WebTab*>(m_tabWidget->widget(m_clickedTab));

	if (webTab)
		webTab->toggleMuted();
}

void MainTabBar::closeCurrentTab()
{
	m_tabWidget->requestCloseTab(currentIndex());
}

void MainTabBar::closeAllButCurrent()
{
	QMessageBox::StandardButton button = QMessageBox::question(this,
															   tr("Close Tabs"),
															   tr("Do you rally want to close other tabs?"),
															   QMessageBox::Yes | QMessageBox::No,
															   QMessageBox::No);

	if (button == QMessageBox::Yes)
		emit closeAllButCurrent(m_clickedTab);
}

void MainTabBar::closeToRight()
{
	QMessageBox::StandardButton button = QMessageBox::question(this,
															   tr("Close Tabs"),
															   tr("Do you really want to close all tabs to right?"),
															   QMessageBox::Yes | QMessageBox::No,
															   QMessageBox::No);

	if (button == QMessageBox::Yes)
		emit closeToRight(m_clickedTab);
}

void MainTabBar::closeToLeft()
{
	QMessageBox::StandardButton button = QMessageBox::question(this,
															   tr("Close Tabs"),
															   tr("Do you really want to close all tabs to left?"),
															   QMessageBox::Yes | QMessageBox::No,
															   QMessageBox::No);

	if (button == QMessageBox::Yes)
		emit closeToLeft(m_clickedTab);

}

void MainTabBar::closeTabFromButton()
{
	QWidget* button = qobject_cast<QWidget*>(sender());
	int tabToClose{-1};

	for (int i{0}; i < count(); ++i) {
		if (tabButton(i, closeButtonPosition()) == button) {
			tabToClose = i;
			break;
		}
	}

	if (tabToClose != -1)
		m_tabWidget->requestCloseTab(tabToClose);
}

void MainTabBar::createNewLeftTabsSpace()
{
	WebTab* tab{webTab(m_clickedTab)};

	m_window->createNewTabsSpace(BrowserWindow::TSP_Left, tab);
}

void MainTabBar::createNewRightTabsSpace()
{
	WebTab* tab{webTab(m_clickedTab)};

	m_window->createNewTabsSpace(BrowserWindow::TSP_Right, tab);
}

void MainTabBar::createNewTopTabsSpace()
{
	WebTab* tab{webTab(m_clickedTab)};

	m_window->createNewTabsSpace(BrowserWindow::TSP_Top, tab);
}

void MainTabBar::createNewBottomTabsSpace()
{
	WebTab* tab{webTab(m_clickedTab)};

	m_window->createNewTabsSpace(BrowserWindow::TSP_Bottom, tab);
}

void MainTabBar::tabInserted(int index)
{
	Q_UNUSED(index);

	setVisible(!(count() == 1 && m_hideTabBarWithOneTab));
}

void MainTabBar::tabRemoved(int index)
{
	Q_UNUSED(index);

	showCloseButton(currentIndex());
	setVisible(!(count() == 1 && m_hideTabBarWithOneTab));

	if (normalTabsCount() == 0) {
		int xForAddTabButton{cornerWidth(Qt::TopLeftCorner) + pinTabBarWidth()};

		if (QApplication::layoutDirection() == Qt::RightToLeft)
			xForAddTabButton = width() - xForAddTabButton;

		emit moveAddTabButton(xForAddTabButton);
	}
}

void MainTabBar::hideCloseButton(int index)
{
	if (!validIndex(index) || tabsClosable())
		return;

	TabCloseButton* button = qobject_cast<TabCloseButton*>(tabButton(index, closeButtonPosition()));

	if (!button)
		return;

	setTabButton(index, closeButtonPosition(), 0);
	button->deleteLater();
}

void MainTabBar::showCloseButton(int index)
{
	if (!validIndex(index))
		return;

	WebTab* webTab = qobject_cast<WebTab*>(m_tabWidget->widget(index));
	QAbstractButton* button = qobject_cast<QAbstractButton*>(tabButton(index, closeButtonPosition()));

	if (button || (webTab && webTab->isPinned()))
		return;

	insertCloseButton(index);
}

void MainTabBar::contextMenuEvent(QContextMenuEvent* event)
{
	int index{tabAt(event->pos())};
	QMenu menu{};
	m_clickedTab = index;

	if (index != -1) {
		WebTab* webTab = qobject_cast<WebTab*>(m_tabWidget->widget(index));

		if (!webTab)
			return;

		if (m_window->webView(m_clickedTab)->isLoading())
			menu.addAction(QIcon::fromTheme(QStringLiteral("process-stop")),
						   tr("&Stop Tab"),
						   this,
						   SLOT(stopTab()));
		else
			menu.addAction(QIcon::fromTheme(QStringLiteral("view-refresh")),
						   tr("&Reload Tab"),
						   this,
						   SLOT(reloadTab()));

		menu.addAction(QIcon::fromTheme("tab-duplicate"), tr("&Duplicate Tab"), this, SLOT(duplicateTab()));

		if (count() > 1 && !webTab->isPinned()) {
			menu.addAction(QIcon::fromTheme("tab-detach"), tr("D&etach Tab"), this, SLOT(detachTab()));
			menu.addSeparator();
			menu.addAction(tr("Add To New Left Tabs Space"), this, &MainTabBar::createNewLeftTabsSpace);
			menu.addAction(tr("Add To New Right Tabs Space"), this, &MainTabBar::createNewRightTabsSpace);
			menu.addAction(tr("Add To New Top Tabs Space"), this, &MainTabBar::createNewTopTabsSpace);
			menu.addAction(tr("Add To New Bottom Tabs Space"), this, &MainTabBar::createNewBottomTabsSpace);
			menu.addSeparator();
		}

		menu.addAction(webTab->isPinned() ? tr("Un&pin Tab") : tr("&Pin Tab"), this, SLOT(pinTab()));
		menu.addAction(webTab->isMuted() ? tr("Un&mute Tab") : tr("&Mute Tab"), this, SLOT(muteTab()));
		menu.addSeparator();
		menu.addAction(tr("Re&load All Tabs"), m_tabWidget, SLOT(reloadAllTabs()));
		menu.addAction(tr("Bookmark &All Tabs"), m_window, SLOT(bookmarkAllTabs()));
		menu.addSeparator();
		menu.addAction(tr("Close Ot&her Tabs"), this, SLOT(closeAllButCurrent()));
		menu.addAction(tr("Close Tabs To The Right"), this, SLOT(closeToRight()));
		menu.addAction(tr("Close Tabs To The Left"), this, SLOT(closeToLeft()));
		menu.addSeparator();
		menu.addAction(QIcon::fromTheme("window-close"), tr("&Close"), this, SLOT(closeTab()));
	}
	else {
		menu.addAction(tr("&New Tab"), m_window, SLOT(addTab()));
		menu.addSeparator();
		menu.addAction(tr("Reloa&d All Tabs"), m_tabWidget, SLOT(reloadAllTabs()));
		menu.addAction(tr("Bookmark &All Tabs"), m_window, SLOT(bookmarkAllTabs()));
		menu.addSeparator();
		menu.addAction(m_tabWidget->isMuted() ? tr("Un&mute Tabs Space") : tr("&Mute Tabs Space"),
					   m_tabWidget,
					   &TabWidget::toggleMuted);
	}

	const QPoint position{event->globalPos()};
	QPoint point{position.x(), position.y() + 1};
	menu.exec(point);
}

void MainTabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (Application::instance()->plugins()->processMouseDoubleClick(Application::ON_TabBar, this, event))
		return;

	if (event->buttons() == Qt::LeftButton && emptyArea(event->pos())) {
		m_tabWidget->addView(QUrl(), Application::NTT_SelectedTabAtEnd, true);
		return;
	}

	ComboTabBar::mouseDoubleClickEvent(event);
}

void MainTabBar::mousePressEvent(QMouseEvent* event)
{
	if (Application::instance()->plugins()->processMousePress(Application::ON_TabBar, this, event))
		return;

	if (event->buttons() == Qt::LeftButton && !emptyArea(event->pos()))
		m_dragStartPosition = mapFromGlobal(event->globalPos());
	else
		m_dragStartPosition = QPoint();

	ComboTabBar::mousePressEvent(event);
}

void MainTabBar::mouseMoveEvent(QMouseEvent* event)
{
	if (Application::instance()->plugins()->processMouseMove(Application::ON_TabBar, this, event))
		return;

	if (!m_dragStartPosition.isNull() && m_tabWidget->buttonAddTab()->isVisible()) {
		int manhattanLenght{(event->pos() - m_dragStartPosition).manhattanLength()};

		if (manhattanLenght > QApplication::startDragDistance())
			m_tabWidget->buttonAddTab()->hide();
	}

	ComboTabBar::mouseMoveEvent(event);
}

void MainTabBar::mouseReleaseEvent(QMouseEvent* event)
{
	m_dragStartPosition = QPoint();

	if (Application::instance()->plugins()->processMouseRelease(Application::ON_TabBar, this, event))
		return;

	if (m_tabWidget->buttonAddTab()->isHidden() && !isMainBarOverflowed())
		QTimer::singleShot(500, m_tabWidget->buttonAddTab(), &AddTabButton::show);

	if (!rect().contains(event->pos())) {
		ComboTabBar::mouseReleaseEvent(event);
		return;
	}

	if (event->button() == Qt::MiddleButton) {
		if (emptyArea(event->pos())) {
			m_tabWidget->addView(QUrl(), Application::NTT_SelectedTabAtEnd, true);
			return;
		}

		int id{tabAt(event->pos())};

		if (id != -1) {
			m_tabWidget->requestCloseTab(id);
			return;
		}
	}

	ComboTabBar::mouseReleaseEvent(event);
}

void MainTabBar::enterEvent(QEvent* event)
{
	event->accept();

	emit m_tabWidget->focusIn(m_tabWidget);
}

void MainTabBar::dragEnterEvent(QDragEnterEvent* event)
{
	const QMimeData* mime{event->mimeData()};

	if (mime->hasUrls()) {
		event->acceptProposedAction();
		return;
	}

	ComboTabBar::dragEnterEvent(event);
}

void MainTabBar::dropEvent(QDropEvent* event)
{
	const QMimeData* mime{event->mimeData()};

	if (!mime->hasUrls()) {
		ComboTabBar::dropEvent(event);
		return;
	}

	int index{tabAt(event->pos())};

	if (index == -1) {
			foreach (const QUrl& url, mime->urls()) m_tabWidget->addView(url, Application::NTT_SelectedTabAtEnd);
	}
	else {
		WebTab* tab{m_window->webView(index)->webTab()};
		if (tab->isRestored())
			tab->webView()->load(mime->urls()[0]);
	}
}

QSize MainTabBar::tabSizeHint(int index, bool fast) const
{
	if (!m_window->isVisible()) {
		return QSize(-1, -1);
	}

	const int pinnedTabWidth = comboTabBarPixelMetric(ComboTabBar::PinnedTabWidth);
	const int minTabWidth = comboTabBarPixelMetric(ComboTabBar::NormalTabMinimumWidth);

	QSize size = ComboTabBar::tabSizeHint(index);

	if (fast) {
		size.setWidth(index >= pinnedTabsCount() ? minTabWidth : pinnedTabWidth);
		return size;
	}

	WebTab* webTab = qobject_cast<WebTab*>(m_tabWidget->widget(index));
	MainTabBar* tabBar = const_cast<MainTabBar*>(this);

	if (webTab && webTab->isPinned())
		size.setWidth(pinnedTabWidth);
	else {
		int availableWidth{mainTabBarWidth() - comboTabBarPixelMetric(ExtraReservedWidth)};

		if (availableWidth < 0)
			return QSize(-1, -1);

		const int normalTabsCount{ComboTabBar::normalTabsCount()};
		const int maxTabWidth{comboTabBarPixelMetric(ComboTabBar::NormalTabMaximumWidth)};

		if (availableWidth >= maxTabWidth * normalTabsCount) {
			m_normalTabWidth = maxTabWidth;
			size.setWidth(m_normalTabWidth);
		}
		else if (normalTabsCount > 0) {
			const int minActiveTabWidth{comboTabBarPixelMetric(ComboTabBar::ActiveTabMinimumWidth)};
			int maxWidthForTab{availableWidth / normalTabsCount};
			int realTabWidth{maxWidthForTab};
			bool adjustingActiveTab{false};

			if (realTabWidth < minActiveTabWidth) {
				maxWidthForTab = normalTabsCount > 1 ? (availableWidth - minActiveTabWidth) / (normalTabsCount - 1) : 0;
				realTabWidth = minActiveTabWidth;
				adjustingActiveTab = true;
			}

			bool tryAdjusting{availableWidth >= minTabWidth * normalTabsCount};

			if (m_showCloseOnInactive != 1 && tabsClosable() && availableWidth < (minTabWidth + 25) * normalTabsCount) {
				tabBar->setTabsClosable(false);
				tabBar->showCloseButton(currentIndex());
			}

			if (tryAdjusting) {
				m_normalTabWidth = maxWidthForTab;

				if (index == mainTabBarCurentIndex()) {
					if (adjustingActiveTab)
						m_activeTabWidth = (availableWidth - minActiveTabWidth - maxWidthForTab * (normalTabsCount - 1))
										   + realTabWidth;
					else
						m_activeTabWidth = (availableWidth - maxWidthForTab * normalTabsCount) + maxWidthForTab;

					size.setWidth(m_activeTabWidth);
				}
				else
					size.setWidth(m_normalTabWidth);
			}
		}

		if (m_showCloseOnInactive != 2 && !tabsClosable() && availableWidth >= (minTabWidth + 25) * normalTabsCount) {
			tabBar->setTabsClosable(true);

			for (int i{0}; i < count(); ++i)
				tabBar->updatePinnedTabCloseButton(i);
		}

	}

	if (index == count() - 1) {
		WebTab* lastMainActiveTab = qobject_cast<WebTab*>(m_tabWidget->widget(mainTabBarCurentIndex()));
		int xForAddTabButton{cornerWidth(Qt::TopLeftCorner) + pinTabBarWidth() + normalTabsCount() * m_normalTabWidth};

		if (lastMainActiveTab && m_activeTabWidth > m_normalTabWidth)
			xForAddTabButton += m_activeTabWidth - m_normalTabWidth;
		if (QApplication::layoutDirection() == Qt::RightToLeft)
			xForAddTabButton = width() - xForAddTabButton;

		emit tabBar->moveAddTabButton(xForAddTabButton);
	}

	return size;
}

int MainTabBar::comboTabBarPixelMetric(ComboTabBar::SizeType sizeType) const
{
	if (!isVisible())
		return -1;

	switch (sizeType) {
	case ComboTabBar::PinnedTabWidth:
		return iconButtonSize().width() + style()->pixelMetric(QStyle::PM_TabBarTabHSpace, 0, this);
	case ComboTabBar::ActiveTabMinimumWidth:
	case ComboTabBar::NormalTabMinimumWidth:
	case ComboTabBar::OverflowedTabWidth:
		return 100;
	case ComboTabBar::NormalTabMaximumWidth:
		return 250;
	case ComboTabBar::ExtraReservedWidth:
		return m_tabWidget->extraReservedWidth();
	default:
		break;
	}

	return -1;
}

WebTab* MainTabBar::webTab(int index)
{
	if (index == -1)
		return qobject_cast<WebTab*>(m_tabWidget->widget(currentIndex()));

	return qobject_cast<WebTab*>(m_tabWidget->widget(index));
}

}
