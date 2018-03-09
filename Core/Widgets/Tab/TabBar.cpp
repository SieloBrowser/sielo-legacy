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

#include "Widgets/Tab/TabBar.hpp"

#include <QString>

#include <QStylePainter>
#include <QPixmap>

#include <QTimer>
#include <QtWidgets/QMessageBox>

#include <QMimeData>
#include <QDrag>

#include "Widgets/Tab/ComboTabBar.hpp"
#include "Widgets/Tab/TabDrag.hpp"

#include "Application.hpp"

namespace Sn {

static const int ANIMATION_DURATION = 250;

TabBar::TabBar(bool isPinnedTabBar, ComboTabBar* comboTabBar) :
		QTabBar(comboTabBar),
		m_comboTabBar(comboTabBar),
		m_pressedIndex(-1),
		m_pressedGlobalX(-1),
		m_pressedGlobalY(-1),
		m_dragInProgress(false),
		m_activeTabBar(false),
		m_isPinnedTabBar(isPinnedTabBar),
		m_useFastTabSizeHint(false)
{
	connect(this, &TabBar::tabMoved, this, &TabBar::tabWasMoved);
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
	if (!m_activeTabBar)
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

bool TabBar::isDisplayedOnViewport(int globalLeft, int globalRight)
{
	bool isVisible{true};

	if (m_scrollArea) {
		if (globalRight < m_scrollArea->viewport()->mapToGlobal(QPoint(0, 0)).x()
			|| globalLeft > m_scrollArea->viewport()->mapToGlobal(m_scrollArea->viewport()->rect().topRight()).x())
			isVisible = false;
	}

	return isVisible;
}

bool TabBar::isDragInProgress() const
{
	return m_dragInProgress;
}

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

void TabBar::setCurrentIndex(int index)
{
	if (index == currentIndex() && !m_activeTabBar)
		emit currentChanged(currentIndex());

	QTabBar::setCurrentIndex(index);
}

void TabBar::resetDragState()
{
	if (m_pressedIndex == -1) {
		m_dragInProgress = false;
		update();
	}
}

void TabBar::tabWasMoved(int from, int to)
{
	if (m_pressedIndex != -1) {
		if (m_pressedIndex == from)
			m_pressedIndex = to;
		else {
			const int start{qMin(from, to)};
			const int end{qMax(from, to)};

			if (m_pressedIndex >= start && m_pressedIndex <= end)
				m_pressedIndex += (from < to) ? -1 : 1;
		}
	}
}

void TabBar::tabTearOff()
{
	TabDrag* drag{qobject_cast<TabDrag*>(sender())};

	QByteArray tabData{drag->mimeData()->data("sielo/tabdata")};
	QDataStream dataStream{&tabData, QIODevice::ReadOnly};

	int index{-1};
	dataStream >> index;

	emit detachFromDrop(index);
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
	if (m_dragInProgress) {
		QTabBar::paintEvent(event);
		return;
	}

	QStyleOptionTabBarBase optionTabBase{};
	QStylePainter painter{this};
	int selected{currentIndex()};

	initStyleBaseOption(&optionTabBase, this, size());

	for (int i{0}; i < count(); ++i)
		optionTabBase.tabBarRect |= tabRect(i);

	optionTabBase.selectedTabRect = QRect();

	if (drawBase())
		painter.drawPrimitive(QStyle::PE_FrameTabBarBase, optionTabBase);

	const QPoint cursorPos{QCursor::pos()};
	int indexUnderMouse{isDisplayedOnViewport(cursorPos.x(), cursorPos.x()) ? tabAt(mapFromGlobal(cursorPos)) : -1};

	for (int i{0}; i < count(); ++i) {
		QStyleOptionTab tab{};
		initStyleOption(&tab, i);

		if (i == selected)
			continue;

		if (!isDisplayedOnViewport(mapToGlobal(tab.rect.topLeft()).x(), mapToGlobal(tab.rect.topRight()).x()))
			continue;

		if (!(tab.state & QStyle::State_Enabled))
			tab.palette.setCurrentColorGroup(QPalette::Disabled);

		if (i == indexUnderMouse)
			tab.state |= QStyle::State_MouseOver;
		else
			tab.state &= ~QStyle::State_MouseOver;

		painter.drawControl(QStyle::CE_TabBarTab, tab);
	}

	if (selected >= 0) {
		QStyleOptionTab tab{};
		initStyleOption(&tab, selected);

		if (selected == indexUnderMouse)
			tab.state |= QStyle::State_MouseOver;
		else
			tab.state &= ~QStyle::State_MouseOver;

		if (!m_activeTabBar) {
			QStyleOptionTab tb = tab;
			tb.rect.moveRight((rect().x() + rect().width()) * 2);
			painter.drawControl(QStyle::CE_TabBarTab, tb);

			tab.state = tab.state & ~QStyle::State_Selected;
		}

		painter.drawControl(QStyle::CE_TabBarTab, tab);
	}
}

void TabBar::mousePressEvent(QMouseEvent* event)
{
	event->ignore();

	if (event->button() == Qt::LeftButton) {
		m_pressedIndex = tabAt(event->pos());
		if (m_pressedIndex != -1) {
			m_pressedGlobalX = event->globalX();
			m_pressedGlobalY = event->globalY();
			m_dragInProgress = true;

			if (m_pressedIndex == currentIndex() && !m_activeTabBar)
				emit currentChanged(currentIndex());
		}
	}

	QTabBar::mousePressEvent(event);
}

void TabBar::mouseMoveEvent(QMouseEvent* event)
{
	if (m_dragInProgress && qAbs(m_pressedGlobalY - event->globalY()) >= m_ripOffDistance && count() > 1) {
		// QMessageBox::information(nullptr, "DEBUG", "Must detach the tab");
		QPixmap pixmap = grab(tabRect(m_pressedIndex));

		QByteArray tabData{};
		QDataStream dataStream{&tabData, QIODevice::WriteOnly};
		dataStream << m_pressedIndex;

		QMimeData* mimeData{new QMimeData()};
		mimeData->setData("sielo/tabdata", tabData);

		TabDrag* drag{new TabDrag(this)};
		drag->setMimeData(mimeData);
		drag->setPixmap(pixmap);
		drag->setHotSpot(event->globalPos() - QPoint(m_pressedGlobalX, m_pressedGlobalY));

		drag->exec(Qt::MoveAction, Qt::MoveAction);

		connect(drag, &TabDrag::tearOff, this, &TabBar::tabTearOff);
	}

	QTabBar::mouseMoveEvent(event);
}

void TabBar::mouseReleaseEvent(QMouseEvent* event)
{
	event->ignore();

	if (event->button() != Qt::LeftButton)
		return;

	QTabBar::mouseReleaseEvent(event);

	if (m_pressedIndex >= 0 && m_pressedIndex < count()) {
		const int length{qAbs(m_pressedGlobalX - event->globalX())};
		const int duration{qMin((length * ANIMATION_DURATION) / tabRect(m_pressedIndex).width(), ANIMATION_DURATION)};

		QTimer::singleShot(duration, this, &TabBar::resetDragState);

		m_pressedIndex = -1;
		m_pressedGlobalX = -1;
	}
}

void TabBar::initStyleOption(QStyleOptionTab* option, int tabIndex) const
{
	QTabBar::initStyleOption(option, tabIndex);

	int index{m_isPinnedTabBar ? tabIndex : m_comboTabBar->pinnedTabsCount() + tabIndex};

	if (m_comboTabBar->count() > 1) {
		if (index == 0)
			option->position = QStyleOptionTab::Beginning;
		else if (index == m_comboTabBar->count() - 1)
			option->position = QStyleOptionTab::End;
		else
			option->position = QStyleOptionTab::Middle;
	}
	else {
		option->position = QStyleOptionTab::OnlyOneTab;
	}
}

void TabBar::tabInserted(int index)
{
	if (m_pressedIndex != -1 && index <= m_pressedIndex)
		++m_pressedIndex;
}

void TabBar::tabRemoved(int index)
{
	if (m_pressedIndex != -1) {
		if (index < m_pressedIndex)
			--m_pressedIndex;
		else if (index == m_pressedIndex)
			m_pressedIndex = -1;
	}
}

}