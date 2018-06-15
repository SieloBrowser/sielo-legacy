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

#include "MockupsTabsList.hpp"

#include <QMimeData>

#include "Mockup/MockupsManager.hpp"

namespace Sn
{
MockupsTabsList::MockupsTabsList(MockupsManager* manager, QWidget* parent) :
	QListWidget(parent),
	m_mockupManager(manager)
{
	setObjectName(QLatin1String("mockups-tabslist"));

	setMinimumSize(218, 218);

	setAcceptDrops(true);
	setDragEnabled(true);
	setDropIndicatorShown(true);
	setDefaultDropAction(Qt::MoveAction);
	setDragDropMode(QAbstractItemView::DragDrop);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

MockupsTabsList::~MockupsTabsList()
{
	// Empty
}

void MockupsTabsList::setParentLayout(QVBoxLayout* layout)
{
	m_parentLayout = layout;
}

MockupItem::TabsSpace* MockupsTabsList::tabsSpace()
{
	MockupItem::TabsSpace* tabsSpace{ new MockupItem::TabsSpace() };

	for (int i{0}; i < count(); ++i) {
		MockupItem::Tab* tab{ new MockupItem::Tab() };
		QListWidgetItem* itm{ item(i) };

		tab->icon = itm->icon();
		tab->title = itm->data(MockupsManager::TitleRole).toString();
		tab->url = itm->data(MockupsManager::UrlRole).toUrl();
		tab->selected = i == 0;
		tab->parent = tabsSpace;

		tabsSpace->tabs.append(tab);
	}

	return tabsSpace;
}

void MockupsTabsList::dropEvent(QDropEvent* event)
{
	if (event->dropAction() == Qt::IgnoreAction)
		return;

	MockupsTabsList* source = qobject_cast<MockupsTabsList*>(event->source());
	int count = source->count();

	QListWidget::dropEvent(event);

	if (source->count() <= 1) 
		m_mockupManager->removeTabsSpace(source);
}
}
