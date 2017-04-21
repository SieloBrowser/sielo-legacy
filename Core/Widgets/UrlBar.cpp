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

#include "Widgets/UrlBar.hpp"


#include "Web/Tab/WebTab.hpp"

namespace Sn {

UrlBar::UrlBar(WebTab* parent) :
	m_parent(parent)
{
	m_timeLine.setDuration(1000 * 0.3);
	m_timeLine.setFrameRange(0, 100);

	connect(&m_timeLine, &QTimeLine::frameChanged, this, &UrlBar::animateFrame);

//	setMaximumHeight(0);
}

bool UrlBar::canBeHided() const
{
	return !(m_timeLine.state() == QTimeLine::Running);
}

void UrlBar::hide()
{
	if (m_timeLine.state() == QTimeLine::Running || !isVisible())
		return;

	m_oldHeight = height();
	m_oldPosition = pos();

	m_timeLine.setDirection(QTimeLine::Backward);
	m_timeLine.start();

	connect(&m_timeLine, &QTimeLine::finished, this, &UrlBar::sHide);

	if (m_parent)
		m_parent->setFocus();

}

void UrlBar::sHide()
{
	setFixedHeight(m_oldHeight);
	move(m_oldPosition);
	setVisible(false);
}

void UrlBar::startAnimation()
{
	if (m_timeLine.state() == QTimeLine::Running)
		return;

	if (!isVisible())
		show();

	disconnect(&m_timeLine, &QTimeLine::finished, this, &UrlBar::sHide);

	int shown{0};
	int hidden{-(height())};

	move(QPoint(pos().x(), hidden));

	m_stepY = (hidden - shown) / 100.0;
	m_startY = hidden;
	m_stepHeight = height() / 100.0;

	m_timeLine.setDirection(QTimeLine::Forward);
	m_timeLine.start();
}

void UrlBar::animateFrame(int frame)
{
	setFixedHeight(frame * m_stepHeight);
	move(pos().x(), m_startY - frame * m_stepY);
}

void UrlBar::resizeEvent(QResizeEvent* event)
{
	if (event->size().width() != width())
		resize(event->size().width(), height());

	QLineEdit::resizeEvent(event);
}

}