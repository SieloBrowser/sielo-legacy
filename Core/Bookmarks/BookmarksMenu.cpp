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

#include "BookmarksMenu.hpp"

#include <QAction>

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarksUtils.hpp"
#include "Bookmarks/BookmarkItem.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
BookmarksMenu::BookmarksMenu(QWidget* parent) :
	QMenu(parent)
{
	setTitle(tr("&Bookmarks"));

	addAction(Application::getAppIcon("add-bookmark"), tr("Bookmark &This Page"), this, &BookmarksMenu::bookmarkPage)->
		setShortcut(QKeySequence("Ctrl+D"));
	addAction(tr("Bookmark &All Tabs"), this, &BookmarksMenu::bookmarkAllTabs);
	addAction(Application::getAppIcon("bookmarks"), tr("Organize &Bookmarks"), this, &BookmarksMenu::showBookmarksManager)
		->setShortcut(QKeySequence("Ctrl+Shift+D"));

	addSeparator();

	connect(Application::instance()->bookmarks(), SIGNAL(bookmarkAdded(BookmarkItem*)), this, SLOT(bookmarksChanged()));
	connect(Application::instance()->bookmarks(), SIGNAL(bookmarkRemoved(BookmarkItem*)), this, SLOT(bookmarksChanged()));
	connect(Application::instance()->bookmarks(), SIGNAL(bookmarkChanged(BookmarkItem*)), this, SLOT(bookmarksChanged()));

	connect(this, &QMenu::aboutToShow, this, &BookmarksMenu::aboutToShow);
	connect(this, &QMenu::aboutToShow, this, &BookmarksMenu::menuAboutToShow);
}

BookmarksMenu::~BookmarksMenu()
{
	// Empty
}

void BookmarksMenu::setMainWindow(BrowserWindow* window)
{
	Q_ASSERT(window);

	m_window = window;
}

void BookmarksMenu::bookmarkPage()
{
	if (m_window)
		m_window->bookmarkPage();
}

void BookmarksMenu::bookmarkAllTabs()
{
	if (m_window)
		BookmarksUtils::bookmarkAllTabsDialog(m_window, m_window->tabWidget());
}

void BookmarksMenu::showBookmarksManager()
{
	if (m_window)
		m_window->tabWidget()->openBookmarksDialog();
}

void BookmarksMenu::bookmarksChanged()
{
	m_changed = true;
}

void BookmarksMenu::aboutToShow()
{
	if (m_changed) {
		refresh();
		m_changed = false;
	}
}

void BookmarksMenu::menuAboutToShow()
{
	Q_ASSERT(qobject_cast<QMenu*>(sender()));
	QMenu* menu = static_cast<QMenu*>(sender());

	foreach(QAction *action, menu->actions()) {
		BookmarkItem* item{static_cast<BookmarkItem*>(action->data().value<void*>())};
		if (item && item->type() == BookmarkItem::Url && action->icon().isNull())
			action->setIcon(item->icon());
	}
}

void BookmarksMenu::bookmarkActivated()
{
	if (QAction* action = qobject_cast<QAction*>(sender())) {
		BookmarkItem* item{static_cast<BookmarkItem*>(action->data().value<void*>())};
		Q_ASSERT(item);
		openBookmark(item);
	}
}

void BookmarksMenu::openBookmark(BookmarkItem* item)
{
	Q_ASSERT(item->isUrl());

	if (m_window)
		BookmarksUtils::openBookmark(m_window, item);
}

void BookmarksMenu::refresh()
{
	while (actions().count() != 4) {
		QAction* action{actions()[4]};
		if (action->menu()) 
			action->menu()->clear();

		removeAction(action);
		delete action;
	}

	addActionToMenu(this, Application::instance()->bookmarks()->toolbarFolder());

	addSeparator();

	foreach(BookmarkItem* child, Application::instance()->bookmarks()->menuFolder()->children()) 
		addActionToMenu(this, child);

	addSeparator();

	addActionToMenu(this, Application::instance()->bookmarks()->unsortedFolder());
}

void BookmarksMenu::addActionToMenu(QMenu* menu, BookmarkItem* item)
{
	Q_ASSERT(menu);
	Q_ASSERT(item);

	switch (item->type()) {
	case BookmarkItem::Url:
		addUrlToMenu(menu, item);
		break;
	case BookmarkItem::Folder:
		addFolderToMenu(menu, item);
		break;
	case BookmarkItem::Separator:
		addSeparatorToMenu(menu, item);
		break;
	default:
		break;
	}
}

void BookmarksMenu::addUrlToMenu(QMenu* menu, BookmarkItem* bookmark)
{
	Q_ASSERT(menu);
	Q_ASSERT(bookmark);
	Q_ASSERT(bookmark->isUrl());

	QAction* action{new QAction(menu)};
	QString title{QFontMetrics(action->font()).elidedText(bookmark->title(), Qt::ElideRight, 250)};

	action->setText(title);
	action->setData(QVariant::fromValue<void*>(static_cast<void*>(bookmark)));
	action->setIconVisibleInMenu(true);

	connect(action, &QAction::triggered, this, &BookmarksMenu::bookmarkActivated);

	menu->addAction(action);
}

void BookmarksMenu::addFolderToMenu(QMenu* menu, BookmarkItem* folder)
{
	Q_ASSERT(menu);
	Q_ASSERT(folder);
	Q_ASSERT(folder->isFolder());

	QMenu* m{new QMenu(menu)};
	QString title{QFontMetrics(m->font()).elidedText(folder->title(), Qt::ElideRight, 250)};

	m->setTitle(title);
	m->setIcon(folder->icon());

	addFolderContentsToMenu(m, folder);

	QAction* action{menu->addMenu(m)};
	action->setData(QVariant::fromValue<void*>(static_cast<void*>(folder)));
	action->setIconVisibleInMenu(true);
}

void BookmarksMenu::addSeparatorToMenu(QMenu* menu, BookmarkItem* separator)
{
	Q_UNUSED(separator)
	Q_ASSERT(menu);
	Q_ASSERT(separator->isSeparator());

	menu->addSeparator();
}

void BookmarksMenu::addFolderContentsToMenu(QMenu* menu, BookmarkItem* folder)
{
	connect(menu, &QMenu::aboutToShow, this, &BookmarksMenu::menuAboutToShow);

	foreach(BookmarkItem* child, folder->children())
		addActionToMenu(menu, child);

	if (menu->isEmpty())
		menu->addAction(tr("Empty"))->setEnabled(false);
}
}
