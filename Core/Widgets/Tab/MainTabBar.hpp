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
#ifndef SIELOBROWSER_MAINTABBAR_HPP
#define SIELOBROWSER_MAINTABBAR_HPP

#include <QRect>
#include <QColor>
#include <QPoint>

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "Widgets/Tab/ComboTabBar.hpp"

namespace Sn {
class BrowserWindow;

class TabWidget;

class WebTab;

class MainTabBar: public ComboTabBar {
Q_OBJECT

public:
	MainTabBar(BrowserWindow* window, TabWidget* tabWidget);

	void loadSettings();

	TabWidget* tabWidget() const { return m_tabWidget; }
	void setVisible(bool visible);
	void setForceHidden(bool hidden);

	void overrideTabTextColor(int index, QColor color);
	void restoreTabTextColor(int index);

	void setTabText(int index, const QString& text);
	void updatePinnedTabCloseButton(int index);

	void wheelEvent(QWheelEvent* event);

signals:
	void reloadTab(int index);
	void stopTab(int index);
	void closeAllButCurrent(int index);
	void closeToRight(int index);
	void closeToLeft(int index);
	void duplicateTab(int index);
	void detachTab(int index);

	void moveAddTabButton(int posX);

private slots:
	void currentTabChanged(int index);
	void overflowChanged(bool overflowed);

	void reloadTab() { emit reloadTab(m_clickedTab); }
	void stopTab() { emit stopTab(m_clickedTab); }
	void closeTab() { emit tabCloseRequested(m_clickedTab); }
	void duplicateTab() { emit duplicateTab(m_clickedTab); }
	void detachTab() { emit detachTab(m_clickedTab); }

	void pinTab();
	void muteTab();

	void closeCurrentTab();
	void closeAllButCurrent();
	void closeToRight();
	void closeToLeft();
	void closeTabFromButton();

private:
	inline bool validIndex(int index) const { return (index >= 0 && index < count()); }

	void tabInserted(int index);
	void tabRemoved(int index);

	void hideCloseButton(int index);
	void showCloseButton(int index);

	void contextMenuEvent(QContextMenuEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);

	QSize tabSizeHint(int index, bool fast) const;
	int comboTabBarPixelMetric(ComboTabBar::SizeType sizeType) const;
	WebTab* webTab(int index = -1);

	BrowserWindow* m_window{nullptr};
	TabWidget* m_tabWidget{nullptr};

	bool m_hideTabBarWithOneTab{false};
	int m_showCloseOnInactive{0};
	int m_clickedTab{0};

	mutable int m_normalTabWidth{0};
	mutable int m_activeTabWidth{0};

	QColor m_originalTabTextColor{};
	QPoint m_dragStartPosition{};

	bool m_forceHidden{false};
};

}

#endif //SIELOBROWSER_MAINTABBAR_HPP
