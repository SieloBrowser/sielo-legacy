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

#include "BookmarksToolbar.hpp"

#include <QMimeData>

#include "Utils/Settings.hpp"

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarksUtils.hpp"
#include "Bookmarks/BookmarkItem.hpp"
#include "Bookmarks/BookmarksToolbarButton.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
BookmarksToolbar::BookmarksToolbar(BrowserWindow* window, QWidget* parent) :
	QToolBar(parent),
	m_window(window),
	m_bookmarks(Application::instance()->bookmarks())
{
	setObjectName(QLatin1String("bookmarks-toolbar"));
	setAcceptDrops(true);
	setMinimumHeight(25);

	loadSettings();

	m_updateTimer = new QTimer(this);
	m_updateTimer->setInterval(300);
	m_updateTimer->setSingleShot(true);

	connect(m_updateTimer, &QTimer::timeout, this, &BookmarksToolbar::refresh);

	connect(m_bookmarks, SIGNAL(bookmarkAdded(BookmarkItem*)), this, SLOT(bookmarksChanged()));
	connect(m_bookmarks, SIGNAL(bookmarkRemoved(BookmarkItem*)), this, SLOT(bookmarksChanged()));
	connect(m_bookmarks, SIGNAL(bookmarkChanged(BookmarkItem*)), this, SLOT(bookmarksChanged()));
	connect(m_bookmarks, &Bookmarks::showOnlyIconsInToolbarChanged, this, &BookmarksToolbar::showOnlyIconsChanged);
	connect(m_bookmarks, &Bookmarks::showOnlyTextInToolbarChanged, this, &BookmarksToolbar::showOnlyTextChanged);

	refresh();
}

BookmarksToolbar::~BookmarksToolbar()
{
	// Empty
}

void BookmarksToolbar::createContextMenu(QMenu& menu, const QPoint& pos)
{
	BookmarksToolbarButton* button = buttonAt(pos);
	m_clickedBookmark = button ? button->bookmark() : nullptr;

	QAction* actionHide{menu.addAction(tr("Hide toolbar"))};

	menu.addSeparator();

	QAction* actionNewTab{menu.addAction(tr("Open in new tab"))};
	QAction* actionNewWindow{menu.addAction(Application::getAppIcon("new-window"), tr("Open in new window"))};
	QAction* actionNewPrivateWindow{menu.addAction(tr("Open in new private window"))};

	menu.addSeparator();

	QAction* actionEdit{menu.addAction(tr("Edit"))};
	QAction* actionDelete{menu.addAction(tr("Delete"))};

	menu.addSeparator();

	m_actionShowOnlyIcons = menu.addAction(tr("Show Only Icons"));
	m_actionShowOnlyIcons->setCheckable(true);
	m_actionShowOnlyIcons->setChecked(m_bookmarks->showOnlyIconsInToolbar());

	connect(m_actionShowOnlyIcons, &QAction::toggled, m_bookmarks, &Bookmarks::setShowOnlyIconsInToolbar);

	m_actionShowOnlyText = menu.addAction(tr("Show Only Text"));
	m_actionShowOnlyText->setCheckable(true);
	m_actionShowOnlyText->setChecked(m_bookmarks->showOnlyTextInToolbar());

	connect(m_actionShowOnlyText, &QAction::toggled, m_bookmarks, &Bookmarks::setShowOnlyTextInToolbar);

	connect(actionHide, &QAction::triggered, this, &BookmarksToolbar::hideToolBar);

	connect(actionNewTab, &QAction::triggered, this, &BookmarksToolbar::openBookmarkInNewTab);
	connect(actionNewWindow, &QAction::triggered, this, &BookmarksToolbar::openBookmarkInNewWindow);
	connect(actionNewPrivateWindow, &QAction::triggered, this, &BookmarksToolbar::openBookmarkInNewPrivateWindow);
	connect(actionEdit, &QAction::triggered, this, &BookmarksToolbar::editBookmark);
	connect(actionDelete, &QAction::triggered, this, &BookmarksToolbar::deleteBookmark);

	actionNewTab->setEnabled(m_clickedBookmark && m_clickedBookmark->isUrl());
	actionNewWindow->setEnabled(m_clickedBookmark && m_clickedBookmark->isUrl());
	actionNewPrivateWindow->setEnabled(m_clickedBookmark && m_clickedBookmark->isUrl());
	actionEdit->setEnabled(m_clickedBookmark && m_bookmarks->canBeModified(m_clickedBookmark));
	actionDelete->setEnabled(m_clickedBookmark && m_bookmarks->canBeModified(m_clickedBookmark));
}

void BookmarksToolbar::contextMenuCreated()
{
	m_clickedBookmark = nullptr;
	m_actionShowOnlyIcons = nullptr;
	m_actionShowOnlyText = nullptr;
}

void BookmarksToolbar::loadSettings()
{
	Settings settings{};
	bool showToolBar{settings.value("Settings/ShowBookmarksToolBar", true).toBool()};

	if (showToolBar)
		show();
	else
		hide();
}

void BookmarksToolbar::hideEvent(QHideEvent* event)
{
	Q_UNUSED(event);

	emit visibilityChanged(false);
	QToolBar::hideEvent(event);
}

void BookmarksToolbar::showEvent(QShowEvent* event)
{
	Q_UNUSED(event);

	emit visibilityChanged(true);
	QToolBar::showEvent(event);
}

void BookmarksToolbar::refresh()
{
	clear();
	m_buttons.clear();

	BookmarkItem* folder{Application::instance()->bookmarks()->toolbarFolder()};

	foreach(BookmarkItem* child, folder->children())
		addItem(child);
}

void BookmarksToolbar::bookmarksChanged()
{
	m_updateTimer->start();
}

void BookmarksToolbar::showOnlyIconsChanged(bool state)
{
	if (state && m_actionShowOnlyText)
		m_actionShowOnlyText->setChecked(false);

	foreach(BookmarksToolbarButton* bookmark, m_buttons)
		bookmark->setShowOnlyIcon(state);
}

void BookmarksToolbar::showOnlyTextChanged(bool state)
{
	if (state && m_actionShowOnlyIcons)
		m_actionShowOnlyIcons->setChecked(false);

	foreach(BookmarksToolbarButton* bookmark, m_buttons)
		bookmark->setShowOnlyText(state);
}


void BookmarksToolbar::openBookmarkInNewTab()
{
	if (m_clickedBookmark)
		BookmarksUtils::openBookmarkInNewTab(m_window, m_clickedBookmark);
}

void BookmarksToolbar::openBookmarkInNewWindow()
{
	if (m_clickedBookmark)
		BookmarksUtils::openBookmarkInNewWindow(m_clickedBookmark);
}

void BookmarksToolbar::openBookmarkInNewPrivateWindow()
{
	if (m_clickedBookmark)
		BookmarksUtils::openBookmarkInNewPrivateWindow(m_clickedBookmark);
}

void BookmarksToolbar::editBookmark()
{
	if (m_clickedBookmark)
		BookmarksUtils::editBookmarkDialog(this, m_clickedBookmark);
	m_bookmarks->changeBookmark(m_clickedBookmark);
}

void BookmarksToolbar::deleteBookmark()
{
	if (m_clickedBookmark)
		m_bookmarks->removeBookmark(m_clickedBookmark);
}

void BookmarksToolbar::hideToolBar()
{
	Settings settings{};
	settings.setValue("Settings/ShowBookmarksToolBar", false);

	hide();
}

void BookmarksToolbar::toogleShow(bool showToolBar)
{
	Settings settings{};
	settings.setValue("Settings/ShowBookmarksToolBar", showToolBar);

	if (showToolBar)
		show();
	else
		hide();
}

void BookmarksToolbar::addItem(BookmarkItem* item)
{
	Q_ASSERT(item);

	if (item->isSeparator())
		addSeparator();
	else {
		BookmarksToolbarButton* button{new BookmarksToolbarButton(item, this)};

		button->setMainWindow(m_window);
		button->setShowOnlyIcon(m_bookmarks->showOnlyIconsInToolbar());
		button->setShowOnlyText(m_bookmarks->showOnlyTextInToolbar());

		addWidget(button);
		m_buttons.append(button);
	}
}

BookmarksToolbarButton *BookmarksToolbar::buttonAt(const QPoint& pos)
{
	return qobject_cast<BookmarksToolbarButton*>(QApplication::widgetAt(mapToGlobal(pos)));
}

void BookmarksToolbar::dropEvent(QDropEvent* e)
{
	const QMimeData* mime{e->mimeData()};

	if (!mime->hasUrls()) {
		QToolBar::dropEvent(e);
		return;
	}

	QUrl url{mime->urls()[0]};
	QString title{mime->hasText() ? mime->text() : url.toEncoded(QUrl::RemoveScheme)};

	BookmarkItem* parent{m_bookmarks->toolbarFolder()};
	BookmarksToolbarButton* button{buttonAt(e->pos())};

	if (button && button->bookmark()->isFolder())
		parent = button->bookmark();

	BookmarkItem* bookmark{new BookmarkItem(BookmarkItem::Url)};
	bookmark->setTitle(title);
	bookmark->setUrl(url);

	m_bookmarks->addBookmark(parent, bookmark);
}

void BookmarksToolbar::dragEnterEvent(QDragEnterEvent* e)
{
	const QMimeData* mime{e->mimeData()};

	if (mime->hasUrls() && mime->hasText()) {
		e->acceptProposedAction();
		return;
	}

	QToolBar::dragEnterEvent(e);
}
}
