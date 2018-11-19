#pragma once
#ifndef SIELOBROWSER_TABBAR_HPP
#define SIELOBROWSER_TABBAR_HPP

#include "SharedDefines.hpp"

#include <QTabBar>
#include <QScrollArea>

#include <QSize>

#include <QStyle>
#include <QStyleOptionTabBarBase>
#include <QStyleOptionTab>

#include <QEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>

#include "Widgets/Tab/ComboTabBar.hpp"

namespace Sn
{

class SIELO_SHAREDLIB MovableTab: public QWidget {
public:
	QPixmap m_pixmap{};
};

class SIELO_SHAREDLIB TabBar: public QTabBar {
	Q_OBJECT
		Q_PROPERTY(int tabPadding READ tabPadding WRITE setTabPadding)
		Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor)

public:
	TabBar(bool isPinnedTabBar, ComboTabBar* comboTabBar);

	int tabPadding() const { return m_tabPadding; }
	void  setTabPadding(int padding);

	QColor baseColor() const { return m_baseColor; }
	void setBaseColor(const QColor& color);
		 
	void setTabButton(int index, QTabBar::ButtonPosition position, QWidget* widget);

	QSize tabSizeHint(int index) const;
	QSize baseClassTabSizeHint(int index) const;

	QRect draggedTabRect() const;
	QPixmap tabPixmap(int index) const;

	ComboTabBar* comboTabBar() const { return m_comboTabBar; };

	bool isActiveTabBar() { return m_activeTabBar; }
	void setActiveTabBar(bool activate);

	void removeTab(int index);

	void setScrollArea(QScrollArea* scrollArea);
	void useFastTabSizeHint(bool enable);

	void showDropIndicator(int index, ComboTabBar::DropIndicatorPosition position);
	void clearDropIndicator();

	bool isDisplayedOnViewport(int globalLeft, int globalRight);
	bool isDragInProgress() const { return m_dragInProgress; }

	static void initStyleBaseOption(QStyleOptionTabBarBase* optionTabBase, QTabBar* tabBar, QSize size);

public slots:
	void setCurrentIndex(int index);

private:
	bool event(QEvent* event);
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	int dragOffset(QStyleOptionTab* option, int tabIndex) const;
	void initStyleOption(QStyleOptionTab* option, int tabIndex) const;

	ComboTabBar* m_comboTabBar{nullptr};
	QScrollArea* m_scrollArea{nullptr};
	MovableTab* m_movingTab{nullptr};

	QColor m_baseColor{};
	QPoint m_dragStartPosition{};
	ComboTabBar::DropIndicatorPosition m_dropIndicatorPosition{};

	int m_tabPadding{-1};
	int m_pressedIndex{-1};
	int m_dropIndicatorIndex{-1};
	bool m_dragInProgress{false};
	bool m_activeTabBar{false};
	bool m_isPinnedTabBar{false};
	bool m_useFastTabSizeHint{false};

};
}

#endif //SIELOBROWSER_TABBAR_HPP
