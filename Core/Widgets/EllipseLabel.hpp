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
#ifndef CORE_ELLIPSELABEL_HPP
#define CORE_ELLIPSELABEL_HPP

#include "SharedDefines.hpp"

#include <QLabel>

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QMouseEvent>

namespace Sn {
class SIELO_SHAREDLIB EllipseLabel: public QLabel {
Q_OBJECT
public:
	EllipseLabel(QWidget* parent);
	EllipseLabel(const QString& string);

	QString originalText();

	void setText(const QString& text);

protected:
	void contextMenuEvent(QContextMenuEvent* event);

	void keyPressEvent(QKeyEvent* event);

	void resizeEvent(QResizeEvent* event);

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

private slots:
	void copy();

private:
	QString m_originalText;
	QPoint m_dragStart;
};

}
#endif //CORE_ELLIPSELABEL_HPP
