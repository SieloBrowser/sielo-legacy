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

#pragma once
#ifndef SIELOBROWSER_TABBAR_HPP
#define SIELOBROWSER_TABBAR_HPP

#include <QTabBar>
#include <QScrollArea>

#include <QSize>

#include <QStyle>
#include <QStyleOptionTabBarBase>
#include <QStyleOptionTab>

#include <QEvent>
#include <QPaintEvent>
#include <QMouseEvent>

namespace Sn {
class ComboTabBar;

class TabBar: public QTabBar {
Q_OBJECT

public:
	TabBar(bool isPinnedTabBar, ComboTabBar* comboTabBar);

	void setTabButton(int index, QTabBar::ButtonPosition position, QWidget* widget);

	QSize tabSizeHint(int index) const;
	QSize baseClassTabSizeHint(int index) const;

	bool isActiveTabBar() { return m_activeTabBar; }
	void setActiveTabBar(bool activate);

	void removeTab(int index);

	void setScrollArea(QScrollArea* scrollArea);
	void useFastTabSizeHint(bool enable);

	bool isDisplayedOnViewport(int globalLeft, int globalRight);
	bool isDragInProgress() const;

	static void initStyleBaseOption(QStyleOptionTabBarBase* optionTabBase, QTabBar* tabBar, QSize size);

public slots:
	void setCurrentIndex(int index);

private slots:
	void resetDragState();
	void tabWasMoved(int from, int to);

private:
	bool event(QEvent* event);
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	void initStyleOption(QStyleOptionTab* option, int tabIndex) const;

	void tabInserted(int index);
	void tabRemoved(int index);

	ComboTabBar* m_comboTabBar{nullptr};
	QScrollArea* m_scrollArea{nullptr};

	int m_pressedIndex{-1};
	int m_pressedGlobalX{-1};
	bool m_dragInProgress{false};
	bool m_activeTabBar{false};
	bool m_isPinnedTabBar{false};
	bool m_useFastTabSizeHint{false};

};
}

#endif //SIELOBROWSER_TABBAR_HPP
