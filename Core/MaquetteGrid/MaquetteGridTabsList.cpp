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

#include "MaquetteGridTabsList.hpp"

#include <QMimeData>

#include "Utils/AutoSaver.hpp"

#include "MaquetteGrid/MaquetteGridManager.hpp"

#include "Application.hpp"

namespace Sn
{
MaquetteGridTabsList::MaquetteGridTabsList(MaquetteGridManager* manager, QWidget* parent) :
	QListWidget(parent),
	m_maquetteGridManager(manager)
{
	setObjectName(QLatin1String("maquettegrid-tabslist"));

	setMinimumSize(218, 218);

	setAcceptDrops(true);
	setDragEnabled(true);
	setDropIndicatorShown(true);
	setDefaultDropAction(Qt::MoveAction);
	setDragDropMode(QAbstractItemView::DragDrop);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_deleteButton = new QPushButton("X", this);
	m_deleteButton->setObjectName(QLatin1String("maquettegrid-tabslist-btn-delete-tabsspace"));
	m_deleteButton->hide();

	m_addTabButton = new QPushButton("+", this);
	m_addTabButton->setObjectName(QLatin1String("maquettegrid-tabslist-btn-addtab"));
	m_addTabButton->hide();

	connect(m_deleteButton, &QPushButton::clicked, this, &MaquetteGridTabsList::deleteItem);
	connect(m_addTabButton, &QPushButton::clicked, this, &MaquetteGridTabsList::addTab);
}

MaquetteGridTabsList::~MaquetteGridTabsList()
{
	// Empty
}

void MaquetteGridTabsList::setParentLayout(QVBoxLayout* layout)
{
	m_parentLayout = layout;
}

TabsSpaceSplitter::SavedTabsSpace MaquetteGridTabsList::tabsSpace()
{
	TabsSpaceSplitter::SavedTabsSpace tabsSpace{this};

	return tabsSpace;
}

void MaquetteGridTabsList::deleteItem()
{
	if (!currentItem())
		return;

	delete currentItem();

	if (count() <= 0)
		m_maquetteGridManager->removeTabsSpace(this);
	else 
		m_maquetteGridManager->saver()->changeOccurred();
}

void MaquetteGridTabsList::addTab()
{
	QListWidgetItem* newTab{
		new QListWidgetItem(Application::getAppIcon("webpage"), QString("%1 (%2)").arg(tr("New Tab")).arg("https://"))
	};
	newTab->setData(MaquetteGridManager::TitleRole, tr("New Tab"));
	newTab->setData(MaquetteGridManager::UrlRole, QUrl("https://"));

	addItem(newTab);

	m_maquetteGridManager->saver()->changeOccurred();
}

void MaquetteGridTabsList::dropEvent(QDropEvent* event)
{
	if (event->dropAction() == Qt::IgnoreAction)
		return;

	MaquetteGridTabsList* source = qobject_cast<MaquetteGridTabsList*>(event->source());
	int count = source->count();

	QListWidget::dropEvent(event);

	m_maquetteGridManager->saver()->changeOccurred();

	if (source->count() <= 1)
		m_maquetteGridManager->removeTabsSpace(source);
}

void MaquetteGridTabsList::enterEvent(QEvent* event)
{
	m_deleteButton->move(width() - m_deleteButton->width(), 0);
	m_addTabButton->move(width() - m_addTabButton->width(), m_deleteButton->height());

	m_deleteButton->show();
	m_addTabButton->show();
}

void MaquetteGridTabsList::leaveEvent(QEvent* event)
{
	m_deleteButton->move(width() - m_deleteButton->width(), 0);
	m_addTabButton->move(width() - m_addTabButton->width(), m_deleteButton->height());

	m_deleteButton->hide();
	m_addTabButton->hide();
}

void MaquetteGridTabsList::keyPressEvent(QKeyEvent* event)
{
	if ((event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) && model())
		deleteItem();
	else
		QListWidget::keyPressEvent(event);
}
}
