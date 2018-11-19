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
#ifndef SIELOBROWSER_BOOKMARKSMENU_HPP
#define SIELOBROWSER_BOOKMARKSMENU_HPP

#include "SharedDefines.hpp"

#include <QMenu>

#include <QPointer>

namespace Sn
{
class BrowserWindow;

class BookmarkItem;

class SIELO_SHAREDLIB BookmarksMenu: public QMenu {
Q_OBJECT

public:
	BookmarksMenu(QWidget* parent = nullptr);
	~BookmarksMenu();

	void setMainWindow(BrowserWindow* window);

private slots:
	void bookmarkPage();
	void bookmarkAllTabs();
	void showBookmarksManager();

	void bookmarksChanged();
	void aboutToShow();
	void menuAboutToShow();

	void bookmarkActivated();

	void openBookmark(BookmarkItem* item);

private:
	void refresh();

	void addActionToMenu(QMenu* menu, BookmarkItem* item);
	void addUrlToMenu(QMenu* menu, BookmarkItem* bookmark);
	void addFolderToMenu(QMenu* menu, BookmarkItem* folder);
	void addSeparatorToMenu(QMenu* menu, BookmarkItem* separator);
	void addFolderContentsToMenu(QMenu* menu, BookmarkItem* folder);

	QPointer<BrowserWindow> m_window{};
	bool m_changed{true};
};
}

#endif //SIELOBROWSER_BOOKMARKSMENU_HPP
