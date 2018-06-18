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

#include "Utils/AutoSaver.hpp"

#include "Mockup/MockupsManager.hpp"

#include "Application.hpp"

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

	m_deleteButton = new QPushButton(tr("X"), this);
	m_deleteButton->setObjectName(QLatin1String("mockups-tabslist-btn-delete-tabsspace"));
	m_deleteButton->hide();

	m_addTabButton = new QPushButton(tr("+"), this);
	m_addTabButton->setObjectName(QLatin1String("mockups-tabslist-btn-addtab"));
	m_addTabButton->hide();

	connect(m_deleteButton, &QPushButton::clicked, this, &MockupsTabsList::deleteItem);
	connect(m_addTabButton, &QPushButton::clicked, this, &MockupsTabsList::addTab);
}

MockupsTabsList::~MockupsTabsList()
{
	// Empty
}

void MockupsTabsList::setParentLayout(QVBoxLayout* layout)
{
	m_parentLayout = layout;
}

MockupItem::TabsSpace *MockupsTabsList::tabsSpace()
{
	MockupItem::TabsSpace* tabsSpace{new MockupItem::TabsSpace()};

	for (int i{0}; i < count(); ++i) {
		MockupItem::Tab* tab{new MockupItem::Tab()};
		QListWidgetItem* itm{item(i)};

		tab->icon = itm->icon();
		tab->title = itm->data(MockupsManager::TitleRole).toString();
		tab->url = itm->data(MockupsManager::UrlRole).toUrl();
		tab->selected = i == 0;
		tab->parent = tabsSpace;

		tabsSpace->tabs.append(tab);
	}

	return tabsSpace;
}

void MockupsTabsList::deleteItem()
{
	delete currentItem();

	if (count() <= 0)
		m_mockupManager->removeTabsSpace(this);
	else 
		m_mockupManager->saver()->changeOccurred();
}

void MockupsTabsList::addTab()
{
	QListWidgetItem* newTab{
		new QListWidgetItem(Application::getAppIcon("webpage"), QString("%1 (%2)").arg(tr("New Tab")).arg("https://"))
	};
	newTab->setData(MockupsManager::TitleRole, tr("New Tab"));
	newTab->setData(MockupsManager::UrlRole, QUrl("https://"));

	addItem(newTab);

	m_mockupManager->saver()->changeOccurred();
}

void MockupsTabsList::dropEvent(QDropEvent* event)
{
	if (event->dropAction() == Qt::IgnoreAction)
		return;

	MockupsTabsList* source = qobject_cast<MockupsTabsList*>(event->source());
	int count = source->count();

	QListWidget::dropEvent(event);

	m_mockupManager->saver()->changeOccurred();

	if (source->count() <= 1)
		m_mockupManager->removeTabsSpace(source);
}

void MockupsTabsList::enterEvent(QEvent* event)
{
	m_deleteButton->move(width() - m_deleteButton->height(), 0);
	m_addTabButton->move(width() - m_deleteButton->height() - m_addTabButton->height(), 0);

	m_deleteButton->show();
	m_addTabButton->show();
}

void MockupsTabsList::leaveEvent(QEvent* event)
{
	m_deleteButton->move(width() - m_deleteButton->height(), 0);
	m_addTabButton->move(width() - m_deleteButton->height() - m_addTabButton->height(), 0);

	m_deleteButton->hide();
	m_addTabButton->hide();
}
}
