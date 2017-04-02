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

#include "Widgets/Tab/TabCloseButton.hpp"

#include <QPainter>
#include <QStyle>
#include <QStyleOption>

#include <QCursor>

#include <QTabBar>

#include "Widgets/Tab/TabBar.hpp"

namespace Sn {

TabCloseButton::TabCloseButton(QWidget* parent) :
	QAbstractButton(parent)
{
	setFocusPolicy(Qt::NoFocus);
	setCursor(Qt::ArrowCursor);
}

void TabCloseButton::enterEvent(QEvent* event)
{
	if (isEnabled())
		update();

	QAbstractButton::enterEvent(event);
}

void TabCloseButton::leaveEvent(QEvent* event)
{
	if (isEnabled())
		update();

	QAbstractButton::leaveEvent(event);
}

void TabCloseButton::paintEvent(QPaintEvent* event)
{
	bool isUnderMouse{rect().contains(mapFromGlobal(QCursor::pos()))};
	QPainter painter{this};
	QStyleOption option{};

	option.init(this);
	option.state |= QStyle::State_AutoRaise;

	if (isEnabled() && isUnderMouse && !isChecked() && !isDown())
		option.state |= QStyle::State_Raised;
	if (isChecked())
		option.state |= QStyle::State_On;
	if (isDown())
		option.state |= QStyle::State_Sunken;

	if (TabBar* tab = qobject_cast<TabBar*>(parent())) {
		int index{tab->currentIndex()};
		QTabBar::ButtonPosition closeSide
			{static_cast<QTabBar::ButtonPosition>(style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, tab))};

		if (tab->tabButton(index, closeSide) == this && tab->isActiveTabBar())
			option.state |= QStyle::State_Selected;
	}

	style()->drawPrimitive(QStyle::PE_IndicatorTabClose, &option, &painter, this);
}

}