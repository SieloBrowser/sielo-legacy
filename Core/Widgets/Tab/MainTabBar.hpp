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
#ifndef SIELOBROWSER_MAINTABBAR_HPP
#define SIELOBROWSER_MAINTABBAR_HPP

#include "SharedDefines.hpp"

#include <QRect>
#include <QColor>
#include <QPoint>

#include <QPointer> 

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFocusEvent>

#include "Widgets/Tab/ComboTabBar.hpp"

namespace Sn
{
class BrowserWindow;

class TabWidget;

class WebTab;

class SIELO_SHAREDLIB TabBarTabMetrics: public QWidget {
	Q_OBJECT
		Q_PROPERTY(int normalMaxWidth READ normalMaxWidth WRITE setNormalMaxWidth)
		Q_PROPERTY(int normalMinWidth READ normalMinWidth WRITE setNormalMinWidth)
		Q_PROPERTY(int activeMinWidth READ activeMinWidth WRITE setActiveMinWidth)
		Q_PROPERTY(int overflowedWidth READ overflowedWidth WRITE setOverflowedWidth)
		Q_PROPERTY(int pinnedWidth READ pinnedWidth WRITE setPinnedWidth)

public:
	void init()
	{
		if (!m_metrics.isEmpty()) {
			return;
		}
		m_metrics[0] = 250;
		m_metrics[1] = 100;
		m_metrics[2] = 100;
		m_metrics[3] = 100;
		m_metrics[4] = -1; // Will be initialized from MainTabBar
	}

	int normalMaxWidth() const { return m_metrics.value(0); }
	void setNormalMaxWidth(int value) { m_metrics[0] = value; }

	int normalMinWidth() const { return m_metrics.value(1); }
	void setNormalMinWidth(int value) { m_metrics[1] = value; }

	int activeMinWidth() const { return m_metrics.value(2); }
	void setActiveMinWidth(int value) { m_metrics[2] = value; }

	int overflowedWidth() const { return m_metrics.value(3); }
	void setOverflowedWidth(int value) { m_metrics[3] = value; }

	int pinnedWidth() const { return m_metrics.value(4); }
	void setPinnedWidth(int value) { m_metrics[4] = value; }

private:
	QHash<int, int> m_metrics;
};


class SIELO_SHAREDLIB MainTabBar: public ComboTabBar {
	Q_OBJECT

public:
	enum TabDropAction {
		NoAction,
		SelectTab,
		PrependTab,
		AppendTab
	};

	MainTabBar(TabWidget* tabWidget);

	void loadSettings();

	TabWidget* tabWidget() const { return m_tabWidget; }
	void setVisible(bool visible);
	void setForceHidden(bool hidden);

	void overrideTabTextColor(int index, QColor color);
	void restoreTabTextColor(int index);

	void setTabText(int index, const QString& text);

	void wheelEvent(QWheelEvent* event);

	void dropEvent(QDropEvent* event) override;

signals:
	void moveAddTabButton(int posX);

private slots:
	void currentTabChanged(int index);
	void overflowChanged(bool overflowed);

	void closeTabFromButton();

private:
	inline bool validIndex(int index) const;

	void tabInserted(int index);
	void tabRemoved(int index);

	void hideCloseButton(int index);
	void showCloseButton(int index);
	void updatePinnedTabCloseButton(int index);

	void contextMenuEvent(QContextMenuEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void enterEvent(QEvent* event) override;

	void dragEnterEvent(QDragEnterEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void dragLeaveEvent(QDragLeaveEvent* event) override;

	TabDropAction tabDropAction(const QPoint& pos, const QRect& tabRect, bool allowSelect) const;

	QSize tabSizeHint(int index, bool fast) const;
	int comboTabBarPixelMetric(ComboTabBar::SizeType sizeType) const;
	WebTab* webTab(int index = -1);

	TabWidget* m_tabWidget{nullptr};

	bool m_hideTabBarWithOneTab{false};
	int m_showCloseOnInactive{0};
	int m_clickedTab{0};

	mutable int m_normalTabWidth{0};
	mutable int m_activeTabWidth{0};

	QColor m_originalTabTextColor{};
	QPoint m_dragStartPosition{};

	bool m_forceHidden{false};

	QPointer<WebTab> m_lastTab{};
};

}

#endif //SIELOBROWSER_MAINTABBAR_HPP
