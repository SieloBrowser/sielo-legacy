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

#include "HorizontalListWidget.hpp"

namespace Sn {

HorizontalListWidget::HorizontalListWidget(QWidget* parent) :
	QListWidget(parent),
	m_mouseDown(false)
{
	setFocusPolicy(Qt::NoFocus);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setMovement(QListView::Static);
	setResizeMode(QListView::Adjust);
	setViewMode(QListView::IconMode);
	setSelectionRectVisible(false);
}

void HorizontalListWidget::mousePressEvent(QMouseEvent* event)
{
	m_mouseDown = true;

	QListWidget::mousePressEvent(event);
}

void HorizontalListWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (!itemAt(event->pos()))
		return;

	QListWidget::mouseMoveEvent(event);
}

void HorizontalListWidget::mouseReleaseEvent(QMouseEvent* event)
{
	m_mouseDown = false;

	QListWidget::mouseReleaseEvent(event);
}

void HorizontalListWidget::wheelEvent(QWheelEvent* event)
{
	Q_UNUSED(event);
}

}