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

#include "Widgets/EllipseLabel.hpp"

#include <QApplication>

#include <QClipboard>
#include <QMimeData>
#include <QDrag>

#include <QMenu>

namespace Sn {

EllipseLabel::EllipseLabel(QWidget* parent) :
	QLabel(parent)
{
	// Empty
}

EllipseLabel::EllipseLabel(const QString& string) :
	QLabel()
{
	setText(string);
}

QString EllipseLabel::originalText()
{
	return m_originalText;
}

void EllipseLabel::setText(const QString& text)
{
	m_originalText = text;

	QFontMetrics fontMetrics{this->fontMetrics()};
	QString elided{fontMetrics.elidedText(m_originalText, Qt::ElideMiddle, width())};

	QLabel::setText(elided);
}

void EllipseLabel::contextMenuEvent(QContextMenuEvent* event)
{
	if (!(textInteractionFlags() & Qt::TextSelectableByMouse)
		&& !(textInteractionFlags() & Qt::TextSelectableByKeyboard)) {
		event->ignore();
		return;
	}

	QMenu menu{};
	QAction* action{menu.addAction(tr("Copy"), this, &EllipseLabel::copy)};

	action->setShortcut(QKeySequence::Copy);
	action->setEnabled(hasSelectedText());

	menu.exec(event->globalPos());
}

void EllipseLabel::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier)
		copy();
}

void EllipseLabel::resizeEvent(QResizeEvent* event)
{
	QLabel::resizeEvent(event);

	QFontMetrics fontMetrics{this->fontMetrics()};
	QString elided{fontMetrics.elidedText(m_originalText, Qt::ElideMiddle, width())};

	QLabel::setText(elided);
}

void EllipseLabel::mousePressEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
		m_dragStart = event->pos();

	QLabel::mousePressEvent(event);
}

void EllipseLabel::mouseMoveEvent(QMouseEvent* event)
{
	if (!(event->buttons() & Qt::LeftButton) || selectedText().length() != text().length()) {
		QLabel::mouseMoveEvent(event);
		return;
	}

	int manhattanLength{(event->pos() - m_dragStart).manhattanLength()};

	if (manhattanLength <= QApplication::startDragDistance())
		return;

	QDrag* drag{new QDrag(this)};
	QMimeData* mime{new QMimeData};

	mime->setText(m_originalText);

	drag->setMimeData(mime);
	drag->exec();
}

void EllipseLabel::copy()
{
	if (selectedText().length() == text().length())
		QApplication::clipboard()->setText(m_originalText);
	else
		QApplication::clipboard()->setText(selectedText());
}
}