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
#ifndef SIELOBROWSER_FLOATINGBUTTON_HPP
#define SIELOBROWSER_FLOATINGBUTTON_HPP

#include <QMouseEvent>

#include <QVector>

#include "Utils/ToolButton.hpp"

namespace Sn {
class WebTab;

class FloatingButton: public ToolButton {
public:
	enum Type {
		Root,
		Child
	};

	enum Pattern {
		Quad,
		Line
	};

	FloatingButton(WebTab* parent, QString text, Type type = Child);

	QVector<FloatingButton*> children() { return m_children; }
	void setChildren(QVector<FloatingButton*> children);
	void addChildren(QVector<FloatingButton*> children);
	void addChild(FloatingButton* button);

	Type type() const { return m_type; }
	void setType(Type type);

	Pattern pattern() const { return m_pattern; }
	void setPattern(Pattern pattern);

	int positionID() const { return m_positionID; }
	void setPositionID(int newPositionID);

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

private slots:
	void showChildren(QPoint position);
	void hideChildren();

private:
	QPoint m_offset{};
	QPoint m_oldPosition{};

	WebTab* m_parent{nullptr};

	QVector<FloatingButton*> m_children;

	Type m_type{Child};
	Pattern m_pattern{Quad};

	bool m_childrenExpanded{false};
	int m_positionID{0};
};
}

#endif //SIELOBROWSER_FLOATINGBUTTON_HPP
