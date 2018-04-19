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

#include "ExLineEdit.hpp"

#include <QCompleter>

#include <QPainter>
#include <QStyleOptionFrame>

#include "Widgets/ClearButton.hpp"
#include "Widgets/SearchButton.hpp"

namespace Sn {
ExLineEdit::ExLineEdit(QWidget *parent)
		: QWidget(parent)
		, m_leftWidget(0)
		, m_lineEdit(new QLineEdit(this))
		, m_clearButton(0)
{
	setObjectName(QLatin1String("exlinedit"));
	setFocusPolicy(m_lineEdit->focusPolicy());
	setAttribute(Qt::WA_InputMethodEnabled);
	setSizePolicy(m_lineEdit->sizePolicy());
	setBackgroundRole(m_lineEdit->backgroundRole());
	setMouseTracking(true);
	setAcceptDrops(true);
	setAttribute(Qt::WA_MacShowFocusRect, true);

	m_lineEdit->setFrame(false);
	m_lineEdit->setObjectName(QLatin1String("exlinedit-lineedit"));

	m_lineEdit->setFocusProxy(this);
	m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

	m_clearButton = new ClearButton(this);
	m_clearButton->setObjectName(QLatin1String("exlinedit-clearbutton"));

	connect(m_clearButton, &ClearButton::clicked, m_lineEdit, &QLineEdit::clear);
	connect(m_lineEdit, &QLineEdit::textChanged, m_clearButton, &ClearButton::textChanged);
}

void ExLineEdit::setLeftWidget(QWidget *widget)
{
	m_leftWidget = widget;
	m_leftWidget->setObjectName(QLatin1String("exlinedit-leftWidget"));
}

QWidget *ExLineEdit::leftWidget() const
{
	return m_leftWidget;
}

void ExLineEdit::resizeEvent(QResizeEvent *event)
{
	Q_ASSERT(m_leftWidget);
	updateGeometries();
	QWidget::resizeEvent(event);
}

void ExLineEdit::updateGeometries()
{
	QStyleOptionFrame panel;
	initStyleOption(&panel);
	QRect rect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);

	int height = rect.height();
	int width = rect.width();

	int m_leftWidgetHeight = m_leftWidget->height();
	m_leftWidget->setGeometry(rect.x() + 2,          rect.y() + (height - m_leftWidgetHeight)/2,
							  m_leftWidget->width(), m_leftWidget->height());

	int clearButtonWidth = this->height();
	m_lineEdit->setGeometry(m_leftWidget->x() + m_leftWidget->width(),        0,
							width - clearButtonWidth - m_leftWidget->width(), this->height());

	m_clearButton->setGeometry(this->width() - clearButtonWidth, 0,
							   clearButtonWidth, this->height());
}

void ExLineEdit::initStyleOption(QStyleOptionFrame *option) const
{
	option->initFrom(this);
	option->rect = contentsRect();
	option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this);
	option->midLineWidth = 0;
	option->state |= QStyle::State_Sunken;
	if (m_lineEdit->isReadOnly())
		option->state |= QStyle::State_ReadOnly;
#ifdef QT_KEYPAD_NAVIGATION
	if (hasEditFocus())
        option->state |= QStyle::State_HasEditFocus;
#endif
	option->features = QStyleOptionFrame::None;
}

QSize ExLineEdit::sizeHint() const
{
	m_lineEdit->setFrame(true);
	QSize size = m_lineEdit->sizeHint();
	m_lineEdit->setFrame(false);
	return size;
}

void ExLineEdit::focusInEvent(QFocusEvent *event)
{
	m_lineEdit->event(event);
	QWidget::focusInEvent(event);
}

void ExLineEdit::focusOutEvent(QFocusEvent *event)
{
	m_lineEdit->event(event);

	if (m_lineEdit->completer()) {
		connect(m_lineEdit->completer(), SIGNAL(activated(QString)), m_lineEdit, SLOT(setText(QString)));
	}

	QWidget::focusOutEvent(event);
}

void ExLineEdit::keyPressEvent(QKeyEvent *event)
{
	m_lineEdit->event(event);
}

bool ExLineEdit::event(QEvent *event)
{
	if (event->type() == QEvent::ShortcutOverride)
		return m_lineEdit->event(event);
	return QWidget::event(event);
}

QVariant ExLineEdit::inputMethodQuery(Qt::InputMethodQuery property) const
{
	return m_lineEdit->inputMethodQuery(property);
}

void ExLineEdit::inputMethodEvent(QInputMethodEvent *e)
{
	m_lineEdit->event(e);
}


}