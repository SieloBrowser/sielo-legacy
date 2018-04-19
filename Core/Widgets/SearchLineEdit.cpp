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

#include <QPainter>
#include <QStyleOptionFrame>

#include "Widgets/ClearButton.hpp"
#include "Widgets/SearchButton.hpp"

namespace Sn {

SearchLineEdit::SearchLineEdit(QWidget* parent) :
	ExLineEdit(parent)
{
	setObjectName(QLatin1String("search-lineedit"));

	m_searchButton = new SearchButton(this);
	m_inactiveText = tr("Search");
	setLeftWidget(m_searchButton);

	QSizePolicy policy = sizePolicy();
	setSizePolicy(QSizePolicy::Preferred, policy.verticalPolicy());

	connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
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

void SearchLineEdit::paintEvent(QPaintEvent* event)
{
	if (lineEdit()->text().isEmpty() && !hasFocus() && !m_inactiveText.isEmpty()) {
		ExLineEdit::paintEvent(event);
		QStyleOptionFrame panel;
		initStyleOption(&panel);
		QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
		QFontMetrics fm = fontMetrics();
		int horizontalMargin = lineEdit()->x();
		QRect lineRect(horizontalMargin + r.x(), r.y() + (r.height() - fm.height() + 1) / 2,
					   r.width() - 2 * horizontalMargin, fm.height());
		QPainter painter(this);
		painter.setPen(palette().brush(QPalette::Disabled, QPalette::Text).color());
		painter.drawText(lineRect, Qt::AlignLeft|Qt::AlignVCenter, m_inactiveText);
	} else {
		ExLineEdit::paintEvent(event);
	}
}

void SearchLineEdit::resizeEvent(QResizeEvent* event)
{
	updateGeometries();
	ExLineEdit::resizeEvent(event);
}

void SearchLineEdit::updateGeometries()
{
	int menuHeight = height();
	int menuWidth = menuHeight + 1;

	if (!m_searchButton->menu())
		menuWidth = (menuHeight / 5) * 4;

	m_searchButton->resize(QSize(menuWidth, menuHeight));
}
}