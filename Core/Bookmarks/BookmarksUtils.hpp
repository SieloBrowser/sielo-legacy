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

#pragma once
#ifndef SIELOBROWSER_BOOKMARKSTOOLS_HPP
#define SIELOBROWSER_BOOKMARKSTOOLS_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QMenu>

namespace Sn
{
class BrowserWindow;
class TabWidget;

class BookmarkItem;

class SIELO_SHAREDLIB BookmarksUtils {
public:
	// Add Bookmark Dialogs
	static bool addBookmarkDialog(QWidget* parent, const QUrl& url, const QString& title, BookmarkItem* folder = 0);
	static bool bookmarkAllTabsDialog(QWidget* parent, TabWidget* tabWidget, BookmarkItem* folder = 0);

	// Edit Bookmark Dialog
	static bool editBookmarkDialog(QWidget* parent, BookmarkItem* item);

	// Open Bookmarks
	static void openBookmark(BrowserWindow* window, BookmarkItem* item);
	static void openBookmarkInNewTab(BrowserWindow* window, BookmarkItem* item);
	static void openBookmarkInNewWindow(BookmarkItem* item);
	static void openBookmarkInNewPrivateWindow(BookmarkItem* item);
	static void openFolderInTabs(BrowserWindow* window, BookmarkItem* folder);
};
}

#endif //SIELOBROWSER_BOOKMARKSTOOLS_HPP
