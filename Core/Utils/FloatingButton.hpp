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
	FloatingButton(WebTab* parent, QString text);

	QVector<FloatingButton*> children() { return m_children; }
	void setChildren(QVector<FloatingButton*> children);
	void addChildren(QVector<FloatingButton*> children);
	void addChild(FloatingButton* button);

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

private slots:
	void showChildren(QPoint position);
	void hideChildren();

private:
	QPoint m_offset{};

	WebTab* m_parent{nullptr};

	QVector<FloatingButton*> m_children;

	bool m_childrenExpanded{false};
};
}

#endif //SIELOBROWSER_FLOATINGBUTTON_HPP
