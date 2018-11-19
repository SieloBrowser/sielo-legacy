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
#ifndef SIELOBROWSER_BOOKMARKSTREEVIEW_HPP
#define SIELOBROWSER_BOOKMARKSTREEVIEW_HPP

#include "SharedDefines.hpp"

#include <QTreeView>

namespace Sn
{
class Bookmarks;
class BookmarkItem;
class BookmarksModel;
class BookmarksFilterModel;

class SIELO_SHAREDLIB BookmarksTreeView: public QTreeView {
Q_OBJECT

public:
	BookmarksTreeView(QWidget* parent = nullptr);
	~BookmarksTreeView();

	BookmarkItem *selectedBookmark() const;
	QList<BookmarkItem*> selectedBookmarks() const;

	void selectBookmark(BookmarkItem* item);
	void ensureBookmarkVisible(BookmarkItem* item);

signals:
	void bookmarkActivated(BookmarkItem* item);
	void bookmarkCtrlActivated(BookmarkItem* item);
	void bookmarkShiftActivated(BookmarkItem* item);
	void contextMenuRequested(const QPoint& point);
	void bookmarksSelected(QList<BookmarkItem*> items);

public slots:
	void search(const QString& string);

private slots:
	void indexExpanded(const QModelIndex& parent);
	void indexCollapsed(const QModelIndex& parent);

private:
	void restoreExpandedState(const QModelIndex& parent);
	void rowsInserted(const QModelIndex& parent, int start, int end);
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

	void contextMenuEvent(QContextMenuEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);

	Bookmarks* m_bookmarks{nullptr};
	BookmarksModel* m_model{nullptr};
	BookmarksFilterModel* m_filter{nullptr};
};
}

#endif //SIELOBROWSER_BOOKMARKSTREEVIEW_HPP
