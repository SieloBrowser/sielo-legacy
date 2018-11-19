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

#pragma once
#ifndef SIELOBROWSER_TABBARSCROLLWIDGET_HPP
#define SIELOBROWSER_TABBARSCROLLWIDGET_HPP

#include "SharedDefines.hpp"

#include <QTabBar>
#include <QScrollArea>

#include <QEasingCurve>

#include <QPoint>

#include <QWheelEvent>
#include <QMouseEvent>
#include <QResizeEvent>

namespace Sn {
class ToolButton;

class TabScrollBar;

class SIELO_SHAREDLIB TabBarScrollWidget: public QWidget {
Q_OBJECT

public:
	TabBarScrollWidget(QTabBar* tabBar, QWidget* parent = 0);

	QTabBar* tabBar() { return m_tabBar; }
	QScrollArea* scrollArea() { return m_scrollArea; }
	TabScrollBar* scrollBar() { return m_scrollBar; }

	void scrollByWheel(QWheelEvent* event);

	int scrollButtonsWidth() const;
	bool usesScrollButtons() const;
	void setUsesScrollButtons(bool useButtons);

	bool isOverflowed() const;
	int tabAt(const QPoint& position) const;

public slots:
	void ensureVisible(int index = -1, int xmargin = 132);

	void scrollToLeft(int n = 5, QEasingCurve::Type type = QEasingCurve::OutQuad);
	void scrollToRight(int n = 5, QEasingCurve::Type type = QEasingCurve::OutQuad);
	void scrollToLeftEdge();
	void scrollToRightEdge();

	void setUpLayout();

private slots:
	void overflowChanged(bool overflowed);
	void scrollStart();
	void updateScrollButtonState();

private:
	void mouseMoveEvent(QMouseEvent* event);
	void resizeEvent(QResizeEvent* event);

	QTabBar* m_tabBar{nullptr};
	QScrollArea* m_scrollArea{nullptr};

	TabScrollBar* m_scrollBar{nullptr};
	ToolButton* m_rightScrollButton{nullptr};
	ToolButton* m_leftScrollButton{nullptr};

	bool m_usesScrollButtons{false};
	int m_totalDeltas{0};
};

}

#endif //SIELOBROWSER_TABBARSCROLLWIDGET_HPP
