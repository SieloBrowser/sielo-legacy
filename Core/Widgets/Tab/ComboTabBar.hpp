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
#ifndef SIELOBROWSER_COMBOTABBAR_HPP
#define SIELOBROWSER_COMBOTABBAR_HPP

#include "SharedDefines.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QTabBar>

#include <QIcon>
#include <QRect>

#include <QPoint>
#include <QSize>
#include <QColor>

#include <QEvent>
#include <QPaintEvent>
#include <QWheelEvent>

namespace Sn {
class ToolButton;

class TabBar;
class TabBarScrollWidget;
class TabStackedWidget;

class SIELO_SHAREDLIB ComboTabBar: public QWidget {
Q_OBJECT

	Q_PROPERTY(int currentIndex
				   READ
					   currentIndex
				   WRITE
					   setCurrentIndex
				   NOTIFY
				   currentChanged);
	Q_PROPERTY(int count
				   READ
				   count)

public:
	enum SizeType {
		PinnedTabWidth,
		ActiveTabMinimumWidth,
		NormalTabMinimumWidth,
		NormalTabMaximumWidth,
		OverflowedTabWidth,
		ExtraReservedWidth
	};

	enum DropIndicatorPosition {
		BeforeTab,
		AfterTab
	};

	ComboTabBar(QWidget* parent = nullptr);

	int addTab(const QString& text);
	int addTab(const QIcon& icon, const QString& text);

	int insertTab(int index, const QString& text);
	int insertTab(int index, const QIcon& icon, const QString& text, bool pinned = false);

	void removeTab(int index);
	void moveTab(int from, int to);

	bool isTabEnabled(int index) const;
	void setTabEnabled(int index, bool enabled);

	QColor tabTextColor(int index) const;
	void setTabTextColor(int index, const QColor& color);

	QRect tabRect(int index) const;
	QRect draggedTabRect() const;
	QPixmap tabPixmap(int index) const;

	int tabAt(const QPoint& position) const;

	bool emptyArea(const QPoint& position) const;

	int mainTabBarCurentIndex() const;
	int currentIndex() const;
	int count() const;

	bool drawBase() const;
	void setDrawBase(bool drawTheBase);

	Qt::TextElideMode elideMode() const;
	void setElideMode(Qt::TextElideMode elide);

	QString tabText(int index) const;
	void setTabText(int index, const QString& text);

	QString tabToolTip(int index) const;
	void setTabToolTip(int index, const QString& tip);

	bool tabsClosable() const;
	void setTabsClosable(bool closable);

	QWidget* tabButton(int index, QTabBar::ButtonPosition position) const;
	void setTabButton(int index, QTabBar::ButtonPosition position, QWidget* widget);

	QTabBar::SelectionBehavior selectionBehaviorOnRemove() const;
	void setSelectionBehaviorOnRemove(QTabBar::SelectionBehavior behavior);

	bool expanding() const;
	void setExpanding(bool enable);

	bool isMovable() const;
	void setMovable(bool movable);

	bool documentMode() const;
	void setDocumentMode(bool set);

	int pinnedTabsCount() const;
	int normalTabsCount() const;

	bool isPinned(int index) const;

	void setFocusPolicy(Qt::FocusPolicy policy);
	void setObjectName(const QString& name);
	void setMouseTracking(bool enable);

	void insertCloseButton(int index);
	void setCloseButtonsToolTip(const QString& tip);

	QTabBar::ButtonPosition iconButtonPosition() const;
	QSize iconButtonSize() const;
	QTabBar::ButtonPosition closeButtonPosition() const;
	QSize closeButtonSize() const;

	bool validIndex(int index) const;
	void setCurrentNextEnabledIndex(int offset);

	bool usesScrollButtons() const;
	void setUsesScrollButtons(bool useButtons);

	void showDropIndicator(int index, DropIndicatorPosition position);
	void clearDropIndicator();

	bool isDragInProgress() const;
	bool isScrollInProgress() const;
	bool isMainBarOverflowed() const;

	int cornerWidth(Qt::Corner corner) const;
	void addCornerWidget(QWidget* widget, Qt::Corner corner);

	TabBar* qtabBar() const { return m_mainTabBar; }

signals:
	void overFlowChanged(bool overFlow);
	void currentChanged(int index);
	void tabCloseRequested(int index);
	void tabMoved(int from, int to);
	void scrollBarValueChanged(int value);

public slots:
	void setUpLayout();
	void ensureVisible(int index = -1, int xmargin = -1);
	void setCurrentIndex(int index);

protected:
	TabBar* m_mainTabBar{nullptr};
	TabBar* m_pinnedTabBar{nullptr};

	bool m_shouldHideAddTabButton{true};

	int mainTabBarWidth() const;
	int pinTabBarWidth() const;

	bool eventFilter(QObject* obj, QEvent* event);
	bool event(QEvent* event);
	void wheelEvent(QWheelEvent* event);
	void paintEvent(QPaintEvent* event);

	virtual int comboTabBarPixelMetric(SizeType sizeType) const;
	virtual QSize tabSizeHint(int index, bool fast = false) const;
	virtual void tabInserted(int index);
	virtual void tabRemoved(int index);

private slots:
	void setMinimumWidths();
	void sCurrentChanged(int index);
	void sTabCloseRequested(int index);
	void sTabMoved(int from, int to);
	void closeTabFromButton();
	void updateTabBars();
	void emitOverFlowChanged();

private:
	TabBar* mainTabBar() const;
	TabBar* localTabBar(int index = -1) const;

	int toLocalIndex(int globalIndex) const;
	QRect mapFromLocalTabRet(const QRect& rect, QWidget* tabBar) const;
	void updatePinnedTabBarVisibility();

	QHBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_leftLayout{nullptr};
	QHBoxLayout* m_rightLayout{nullptr};
	QWidget* m_leftContainer{nullptr};
	QWidget* m_rightContainer{nullptr};

	TabBarScrollWidget* m_mainTabBarWidget{nullptr};
	TabBarScrollWidget* m_pinnedTabBarWidget{nullptr};

	QString m_closeButtonsToolTip{};
	bool m_mainBarOverFlowed{false};
	bool m_lastAppliedOverflow{false};
	bool m_usesScrollButton{false};
	bool m_blockCurrentChangedSignal{false};

	friend class TabBar;
	friend class TabStackedWidget;

};
}

#endif //SIELOBROWSER_COMBOTABBAR_HPP
