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

#include "Widgets/Tab/TabBar.hpp"

#include <QStylePainter>

#include <QTimer>

#include <QMimeData>
#include <QDrag>

#include "Widgets/Tab/ComboTabBar.hpp"
#include "Widgets/Tab/TabDrag.hpp"

#include "Application.hpp"

namespace Sn {

static const int ANIMATION_DURATION = 250;


void TabBar::initStyleBaseOption(QStyleOptionTabBarBase* optionTabBase, QTabBar* tabBar, QSize size)
{
	QStyleOptionTab tabOverlap{};
	tabOverlap.shape = tabBar->shape();
	int overlap{tabBar->style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, &tabOverlap, tabBar)};
	QWidget* parent{tabBar->parentWidget()};

	optionTabBase->init(tabBar);
	optionTabBase->shape = tabBar->shape();
	optionTabBase->documentMode = tabBar->documentMode();

	if (parent && overlap > 0) {
		QRect rect{};

		switch (tabOverlap.shape) {
		case QTabBar::RoundedNorth:
		case QTabBar::TriangularNorth:
			rect.setRect(0, size.height() - overlap, size.width(), overlap);
			break;
		case QTabBar::RoundedSouth:
		case QTabBar::TriangularSouth:
			rect.setRect(0, 0, size.width(), overlap);
			break;
		case QTabBar::RoundedEast:
		case QTabBar::TriangularEast:
			rect.setRect(0, 0, overlap, size.height());
		case QTabBar::RoundedWest:
		case QTabBar::TriangularWest:
			rect.setRect(size.width() - overlap, 0, overlap, size.height());
		}

		optionTabBase->rect = rect;
	}
}

TabBar::TabBar(bool isPinnedTabBar, ComboTabBar* comboTabBar) :
		QTabBar(comboTabBar),
		m_comboTabBar(comboTabBar),
		m_pressedIndex(-1),
		m_dragInProgress(false),
		m_activeTabBar(false),
		m_isPinnedTabBar(isPinnedTabBar),
		m_useFastTabSizeHint(false)
{
	// Empty
}

void TabBar::setTabPadding(int padding)
{
	m_tabPadding = padding;
}

void TabBar::setBaseColor(const QColor& color)
{
	m_baseColor = color;
}

void TabBar::setTabButton(int index, QTabBar::ButtonPosition position, QWidget* widget)
{
	QTabBar::setTabButton(index, position, widget);
}

QSize TabBar::tabSizeHint(int index) const
{
	if (this == m_comboTabBar->mainTabBar())
		index += m_comboTabBar->pinnedTabsCount();

	return m_comboTabBar->tabSizeHint(index, m_useFastTabSizeHint);
}

QSize TabBar::baseClassTabSizeHint(int index) const
{
	return QTabBar::tabSizeHint(index);
}

QRect TabBar::draggedTabRect() const
{
	if (!m_dragInProgress)
		return QRect();

	QStyleOptionTab tab{};
	initStyleOption(&tab, m_pressedIndex);

	const int tabDragOffset{dragOffset(&tab, m_pressedIndex)};

	if (tabDragOffset != 0)
		tab.rect.moveLeft(tab.rect.x() + tabDragOffset);

	return tab.rect;
}

QPixmap TabBar::tabPixmap(int index) const
{
	QStyleOptionTab tab{};
	initStyleOption(&tab, index);

	tab.state &= ~QStyle::State_MouseOver;
	tab.position = QStyleOptionTab::OnlyOneTab;
	tab.leftButtonSize = QSize();
	tab.rightButtonSize = QSize();

	QWidget* iconButton{tabButton(index, m_comboTabBar->iconButtonPosition())};
	QWidget* closeButton{tabButton(index, m_comboTabBar->closeButtonPosition())};

	if (iconButton) {
		const QPixmap pixmap{iconButton->grab()};

		if (!pixmap.isNull()) {
			tab.icon = pixmap;
			tab.iconSize = pixmap.size() / pixmap.devicePixelRatioF();
		}
	}

	if (closeButton) {
		const int width{tab.fontMetrics.width(tab.text) + closeButton->width()};
		tab.text = tab.fontMetrics.elidedText(tabText(index), Qt::ElideRight, width);
	}

	QPixmap out{tab.rect.size() * devicePixelRatioF()};
	out.setDevicePixelRatio(devicePixelRatioF());
	out.fill(Qt::transparent);

	tab.rect = QRect(QPoint(0, 0), tab.rect.size());

	QPainter painter{&out};
	style()->drawControl(QStyle::CE_TabBarTab, &tab, &painter, this);
	painter.end();

	return out;
}

void TabBar::setActiveTabBar(bool activate)
{
	if (m_activeTabBar != activate) {
		m_activeTabBar = activate;

		if (!m_activeTabBar) {
			m_comboTabBar->m_blockCurrentChangedSignal = true;
			setCurrentIndex(m_isPinnedTabBar ? count() - 1 : 0);
			m_comboTabBar->m_blockCurrentChangedSignal = false;
		}

		update();
	}
}

void TabBar::removeTab(int index)
{
	m_comboTabBar->m_blockCurrentChangedSignal = true;

	QTabBar::removeTab(index);

	m_comboTabBar->m_blockCurrentChangedSignal = false;
}

void TabBar::setScrollArea(QScrollArea* scrollArea)
{
	m_scrollArea = scrollArea;
}

void TabBar::useFastTabSizeHint(bool enable)
{
	m_useFastTabSizeHint = enable;
}

void TabBar::showDropIndicator(int index, ComboTabBar::DropIndicatorPosition position)
{
	m_dropIndicatorIndex = index;
	m_dropIndicatorPosition = position;

	update();
}

void TabBar::clearDropIndicator()
{
	m_dropIndicatorIndex = -1;

	update();
}

bool TabBar::isDisplayedOnViewport(int globalLeft, int globalRight)
{
	bool isVisible{true};

	if (m_scrollArea) {
		if (globalRight < m_scrollArea->viewport()->mapToGlobal(QPoint(0, 0)).x() ||
			globalLeft > m_scrollArea->viewport()->mapToGlobal(m_scrollArea->viewport()->rect().topRight()).x())
			isVisible = false;
	}

	return isVisible;
}

void TabBar::setCurrentIndex(int index)
{
	if (index == currentIndex() && !m_activeTabBar)
		emit currentChanged(currentIndex());

	QTabBar::setCurrentIndex(index);
}

bool TabBar::event(QEvent* event)
{
	switch (event->type()) {
	case QEvent::ToolTip:
		event->ignore();
		return false;
	default:
		break;
	}

	QTabBar::event(event);
	event->ignore();
	return false;
}

void TabBar::paintEvent(QPaintEvent* event)
{
	QStyleOptionTabBarBase optionTabBase{};
	QStylePainter painter{this};
	int selected{currentIndex()};

	initStyleBaseOption(&optionTabBase, this, size());

	for (int i{0}; i < count(); ++i)
		optionTabBase.tabBarRect |= tabRect(i);

	if (m_activeTabBar)
		optionTabBase.selectedTabRect = tabRect(selected);

//	if (drawBase())
//		painter.drawPrimitive(QStyle::PE_FrameTabBarBase, optionTabBase);

	const QPoint cursorPos{QCursor::pos()};
	int indexUnderMouse{isDisplayedOnViewport(cursorPos.x(), cursorPos.x()) ? tabAt(mapFromGlobal(cursorPos)) : -1};

	for (int i{0}; i < count(); ++i) {
		if (i == selected)
			continue;

		QStyleOptionTab tab{};
		initStyleOption(&tab, i);

		if (!isDisplayedOnViewport(mapToGlobal(tab.rect.topLeft()).x(), mapToGlobal(tab.rect.topRight()).x()))
			continue;

		if (!m_activeTabBar)
			tab.selectedPosition = QStyleOptionTab::NotAdjacent;

		if (!(tab.state & QStyle::State_Enabled))
			tab.palette.setCurrentColorGroup(QPalette::Disabled);

		if (m_dragInProgress && i == indexUnderMouse)
			tab.state |= QStyle::State_MouseOver;
		else
			tab.state &= ~QStyle::State_MouseOver;

		painter.drawControl(QStyle::CE_TabBarTab, tab);
	}

	if (selected >= 0) {
		QStyleOptionTab tab;
		initStyleOption(&tab, selected);

		const int tabDragOffset{dragOffset(&tab, selected)};
		
		if (tabDragOffset != 0) 
			tab.rect.moveLeft(tab.rect.x() + tabDragOffset);

		if (selected == indexUnderMouse)
			tab.state |= QStyle::State_MouseOver;
		else 
			tab.state &= ~QStyle::State_MouseOver;

		if (!m_activeTabBar) {
			QStyleOptionTab tb{tab};
			tb.rect.moveRight((rect().x() + rect().width()) * 2);
			
			painter.drawControl(QStyle::CE_TabBarTab, tb);

			tab.state = tab.state & ~QStyle::State_Selected;
		}

		if (!m_movingTab || !m_movingTab->isVisible()) 
			painter.drawControl(QStyle::CE_TabBarTab, tab);
		else {
			int taboverlap{style()->pixelMetric(QStyle::PM_TabBarTabOverlap, nullptr, this)};
			m_movingTab->setGeometry(tab.rect.adjusted(-taboverlap, 0, taboverlap, 0));

			QRect grabRect{tabRect(selected)};
			grabRect.adjust(-taboverlap, 0, taboverlap, 0);
			
			QPixmap grabImage{grabRect.size() * devicePixelRatioF()};
			grabImage.setDevicePixelRatio(devicePixelRatioF());
			grabImage.fill(Qt::transparent);

			QStylePainter p{&grabImage, this};
			p.initFrom(this);

			if (tabDragOffset != 0) 
				tab.position = QStyleOptionTab::OnlyOneTab;

			tab.rect.moveTopLeft(QPoint(taboverlap, 0));
			p.drawControl(QStyle::CE_TabBarTab, tab);

			m_movingTab->m_pixmap = grabImage;
			m_movingTab->update();
		}
	}

	// Draw drop indicator
	if (m_dropIndicatorIndex != -1) {
		const QRect tr{tabRect(m_dropIndicatorIndex)};
		QRect r{};

		if (m_dropIndicatorPosition == ComboTabBar::BeforeTab) 
			r = QRect(qMax(0, tr.left() - 1), tr.top(), 3, tr.height());
		else {
			const int rightOffset{m_dropIndicatorIndex == count() - 1 ? -2 : 0};
			r = QRect(tr.right() + rightOffset, tr.top(), 3, tr.height());
		}

		QColor color{palette().brush(QPalette::Normal, QPalette::Highlight).color()};
		const int x{ (r.left() + r.right()) / 2};
		const int thickness{qRound(r.width() / 2.0)};
		int alpha{255};
		const int alphaDec{alpha / (thickness + 1)};

		for (int i{0}; i < thickness; i++) {
			color.setAlpha(alpha);
			alpha -= alphaDec;

			painter.setPen(color);
			painter.drawLine(x - i, r.top(), x - i, r.bottom());
			painter.drawLine(x + i, r.top(), x + i, r.bottom());
		}
	}
}

void TabBar::mousePressEvent(QMouseEvent* event)
{
	event->ignore();

	if (event->button() == Qt::LeftButton) {
		m_pressedIndex = tabAt(event->pos());
		if (m_pressedIndex != -1) {
			m_dragStartPosition = event->pos();

			if (m_pressedIndex == currentIndex() && !m_activeTabBar)
				emit currentChanged(currentIndex());
		}
	}

	QTabBar::mousePressEvent(event);
}

void TabBar::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_dragInProgress && m_pressedIndex != -1) {
		if ((event->pos() - m_dragStartPosition).manhattanLength() > QApplication::startDragDistance())
			m_dragInProgress = true;
	}

	QTabBar::mouseMoveEvent(event);

	if (m_dragInProgress && !m_movingTab) {
		const auto objects = children();
		const int taboverlap{style()->pixelMetric(QStyle::PM_TabBarTabOverlap, nullptr, this)};

		QRect grabRect{tabRect(currentIndex())};
		grabRect.adjust(-taboverlap, 0, taboverlap, 0);

		foreach (QObject* obj, objects)
		{
			QWidget* widget = qobject_cast<QWidget*>(obj);

			if (widget && widget->geometry() == grabRect) {
				m_movingTab = static_cast<MovableTab*>(widget);
				break;
			}
		}
	}

	if (m_dragInProgress && m_movingTab) {
		if (isRightToLeft())
			return;

		QRect rect{tabRect(m_pressedIndex)};
		rect.moveLeft(rect.x() + (event->pos().x() - m_dragStartPosition.x()));

		bool sendEvent{false};
		int diff{rect.topRight().x() - tabRect(count() - 1).topRight().x()};

		if (diff > 0)
			sendEvent = true;
		else {
			diff = rect.topLeft().x() - tabRect(0).topLeft().x();

			if (diff < 0)
				sendEvent = true;
		}

		if (sendEvent) {
			QPoint pos{event->pos()};
			pos.setX(pos.x() - diff);

			QMouseEvent ev{event->type(), pos, event->button(), event->buttons(), event->modifiers()};
			QTabBar::mouseMoveEvent(&ev);
		}
	}
}

void TabBar::mouseReleaseEvent(QMouseEvent* event)
{
	event->ignore();

	if (event->button() != Qt::LeftButton) {
		m_pressedIndex = -1;
		m_dragInProgress = false;
		m_dragStartPosition = QPoint();
	}

	QTabBar::mouseReleaseEvent(event);

	update();
}

int TabBar::dragOffset(QStyleOptionTab* option, int tabIndex) const
{
	QRect rect{};
	QWidget* button{tabButton(tabIndex, QTabBar::LeftSide)};

	if (button)
		rect = style()->subElementRect(QStyle::SE_TabBarTabLeftButton, option, this);

	if (!rect.isValid()) {
		button = tabButton(tabIndex, QTabBar::RightSide);
		rect = style()->subElementRect(QStyle::SE_TabBarTabRightButton, option, this);
	}

	if (!button || !rect.isValid())
		return 0;

	return button->pos().x() - rect.topLeft().x();
}

void TabBar::initStyleOption(QStyleOptionTab* option, int tabIndex) const
{
	QTabBar::initStyleOption(option, tabIndex);

	if (m_tabPadding) {
		const QRect textRect{style()->subElementRect(QStyle::SE_TabBarTabText, option, this)};
		const int width{textRect.width() - 2 * m_tabPadding};

		option->text = option->fontMetrics.elidedText(tabText(tabIndex), elideMode(), width, Qt::TextShowMnemonic);
	}

	int index{m_isPinnedTabBar ? tabIndex : m_comboTabBar->pinnedTabsCount() + tabIndex};

	if (m_comboTabBar->count() > 1) {
		if (index == 0)
			option->position = QStyleOptionTab::Beginning;
		else if (index == m_comboTabBar->count() - 1)
			option->position = QStyleOptionTab::End;
		else
			option->position = QStyleOptionTab::Middle;
	}
	else 
		option->position = QStyleOptionTab::OnlyOneTab;
}
}
