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

#include "Widgets/Tab/TabScrollBar.hpp"


namespace Sn {
TabScrollBar::TabScrollBar(QWidget* parent) :
	QScrollBar(Qt::Horizontal, parent)
{
	m_animation = new QPropertyAnimation(this, "value", this);
}

TabScrollBar::~TabScrollBar()
{
	// Empty
}

bool TabScrollBar::isScrolling() const
{
	return m_animation->state() == QPropertyAnimation::Running;
}

void TabScrollBar::animateToValue(int to, QEasingCurve::Type type)
{
	to = qBound(minimum(), to, maximum());
	int lenght{qAbs(to - value())};
	int duration{qMin(1500, 200 + lenght / 2)};

	m_animation->stop();
	m_animation->setEasingCurve(type);
	m_animation->setDuration(duration);
	m_animation->setStartValue(value());
	m_animation->setEndValue(to);
	m_animation->start();
}
}