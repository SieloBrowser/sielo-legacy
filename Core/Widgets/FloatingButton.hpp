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
#ifndef SIELOBROWSER_FLOATINGBUTTON_HPP
#define SIELOBROWSER_FLOATINGBUTTON_HPP

#include "SharedDefines.hpp"

#include <QMouseEvent>
#include <QContextMenuEvent>

#include <QPoint>

#include <QPushButton>

#include <QVector>
#include <QHash>

#include "Utils/ToolButton.hpp"

namespace Sn {
class WebTab;

class TabWidget;

class RootFloatingButton;

class BrowserWindow;

class SIELO_SHAREDLIB FloatingButton : public QPushButton {
Q_OBJECT
public:
	FloatingButton(RootFloatingButton* parent, const QString& name, const QString& toolTip = "Floating Button");

	void setIndex(int index);

	int index() const { return m_index; }

	void moveButton(QPoint destination, int animationTime = 0.2 * 1000, bool hideAtEnd = false);

signals:
	void isClicked();
	void statusChanged();

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

private:

	int m_index{0};
	bool m_blockClick{false};

	QPoint m_offset{};
	QPoint m_oldPosition{};

	QWidget* m_parent{nullptr};
};

class SIELO_SHAREDLIB RootFloatingButton : public QPushButton {
Q_OBJECT
public:
	enum Pattern {
		Floating = 1,
		TopToolbar = 2,
		BottomToolbar = 3,
		RightToolbar = 4,
		LeftToolbar = 5
	};

	RootFloatingButton(BrowserWindow* window, QWidget* parent, Pattern pattern = RightToolbar);
	~RootFloatingButton();

	void addButton(const QString& name, const QString& toolTip);
	FloatingButton* button(const QString& name);

	QHash<QString, FloatingButton*> buttons() const { return m_buttons; }

	Pattern pattern() const { return m_pattern; }
	void setPattern(Pattern pattern);

	void expandAround(QPoint around);
	void expandInToolbar(TabWidget* tabWidget);
	void closeButton();

	void tabWidgetChanged(TabWidget* tabWidget);

signals:
	void patternChanged(Pattern newPattern);
	void statusChanged();

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	void showMenu(const QPoint& pos);

private slots:
	void changePattern();

private:
	QHash<QString, FloatingButton*> m_buttons;
	Pattern m_pattern{};

	QPoint m_offset{};

	BrowserWindow* m_window{nullptr};

	bool m_blockClick{false};
	bool m_childrenExpanded{false};
};
}
#endif //SIELOBROWSER_FLOATINGBUTTON_HPP
