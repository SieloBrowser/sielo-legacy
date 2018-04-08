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

#include "TipLabel.hpp"

#include <QToolTip>

#include <QStylePainter>
#include <QStyleOptionFrame>

namespace Sn
{

TipLabel::TipLabel(QWidget* parent) :
	QLabel(parent)
{
	setWindowFlags(Qt::ToolTip);
	setForegroundRole(QPalette::ToolTipText);
	setBackgroundRole(QPalette::ToolTipBase);
	setPalette(QToolTip::palette());
	ensurePolished();
	setFrameStyle(QFrame::NoFrame);
	setMargin(3);

	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	m_timer->setInterval(500);

	connect(m_timer, &QTimer::timeout, this, &TipLabel::hide);
}

void TipLabel::show(QWidget* widget)
{
	m_timer->stop();

	widget->installEventFilter(this);
	QLabel::show();
}

void TipLabel::hideDelayed()
{
	m_timer->start();
}

bool TipLabel::eventFilter(QObject* object, QEvent event)
{
	switch (event.type()) {
	case QEvent::Leave:
	case QEvent::WindowDeactivate:
	case QEvent::Wheel:
		hide();
		break;

	case QEvent::MouseMove:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
		if (object == this)
			hide();
		break;

	default:
		break;
	}

	return false;
}

void TipLabel::paintEvent(QPaintEvent* event)
{
	QStylePainter painter{this};
	QStyleOptionFrame option{};

	option.init(this);
	painter.drawPrimitive(QStyle::PE_PanelTipLabel, option);
	painter.end();

	QLabel::paintEvent(event);
}

}