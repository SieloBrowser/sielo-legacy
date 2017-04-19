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

#include <QPropertyAnimation>

#include "Application.hpp"

#include "Web/Tab/WebTab.hpp"

namespace Sn {

static const int ANIMATION_DURATION = 1000 * 0.2;

FloatingButton::FloatingButton(WebTab* parent, Type type) :
	QPushButton(parent),
	m_parent(parent),
	m_type(type),
	m_pattern(FloatingButton::Quad)
{
	setFixedSize(QSize(48, 48));
	setIconSize(QSize(48, 48));
	setFlat(true);
	setObjectName("fbutton");
}

void FloatingButton::setChildren(QVector<FloatingButton*> children)
{
		foreach (FloatingButton* child, children) {
			child->setParent(m_parent);
			child->setPattern(FloatingButton::Line);
			child->setType(FloatingButton::Child);
			child->hide();
		}

	m_children = children;
}

void FloatingButton::addChildren(QVector<FloatingButton*> children)
{
		foreach (FloatingButton* child, children) {
			child->setParent(m_parent);
			child->setPattern(FloatingButton::Line);
			child->setType(FloatingButton::Child);
			child->hide();
		}

	m_children.append(children);
}

void FloatingButton::addChild(FloatingButton* button)
{
	button->setParent(m_parent);
	button->setPattern(FloatingButton::Line);
	button->setType(FloatingButton::Child);
	button->hide();
	m_children.append(button);
}

void FloatingButton::setType(Type type)
{
	m_type = type;
}

void FloatingButton::setPattern(Pattern pattern)
{
	m_pattern = pattern;
}

void FloatingButton::setPositionID(int newPositionID)
{
	m_positionID = newPositionID;
}

void FloatingButton::showChildren(QPoint position)
{
	if (m_children.isEmpty() || m_childrenExpanded)
		return;

	position = mapToParent(position - m_offset);

	int current{-(m_children.size() / 8)};

	if (m_pattern == FloatingButton::Quad) {
		for (int i{0}; i < m_children.size(); ++i) {
			FloatingButton* child = m_children[i];
			child->move(position);
			child->setPositionID(i);

			if (i < m_children.size() / 8) {
				child->moveButton(QPoint(position.x() + current * width(),
										 position.y() - height() * m_children.size() / 8));
				++current;
				continue;
			}
			if (i == m_children.size() / 8) {
				child->moveButton(QPoint(position.x(), position.y() - height() * m_children.size() / 8));
				current = 0;
				continue;
			}
			if (i < m_children.size() / 4) {
				++current;
				child->moveButton(QPoint(position.x() + current * width(),
										 position.y() - height() * m_children.size() / 8));
				continue;
			}
			if (i < m_children.size() / 2) {
				child->moveButton(QPoint(position.x() + width() * m_children.size() / 8,
										 position.y() - (current + 1) * height()));
				--current;
				continue;
			}
			if (i < m_children.size() - m_children.size() / 4) {
				++current;
				child->moveButton(QPoint(position.x() - current * width(),
										 position.y() + height() * m_children.size() / 8));
				continue;
			}
			if (i == m_children.size() - m_children.size() / 4) {
				current = m_children.size() / 8;
				child->moveButton(QPoint(position.x() - width() * m_children.size() / 8,
										 position.y() + current * height()));
				continue;
			}

			--current;
			child
				->moveButton(QPoint(position.x() - width() * m_children.size() / 8, position.y() + current * height()));
		}

	}
}

void FloatingButton::hideChildren()
{
	if (m_children.isEmpty() || !m_childrenExpanded)
		return;

	QVector<FloatingButton*> newChildren;

	for (int i{0}; i < m_children.size(); ++i) {
		for (int j{0}; j < m_children.size(); ++j) {
			FloatingButton* child = m_children[j];

			if (child->positionID() == i) {
				newChildren.append(child);
				continue;
			}
		}
	}

	m_children.clear();
	m_children = newChildren;

		foreach (FloatingButton* child, m_children) child->hideButton(pos());
}

void FloatingButton::mousePressEvent(QMouseEvent* event)
{
	m_offset = event->pos();
	m_oldPosition = pos();

	if (m_childrenExpanded)
		hideChildren();

	QPushButton::mousePressEvent(event);
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

	QPushButton::mouseMoveEvent(event);
}

void FloatingButton::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_type != FloatingButton::Root) {
		setAttribute(Qt::WA_TransparentForMouseEvents, true);

		if (Application::widgetAt(mapToGlobal(event->pos()))
			&& Application::widgetAt(mapToGlobal(event->pos()))->objectName().contains(QLatin1String("fbutton"))
			&& static_cast<FloatingButton*>(Application::widgetAt(mapToGlobal(event->pos())))->pattern()
			   != FloatingButton::Quad) {
			FloatingButton* button = static_cast<FloatingButton*>(Application::widgetAt(mapToGlobal(event->pos())));

			QPoint newPosition{button->pos()};
			int newPositionID{button->positionID()};

			button->moveButton(m_oldPosition);
			button->setPositionID(m_positionID);
			moveButton(newPosition);
			m_positionID = newPositionID;
		}
		else {
			moveButton(m_oldPosition);
		}

		setAttribute(Qt::WA_TransparentForMouseEvents, false);
	}

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

	QPushButton::mouseReleaseEvent(event);
}

void FloatingButton::moveButton(QPoint destination)
{
	if (!isVisible())
		show();

	setAttribute(Qt::WA_TransparentForMouseEvents, true);

	QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");

	animation->setDuration(ANIMATION_DURATION);
	animation->setStartValue(QRect(pos().x(), pos().y(), width(), height()));
	animation->setEndValue(QRect(destination.x(), destination.y(), width(), height()));
	animation->start(QAbstractAnimation::DeleteWhenStopped);

	connect(animation, &QPropertyAnimation::finished, this, [this]()
	{
		setAttribute(Qt::WA_TransparentForMouseEvents, false);
	});

}

void FloatingButton::hideButton(QPoint destination)
{
	if (!isVisible())
		show();

	setAttribute(Qt::WA_TransparentForMouseEvents, true);

	QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");

	animation->setDuration(ANIMATION_DURATION);
	animation->setStartValue(QRect(pos().x(), pos().y(), width(), height()));
	animation->setEndValue(QRect(destination.x(), destination.y(), width(), height()));
	animation->start(QAbstractAnimation::DeleteWhenStopped);

	connect(animation, &QPropertyAnimation::finished, this, [this]()
	{
		setAttribute(Qt::WA_TransparentForMouseEvents, false);
		hide();
	});
}

}