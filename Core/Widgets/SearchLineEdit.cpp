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

#include "SearchLineEdit.hpp"

#include <QCompleter>

#include <QPainter>
#include <QStyleOptionFrame>

#include "Widgets/ClearButton.hpp"
#include "Widgets/SearchButton.hpp"

namespace Sn {

SearchLineEdit::SearchLineEdit(QWidget* parent) :
	QWidget(parent)
{
	m_lineEdit = new QLineEdit(this);

	setObjectName(QLatin1String("search-lineedit"));

	setFocusPolicy(m_lineEdit->focusPolicy());
	setAttribute(Qt::WA_InputMethodEnabled);
	setSizePolicy(m_lineEdit->sizePolicy());
	setBackgroundRole(m_lineEdit->backgroundRole());
	setMouseTracking(true);
	setAcceptDrops(true);
	setAttribute(Qt::WA_MacShowFocusRect, true);

	QPalette p = m_lineEdit->palette();
	setPalette(p);

	m_lineEdit->setFrame(false);
	m_lineEdit->setFocusProxy(this);
	m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

	QPalette clearPalette = m_lineEdit->palette();
	clearPalette.setBrush(QPalette::Base, QBrush(Qt::transparent));
	m_lineEdit->setPalette(clearPalette);

	m_searchButton = new SearchButton(this);
	m_clearButton = new ClearButton(this);

	QSizePolicy policy = sizePolicy();
	setSizePolicy(QSizePolicy::Preferred, policy.verticalPolicy());

	connect(m_clearButton, &ClearButton::clicked, m_lineEdit, &QLineEdit::clear);
	connect(m_lineEdit, &QLineEdit::textChanged, m_clearButton, &ClearButton::textChanged);
	connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));

	m_inactiveText = tr("Search");
}

SearchLineEdit::~SearchLineEdit()
{
	// Empty
}

void SearchLineEdit::setInactiveText(const QString& text)
{
	m_inactiveText = text;
}

QMenu *SearchLineEdit::menu() const
{
	if (!m_searchButton->menu()) {
		m_searchButton->setMenu(new QMenu(m_searchButton));

		if (isVisible())
			(const_cast<SearchLineEdit*>(this))->updateGeometries();
	}

	return m_searchButton->menu();
}

void SearchLineEdit::setMenu(QMenu *menu)
{
	if (m_searchButton->menu())
		m_searchButton->menu()->deleteLater();

	m_searchButton->setMenu(menu);
	updateGeometries();
}

QSize SearchLineEdit::sizeHint()
{
	m_lineEdit->setFrame(true);
	QSize size{m_lineEdit->sizeHint()};
	m_lineEdit->setFrame(false);

	return size;
}

QVariant SearchLineEdit::inputMethodeQuery(Qt::InputMethodQuery property) const
{
	return m_lineEdit->inputMethodQuery(property);
}

void SearchLineEdit::focusInEvent(QFocusEvent* event)
{
	m_lineEdit->event(event);

	QWidget::focusInEvent(event);
}

void SearchLineEdit::focusOutEvent(QFocusEvent* event)
{
	m_lineEdit->event(event);

	if (m_lineEdit->completer()) {
		connect(m_lineEdit->completer(), SIGNAL(activated(QString)), m_lineEdit, SLOT(setText(QString)));	}

	QWidget::focusOutEvent(event);
}

void SearchLineEdit::keyPressEvent(QKeyEvent* event)
{
	m_lineEdit->event(event);
}

void SearchLineEdit::paintEvent(QPaintEvent* event)
{
	QPainter painter{this};
	QStyleOptionFrame panel{};

	initStyleOption(&panel);
	style()->drawPrimitive(QStyle::PE_PanelLineEdit, &panel, &painter, this);

	if (m_lineEdit->text().isEmpty() && !hasFocus() & !m_inactiveText.isEmpty()) {
		QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
		QFontMetrics fm = fontMetrics();
		int horizontalMargin = lineEdit()->x();
		QRect lineRect(horizontalMargin + r.x(), r.y() + (r.height() - fm.height() + 1) / 2,
					   r.width() - 2 * horizontalMargin, fm.height());
		QPainter painter(this);
		painter.setPen(palette().brush(QPalette::Disabled, QPalette::Text).color());
		painter.drawText(lineRect, Qt::AlignLeft|Qt::AlignVCenter, m_inactiveText);
	}
}

void SearchLineEdit::resizeEvent(QResizeEvent* event)
{
	Q_ASSERT(m_searchButton);
	updateGeometries();
	QWidget::resizeEvent(event);
}

void SearchLineEdit::inputMethodEvent(QInputMethodEvent* event)
{
	m_lineEdit->event(event);
}

bool SearchLineEdit::event(QEvent* event)
{
	if (event->type() == QEvent::ShortcutOverride)
		return m_lineEdit->event(event);

	return QWidget::event(event);
}

void SearchLineEdit::updateGeometries()
{
	int menuHeight = height();
	int menuWidth = menuHeight + 1;

	if (!m_searchButton->menu())
		menuWidth = (menuHeight / 5) * 4;

	m_searchButton->resize(menuWidth, menuHeight);
}

void SearchLineEdit::initStyleOption(QStyleOptionFrame *option) const
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

}