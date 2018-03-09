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
#include <QDragEnterEvent>

namespace Sn {
class ComboTabBar;

class TabBar : public QTabBar {
Q_OBJECT

public:
	TabBar(bool isPinnedTabBar, ComboTabBar* comboTabBar);

	void setTabButton(int index, QTabBar::ButtonPosition position, QWidget* widget);

	QSize tabSizeHint(int index) const;
	QSize baseClassTabSizeHint(int index) const;

	ComboTabBar* comboTabBar() const { return m_comboTabBar; };

	bool isActiveTabBar() { return m_activeTabBar; }

	void setActiveTabBar(bool activate);

	void removeTab(int index);

	void setScrollArea(QScrollArea* scrollArea);
	void useFastTabSizeHint(bool enable);

	bool isDisplayedOnViewport(int globalLeft, int globalRight);
	bool isDragInProgress() const;

	static void initStyleBaseOption(QStyleOptionTabBarBase* optionTabBase, QTabBar* tabBar, QSize size);

signals:
	void detachFromDrop(int index);

public slots:
	void setCurrentIndex(int index);

private slots:
	void resetDragState();
	void tabWasMoved(int from, int to);

	void tabTearOff();

private:
	bool event(QEvent* event);
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	void initStyleOption(QStyleOptionTab* option, int tabIndex) const;

	void tabInserted(int index);
	void tabRemoved(int index);

	ComboTabBar* m_comboTabBar{nullptr};
	QScrollArea* m_scrollArea{nullptr};

	int m_pressedIndex{-1};
	int m_pressedGlobalX{-1};
	int m_pressedGlobalY{-1};
	int m_ripOffDistance{30};
	bool m_dragInProgress{false};
	bool m_activeTabBar{false};
	bool m_isPinnedTabBar{false};
	bool m_useFastTabSizeHint{false};

};
}

#endif //SIELOBROWSER_TABBAR_HPP
