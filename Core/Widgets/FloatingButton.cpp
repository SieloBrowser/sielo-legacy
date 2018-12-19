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

#include "Widgets/FloatingButton.hpp"

#include <QtMath>

#include <QCursor>

#include <QGraphicsDropShadowEffect>

#include <QMessageBox>

#include <QPropertyAnimation>

#include "Application.hpp"
#include "BrowserWindow.hpp"

#include "Web/Tab/WebTab.hpp"

#include "Widgets/Tab/TabWidget.hpp"

namespace Sn {

static const int ANIMATION_DURATION = 1000 * 0.2;

FloatingButton::FloatingButton(RootFloatingButton* parent, const QString& name, const QString& toolTip) :
		QPushButton(parent->parentWidget())
{
	setFixedSize(QSize(48, 48));
	setIconSize(QSize(48, 48));
	setFlat(true);
	setObjectName(name);
	setToolTip(toolTip);

	QGraphicsDropShadowEffect* effect{new QGraphicsDropShadowEffect(this)};
	effect->setBlurRadius(4);
	effect->setOffset(0, 2);

	setGraphicsEffect(effect);
	m_parent = parent->parentWidget();

	hide();
}

void FloatingButton::setIndex(int index)
{
	m_index = index;
}

void FloatingButton::mousePressEvent(QMouseEvent* event)
{
	m_offset = event->pos();
	m_oldPosition = pos();

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
		m_blockClick = true;
	}
}

void FloatingButton::mouseReleaseEvent(QMouseEvent* event)
{
	setAttribute(Qt::WA_TransparentForMouseEvents, true);

	if (Application::widgetAt(mapToGlobal(event->pos()))) {
		FloatingButton* button = qobject_cast<FloatingButton*>(Application::widgetAt(mapToGlobal(event->pos())));

		if (button) {
			int buttonIndex = button->index();
			button->setIndex(m_index);
			m_index = buttonIndex;

			move(button->pos());
			button->moveButton(m_oldPosition);

			emit statusChanged();
		} else {
			moveButton(m_oldPosition);
		}
	}

	setAttribute(Qt::WA_TransparentForMouseEvents, false);

	if (!m_blockClick)
			emit isClicked();

	m_blockClick = false;
}

void FloatingButton::moveButton(QPoint destination, int animationTime, bool hideAtEnd)
{
	if (!isVisible())
		show();

	setAttribute(Qt::WA_TransparentForMouseEvents, true);

	QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");

	animation->setDuration(animationTime);
	animation->setStartValue(QRect(pos().x(), pos().y(), width(), height()));
	animation->setEndValue(QRect(destination.x(), destination.y(), width(), height()));
	animation->start(QAbstractAnimation::DeleteWhenStopped);

	connect(animation, &QPropertyAnimation::finished, this, [=]() {
		setAttribute(Qt::WA_TransparentForMouseEvents, false);
		if (hideAtEnd) {
			hide();
		}
	});

}

RootFloatingButton::RootFloatingButton(BrowserWindow* window, QWidget* parent, Pattern pattern) :
		QPushButton(parent),
		m_pattern(pattern),
		m_window(window),
		m_blockClick(false)
{
	setFixedSize(QSize(48, 48));
	setIconSize(QSize(48, 48));
	setFlat(true);
	setObjectName("fbutton-root");

	setToolTip(tr("Floating button"));
	setCursor(Qt::SizeAllCursor);
	setContextMenuPolicy(Qt::CustomContextMenu);

	QGraphicsDropShadowEffect* effect{new QGraphicsDropShadowEffect(this)};
	effect->setBlurRadius(4);
	effect->setOffset(0, 4);

	setGraphicsEffect(effect);

	connect(this, &QPushButton::customContextMenuRequested, this, &RootFloatingButton::showMenu);

	show();
}

RootFloatingButton::~RootFloatingButton()
{
	qDeleteAll(m_buttons);
}

void RootFloatingButton::addButton(const QString& name, const QString& toolTip)
{
	FloatingButton* newButton{new FloatingButton(this, name)};
	newButton->setIndex(m_buttons.size());
	newButton->setToolTip(toolTip);

	connect(newButton, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

	m_buttons.insert(name, newButton);
}

FloatingButton* RootFloatingButton::button(const QString& name)
{
	return m_buttons.value(name);
}

void RootFloatingButton::setPattern(Pattern pattern)
{
	m_pattern = pattern;
}

void RootFloatingButton::expandAround(QPoint around)
{
	if (m_childrenExpanded)
		return;

	m_childrenExpanded = true;

	float deegresSpace = 360 / m_buttons.size();
	int radius = width() + 15;

	for (int i{0}; i < m_buttons.size(); ++i) {
		float degrees = deegresSpace * (i + 1);
		int x = around.x() + radius * qCos(qDegreesToRadians(degrees));
		int y = around.y() + radius * qSin(qDegreesToRadians(degrees));

		foreach (FloatingButton* button, m_buttons) {
			if (button->index() == i) {
				button->move(around);
				button->show();
				button->moveButton(QPoint(x, y));
				break;
			}
		}
	}
}

void RootFloatingButton::expandInToolbar(TabWidget* tabWidget)
{
	if (m_childrenExpanded)
		return;

	m_childrenExpanded = true;

	for (int i{1}; i <= m_buttons.size(); ++i) {
				foreach (FloatingButton* button, m_buttons) {
				if (button->index() == i - 1) {
					QPoint relativePos{tabWidget->mapTo(m_window, tabWidget->pos())};
					QPoint destination{0, 0};

					if (m_pattern == Pattern::TopToolbar)
						destination = QPoint(relativePos.x() + i * width(), relativePos.y());
					if (m_pattern == Pattern::BottomToolbar)
						destination = QPoint(relativePos.x() + i * width(),
											 relativePos.y() + (tabWidget->height() - height() * 2));
					if (m_pattern == Pattern::LeftToolbar)
						destination = QPoint(relativePos.x(), relativePos.y() + i * height());
					if (m_pattern == Pattern::RightToolbar)
						destination = QPoint(relativePos.x() + (tabWidget->width() - width() * 2),
											 relativePos.y() + i * height());

					button->move(pos());
					button->show();
					button->moveButton(destination);
					break;
				}
			}
	}
}

void RootFloatingButton::closeButton()
{
	if (!m_childrenExpanded)
		return;

			foreach (FloatingButton* button, m_buttons) {
			button->moveButton(pos(), 100, true);
		}

	m_childrenExpanded = false;
}

void RootFloatingButton::tabWidgetChanged(TabWidget* tabWidget)
{
	closeButton();

	QPoint position{tabWidget->mapTo(m_window, tabWidget->pos()).x(), tabWidget->mapTo(m_window, tabWidget->pos()).y()};
	int newX{};
	int newY{};

	newX = (position.x() >= 0) ? qMin(m_window->width() - width(), position.x()) : qMax(0, position.x());
	newY = (position.y() >= 0) ? qMin(m_window->height() - height(), position.y()) : qMax(0, position.y());

	move(QPoint(newX, newY));

	if (m_pattern != Pattern::Floating) {
		if (m_pattern == BottomToolbar)
			move(x(), y() + (tabWidget->height() - height() * 2));
		else if (m_pattern == RightToolbar)
			move(x() + (tabWidget->width() - width() * 2), y());

		expandInToolbar(tabWidget);
	}
}

void RootFloatingButton::mousePressEvent(QMouseEvent* event)
{
	m_offset = event->pos();

	QPushButton::mousePressEvent(event);
}

void RootFloatingButton::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		if (m_pattern != Pattern::Floating) {
			m_pattern = Pattern::Floating;
			emit statusChanged();
		}

		if (m_childrenExpanded)
			closeButton();

		QPoint position{mapToParent(event->pos() - m_offset)};
		int x{};
		int y{};

		x = (position.x() >= 0) ? qMin(m_window->width() - width(), position.x()) : qMax(0, position.x());
		y = (position.y() >= 0) ? qMin(m_window->height() - height(), position.y()) : qMax(0, position.y());

		move(QPoint(x, y));
		m_blockClick = true;
	}
}

void RootFloatingButton::mouseReleaseEvent(QMouseEvent* event)
{
	if (!m_blockClick && event->button() == Qt::LeftButton) {
		if (!m_childrenExpanded && m_pattern == Pattern::Floating)
			expandAround(mapToParent(event->pos() - m_offset));
		else if (m_pattern == Floating)
			closeButton();
	}

	m_blockClick = false;
}

void RootFloatingButton::showMenu(const QPoint& pos)
{
	QMenu menu{m_window};
	QMenu patternsMenu{"Patterns", m_window};

	patternsMenu.addAction(tr("Floating"), this, &RootFloatingButton::changePattern)->setData(Pattern::Floating);
	patternsMenu.addSeparator();
	patternsMenu.addAction(tr("Left Toolbar"), this, &RootFloatingButton::changePattern)->setData(Pattern::LeftToolbar);
	patternsMenu.addAction(tr("Right Toolbar"), this, &RootFloatingButton::changePattern)->setData(
			Pattern::RightToolbar);
	patternsMenu.addAction(tr("Top Toolbar"), this, &RootFloatingButton::changePattern)->setData(Pattern::TopToolbar);
	patternsMenu.addAction(tr("Bottom Toolbar"), this, &RootFloatingButton::changePattern)->setData(
			Pattern::BottomToolbar);

	menu.addMenu(&patternsMenu);

	menu.exec(mapToGlobal(QPoint(pos.x(), pos.y() + 1)));
}

void RootFloatingButton::changePattern()
{
	QAction* action{qobject_cast<QAction*>(sender())};

	if (!action)
		return;

	m_pattern = static_cast<Pattern>(action->data().toInt());

	emit patternChanged(m_pattern);
	emit statusChanged();
}

}
