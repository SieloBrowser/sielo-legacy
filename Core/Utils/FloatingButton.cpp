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

#include "Utils/FloatingButton.hpp"

#include <QCursor>

#include <QMessageBox>

#include "Web/Tab/WebTab.hpp"

namespace Sn {

FloatingButton::FloatingButton(WebTab* parent, QString text) :
	ToolButton(parent),
	m_parent(parent)
{
	this->setText(text);
	setShowMenuInside(true);
}

void FloatingButton::setChildren(QVector<FloatingButton*> children)
{
		foreach (FloatingButton* child, children) {
			child->setParent(m_parent);
			child->hide();
		}

	m_children = children;
}

void FloatingButton::addChildren(QVector<FloatingButton*> children)
{
		foreach (FloatingButton* child, children) {
			child->setParent(m_parent);
			child->hide();
		}

	m_children.append(children);
}

void FloatingButton::addChild(FloatingButton* button)
{
	button->setParent(m_parent);
	button->hide();
	m_children.append(button);
}

void FloatingButton::mousePressEvent(QMouseEvent* event)
{
	m_offset = event->pos();

	if (m_childrenExpanded)
		hideChildren();

	ToolButton::mousePressEvent(event);
}

void FloatingButton::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		QPoint position{mapToParent(event->pos() - m_offset)};
		int x{};
		int y{};

		x = (position.x() >= 0) ? qMin(m_parent->width() - width(), position.x()) : qMax(0, position.x());
		y = (position.y() >= 0) ? qMin(m_parent->height() - height(), position.y()) : qMax(0, position.y());

		move(QPoint(x, y));
		QCursor::setPos(event->globalPos());
	}

	ToolButton::mouseMoveEvent(event);
}

void FloatingButton::mouseReleaseEvent(QMouseEvent* event)
{
	if (!m_children.isEmpty()) {
		if (m_childrenExpanded) {
			hideChildren();
			m_childrenExpanded = false;
		}
		else {
			showChildren(event->pos());
			m_childrenExpanded = true;
		}
	}

	ToolButton::mouseReleaseEvent(event);
}

void FloatingButton::showChildren(QPoint position)
{
	if (m_children.isEmpty() || m_childrenExpanded)
		return;

	position = mapToParent(position - m_offset);

	for (int i{0}; i < m_children.size(); ++i) {
		FloatingButton* child = m_children[i];
		if (i < m_children.size() / 2) {
			child->move(QPoint(position.x() - (width() + 5), (position.y() - height() * (m_children.size() / 2 - i))));
		}
		else if (i == m_children.size() / 2) {
			child->move(QPoint(position.x() - (width() + 5), position.y()));
		}
		else {
			child->move(QPoint(position.x() - (width() + 5), (position.y() + height() * (i - m_children.size() / 2))));
		}
		child->show();
	}
}

void FloatingButton::hideChildren()
{
	if (m_children.isEmpty() || !m_childrenExpanded)
		return;

		foreach (FloatingButton* child, m_children) child->hide();
}

}