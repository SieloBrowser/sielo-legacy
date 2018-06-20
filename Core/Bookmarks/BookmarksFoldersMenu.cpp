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

#include "BookmarksFoldersMenu.hpp"

#include <QAction>

#include "Bookmarks/BookmarkItem.hpp"
#include "Bookmarks/Bookmarks.hpp"

#include "Application.hpp"

namespace Sn
{
BookmarksFoldersMenu::BookmarksFoldersMenu(QWidget* parent) :
	QMenu(parent)
{
	// Toolbar folder
	BookmarkItem* toolBarFolderItem{Application::instance()->bookmarks()->toolbarFolder()};
	QMenu* toolBarFolderMenu{addMenu(toolBarFolderItem->icon(), toolBarFolderItem->title())};

	createMenu(toolBarFolderMenu, toolBarFolderItem);

	// Menu folder
	BookmarkItem* menuFolderItem{Application::instance()->bookmarks()->menuFolder()};
	QMenu* menuFolderMenu{addMenu(menuFolderItem->icon(), menuFolderItem->title())};

	createMenu(menuFolderMenu, menuFolderItem);

	// Unsorted folder
	BookmarkItem* unsortedFolderItem{Application::instance()->bookmarks()->unsortedFolder()};
	QMenu* unsortedFolderMenu{addMenu(unsortedFolderItem->icon(), unsortedFolderItem->title())};

	createMenu(unsortedFolderMenu, unsortedFolderItem);
}

BookmarksFoldersMenu::~BookmarksFoldersMenu()
{
	// Empty
}

void BookmarksFoldersMenu::folderChoosed()
{
	if (QAction* action = qobject_cast<QAction*>(sender())) {
		BookmarkItem* folder = static_cast<BookmarkItem*>(action->data().value<void*>());

		emit folderSelected(folder);
	}
}

void BookmarksFoldersMenu::createMenu(QMenu* menu, BookmarkItem* parent)
{
	QAction* action{menu->addAction(tr("Choose %1").arg(parent->title()))};
	action->setData(QVariant::fromValue<void*>(static_cast<void*>(parent)));

	connect(action, &QAction::triggered, this, &BookmarksFoldersMenu::folderChoosed);

	menu->addSeparator();

	foreach (BookmarkItem* child, parent->children()) {
		if (child->isFolder()) {
			QMenu* m{menu->addMenu(child->icon(), child->title())};
			createMenu(m, child);
		}
	}
}
}
