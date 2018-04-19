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

#include "Widgets/Tab/TabBarScrollWidget.hpp"

#include <QStyle>

#include <QFrame>
#include <QHBoxLayout>
#include <QRect>

#include <QApplication>

#include "Utils/ToolButton.hpp"

#include "Widgets/Tab/TabScrollBar.hpp"

#include "Application.hpp"

namespace Sn {

TabBarScrollWidget::TabBarScrollWidget(QTabBar* tabBar, QWidget* parent) :
	QWidget(parent),
	m_tabBar(tabBar),
	m_usesScrollButtons(false),
	m_totalDeltas(0)
{
	m_scrollArea = new QScrollArea(this);
	m_scrollArea->setFocusPolicy(Qt::NoFocus);
	m_scrollArea->setFrameStyle(QFrame::NoFrame);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	m_scrollBar = new TabScrollBar(m_scrollArea);

	m_scrollArea->setHorizontalScrollBar(m_scrollBar);
	m_scrollArea->setWidget(m_tabBar);

	m_leftScrollButton = new ToolButton(this);
	m_leftScrollButton->setObjectName("tabwidget-button-leftscroll");
	m_leftScrollButton->setAutoRaise(true);
	m_leftScrollButton->setAutoRepeat(true);
	m_leftScrollButton->setAutoRepeatDelay(200);
	m_leftScrollButton->setAutoRepeatInterval(200);
	m_leftScrollButton->setIcon(Application::getAppIcon("tabbar-left-arrow", "tabs"));

	m_rightScrollButton = new ToolButton(this);
	m_rightScrollButton->setObjectName("tabwidget-button-rightscroll");
	m_rightScrollButton->setAutoRaise(true);
	m_rightScrollButton->setAutoRepeat(true);
	m_rightScrollButton->setAutoRepeatDelay(200);
	m_rightScrollButton->setAutoRepeatInterval(200);
	m_rightScrollButton->setIcon(Application::getAppIcon("tabbar-right-arrow", "tabs"));

	QHBoxLayout* layout{new QHBoxLayout()};
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_leftScrollButton);
	layout->addWidget(m_scrollArea);
	layout->addWidget(m_rightScrollButton);

	m_scrollArea->viewport()->setAutoFillBackground(false);

	setLayout(layout);

	connect(m_leftScrollButton, &ToolButton::pressed, this, &TabBarScrollWidget::scrollStart);
	connect(m_leftScrollButton, &ToolButton::doubleClicked, this, &TabBarScrollWidget::scrollToLeftEdge);
	connect(m_leftScrollButton, SIGNAL(middleMouseClicked()), this, SLOT(ensureVisible()));
	connect(m_rightScrollButton, &ToolButton::pressed, this, &TabBarScrollWidget::scrollStart);
	connect(m_rightScrollButton, &ToolButton::doubleClicked, this, &TabBarScrollWidget::scrollToRightEdge);
	connect(m_rightScrollButton, SIGNAL(middleMouseClicked()), this, SLOT(ensureVisible()));
	connect(m_scrollBar, &TabScrollBar::valueChanged, this, &TabBarScrollWidget::updateScrollButtonState);

	updateScrollButtonState();
	overflowChanged(false);
}

void TabBarScrollWidget::scrollByWheel(QWheelEvent* event)
{
	event->accept();

	if (m_totalDeltas * event->delta() < 0)
		m_totalDeltas = 0;

	m_totalDeltas += event->delta();

	if (event->orientation() == Qt::Horizontal) {
		if (event->delta() > 0)
			scrollToLeft();
		else if (event->delta() < 0)
			scrollToRight();

		return;
	}

	if (event->orientation() == Qt::Vertical && event->modifiers() == Qt::ControlModifier) {
		if (event->delta() > 0)
			scrollToLeft(10);
		else if (event->delta() < 0)
			scrollToRight(10);

		return;
	}

	int factor{qMax(m_scrollBar->pageStep() / 3, m_scrollBar->singleStep())};
	if ((event->modifiers() & Qt::ControlModifier) || (event->modifiers() & Qt::ShiftModifier))
		factor = m_scrollBar->pageStep();
	int offset{(m_totalDeltas / 120) * factor};

	if (offset != 0) {
		if (isRightToLeft())
			m_scrollBar->animateToValue(m_scrollBar->value() + offset);
		else
			m_scrollBar->animateToValue(m_scrollBar->value() - offset);

		m_totalDeltas -= (offset / factor) * 120;
	}
}

int TabBarScrollWidget::scrollButtonsWidth() const
{
	return m_leftScrollButton->width();
}

bool TabBarScrollWidget::usesScrollButtons() const
{
	return m_usesScrollButtons;
}

void TabBarScrollWidget::setUsesScrollButtons(bool useButtons)
{
	if (useButtons != m_usesScrollButtons) {
		m_usesScrollButtons = useButtons;
		updateScrollButtonState();
		m_tabBar->setElideMode(m_tabBar->elideMode());
	}
}

bool TabBarScrollWidget::isOverflowed() const
{
	return m_tabBar->count() > 0 && m_scrollBar->minimum() != m_scrollBar->maximum();
}

int TabBarScrollWidget::tabAt(const QPoint& position) const
{
	if (m_leftScrollButton->isVisible()
		&& (m_leftScrollButton->rect().contains(position) || m_rightScrollButton->rect().contains(position)))
		return -1;

	return m_tabBar->tabAt(m_tabBar->mapFromGlobal(mapToGlobal(position)));
}

void TabBarScrollWidget::ensureVisible(int index, int xmargin)
{
	if (index == -1)
		index = m_tabBar->currentIndex();

	if (index < 0 || index >= m_tabBar->count())
		return;

	xmargin = qMin(xmargin, m_scrollArea->viewport()->width() / 2);
	const QRect
		logicalTabRect{QStyle::visualRect(m_tabBar->layoutDirection(), m_tabBar->rect(), m_tabBar->tabRect(index))};
	int logicalX{QStyle::visualPos(Qt::LeftToRight, m_scrollArea->viewport()->rect(), logicalTabRect.center()).x()};

	if (logicalX - xmargin < m_scrollBar->value())
		m_scrollBar->animateToValue(qMax(0, logicalX - xmargin));
	else if (logicalX > m_scrollBar->value() + m_scrollArea->viewport()->width() - xmargin)
		m_scrollBar
			->animateToValue(qMin(logicalX - m_scrollArea->viewport()->width() + xmargin, m_scrollBar->maximum()));
}

void TabBarScrollWidget::scrollToLeft(int n, QEasingCurve::Type type)
{
	n = qMax(1, n);
	m_scrollBar->animateToValue(m_scrollBar->value() - n * m_scrollBar->singleStep(), type);
}

void TabBarScrollWidget::scrollToRight(int n, QEasingCurve::Type type)
{
	n = qMax(1, n);
	m_scrollBar->animateToValue(m_scrollBar->value() + n * m_scrollBar->singleStep(), type);
}

void TabBarScrollWidget::scrollToLeftEdge()
{
	m_scrollBar->animateToValue(m_scrollBar->minimum());
}

void TabBarScrollWidget::scrollToRightEdge()
{
	m_scrollBar->animateToValue(m_scrollBar->maximum());
}

void TabBarScrollWidget::setUpLayout()
{
	const int height{m_tabBar->height()};

	setFixedHeight(height);
}

void TabBarScrollWidget::overflowChanged(bool overflowed)
{
	bool showScrollButtons{overflowed && m_usesScrollButtons};

	m_leftScrollButton->setVisible(showScrollButtons);
	m_rightScrollButton->setVisible(showScrollButtons);
}

void TabBarScrollWidget::scrollStart()
{
	bool ctrlModifier{static_cast<bool>(QApplication::keyboardModifiers() & Qt::ControlModifier)};

	if (sender() == m_leftScrollButton) {
		if (ctrlModifier)
			scrollToLeftEdge();
		else
			scrollToLeft(5, QEasingCurve::Linear);
	}
	else if (sender() == m_rightScrollButton) {
		if (ctrlModifier)
			scrollToRightEdge();
		else
			scrollToRight(5, QEasingCurve::Linear);
	}
}

void TabBarScrollWidget::updateScrollButtonState()
{
	m_leftScrollButton->setEnabled(m_scrollBar->value() != m_scrollBar->minimum());
	m_rightScrollButton->setEnabled(m_scrollBar->value() != m_scrollBar->maximum());
}

void TabBarScrollWidget::mouseMoveEvent(QMouseEvent* event)
{
	event->ignore();
}

void TabBarScrollWidget::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	updateScrollButtonState();
}
}