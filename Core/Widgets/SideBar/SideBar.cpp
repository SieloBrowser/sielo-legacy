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

#include "SideBar.hpp"

#include "Utils/SideBarManager.hpp"

#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/SideBar/SideBarInterface.hpp"
#include "Widgets/SideBar/SideBarTitle.hpp"

namespace Sn {
SideBar::SideBar(SideBarManager* manager, TabWidget* tabWidget) :
	QWidget(tabWidget),
	m_tabWidget(tabWidget),
	m_manager(manager)
{
	setObjectName("sidebar");
	setAttribute(Qt::WA_DeleteOnClose);

	m_layout = new QVBoxLayout(this);
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setSpacing(0);

	m_titleBar = new SideBarTitle(QString(), this);

	m_layout->addWidget(m_titleBar);
}

void SideBar::setTitle(const QString& title)
{
	m_titleBar->setTitle(title);
}

void SideBar::setWidget(QWidget* widget)
{
	if (m_layout->count() == 2)
		delete m_layout->itemAt(1)->widget();

	if (widget) {
		widget->setObjectName("sidebar-content");
		widget->setParent(this);
		m_layout->addWidget(widget);
	}
}

void SideBar::close()
{
	m_manager->closeSideBar();

	QWidget* parent = parentWidget();

	if (parent)
		parent->setFocus();

	QWidget::close();
}
}
