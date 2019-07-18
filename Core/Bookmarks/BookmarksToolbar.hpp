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
#ifndef SIELOBROWSER_BOOKMARKSTOOLBAR_HPP
#define SIELOBROWSER_BOOKMARKSTOOLBAR_HPP

#include "SharedDefines.hpp"

#include <QToolBar>

#include <QHBoxLayout>

#include <QTimer>

namespace Sn
{
class BrowserWindow;

class Bookmarks;
class BookmarkItem;
class BookmarksToolbarButton;

class SIELO_SHAREDLIB BookmarksToolbar: public QToolBar {
	Q_OBJECT

public:
	BookmarksToolbar(BrowserWindow* window, QWidget* parent = nullptr);
	~BookmarksToolbar();

	void createContextMenu(QMenu& menu, const QPoint& pos);
	void contextMenuCreated();

	void loadSettings();

signals:
	void visibilityChanged(bool visible);

protected:
	void hideEvent(QHideEvent* event) override;
	void showEvent(QShowEvent* event) override;

private slots:
	void refresh();
	void bookmarksChanged();
	void showOnlyIconsChanged(bool state);
	void showOnlyTextChanged(bool state);

	void openBookmarkInNewTab();
	void openBookmarkInNewWindow();
	void openBookmarkInNewPrivateWindow();
	void editBookmark();
	void deleteBookmark();

	void hideToolBar();
	void toogleShow(bool showToolBar);

private:
	void addItem(BookmarkItem* item);
	BookmarksToolbarButton *buttonAt(const QPoint& pos);

	void dropEvent(QDropEvent* e);
	void dragEnterEvent(QDragEnterEvent* e);

	BrowserWindow* m_window{nullptr};
	Bookmarks* m_bookmarks{nullptr};
	BookmarkItem* m_clickedBookmark{nullptr};

	QTimer* m_updateTimer{nullptr};

	QList<BookmarksToolbarButton*> m_buttons{};
	QAction* m_actionShowOnlyIcons{nullptr};
	QAction* m_actionShowOnlyText{nullptr};
};
}

#endif //SIELOBROWSER_BOOKMARKSTOOLBAR_HPP
