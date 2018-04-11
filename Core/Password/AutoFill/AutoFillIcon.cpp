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

#include "AutoFillIcon.hpp"


#include "Password/AutoFill/AutoFillWidget.hpp"

#include "Web/WebView.hpp"

namespace Sn {

AutoFillIcon::AutoFillIcon(QWidget* parent) :
	QLabel(parent)
{
	setObjectName("addressbar-autofillicon");
	setCursor(Qt::PointingHandCursor);
	setToolTip(tr("Choose username to login"));
	setFocusPolicy(Qt::ClickFocus);

	connect(this, &AutoFillIcon::clicked, this, &AutoFillIcon::iconClicked);
}

void AutoFillIcon::setWebView(WebView* view)
{
	m_view = view;
}

void AutoFillIcon::setFormData(const QVector<PasswordEntry>& data)
{
	m_data = data;
}

void AutoFillIcon::iconClicked()
{
	if (!m_view)
		return;

	AutoFillWidget* autoFillWidget{new AutoFillWidget(m_view, this)};

	autoFillWidget->setFormData(m_data);
	autoFillWidget->showAt(parentWidget());
}

void AutoFillIcon::contextMenuEvent(QContextMenuEvent* event)
{
	event->accept();
}

void AutoFillIcon::mousePressEvent(QMouseEvent* event)
{
	QLabel::mousePressEvent(event);

	event->accept();
}

void AutoFillIcon::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
		if (event->modifiers() == Qt::ControlModifier)
			emit middleClicked(event->globalPos());
		else
			emit clicked(event->globalPos());
	}
	else if (event->button() == Qt::MiddleButton && rect().contains(event->pos()))
		emit middleClicked(event->globalPos());
	else
		QLabel::mouseReleaseEvent(event);
}

}