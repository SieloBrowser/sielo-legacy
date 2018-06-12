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

#include "BookmarksTreeView.hpp"

#include <QHeaderView>

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarkItem.hpp"
#include "Bookmarks/BookmarksModel.hpp"
#include "Bookmarks/BookmarksFilterModel.hpp"
#include "Bookmarks/BookmarksItemDelegate.hpp"

#include "Application.hpp"

namespace Sn
{
	BookmarksTreeView::BookmarksTreeView(QWidget* parent) :
		QTreeView(parent),
		m_bookmarks(Application::instance()->bookmarks()),
		m_model(m_bookmarks->model()),
		m_filter(new BookmarksFilterModel(m_model))
	{
		setModel(m_filter);
		setDragEnabled(true);
		setAcceptDrops(true);
		setUniformRowHeights(true);
		setDropIndicatorShown(true);
		setAllColumnsShowFocus(true);
		setColumnHidden(1, false);
		setHeaderHidden(false);
		setMouseTracking(false);
		setSelectionMode(QAbstractItemView::ExtendedSelection);
		setItemDelegate(new BookmarksItemDelegate(this));

		header()->resizeSections(QHeaderView::ResizeToContents);

		connect(this, &QTreeView::expanded, this, &BookmarksTreeView::indexExpanded);
		connect(this, &QTreeView::collapsed, this, &BookmarksTreeView::indexCollapsed);

		restoreExpandedState(QModelIndex());
	}

	BookmarksTreeView::~BookmarksTreeView()
	{
		// Empty
	}

	BookmarkItem *BookmarksTreeView::selectedBookmark() const
	{
		QList<BookmarkItem*> items = selectedBookmarks();

		return items.count() == 1 ? items[0] : nullptr;
	}

	QList<BookmarkItem*> BookmarksTreeView::selectedBookmarks() const
	{
		QList<BookmarkItem*> items;

		foreach(const QModelIndex &index, selectionModel()->selectedRows()) {
			BookmarkItem* item{m_model->item(m_filter->mapToSource(index))};
			items.append(item);
		}

		return items;
	}

	void BookmarksTreeView::selectBookmark(BookmarkItem* item)
	{
		QModelIndex col0{m_filter->mapFromSource(m_model->index(item, 0))};
		QModelIndex col1{m_filter->mapFromSource(m_model->index(item, 1))};

		selectionModel()->clearSelection();
		selectionModel()->select(col0, QItemSelectionModel::Select);
		selectionModel()->select(col1, QItemSelectionModel::Select);
	}

	void BookmarksTreeView::ensureBookmarkVisible(BookmarkItem* item)
	{
		QModelIndex index{m_filter->mapFromSource(m_model->index(item))};
		QModelIndex parent{m_filter->parent(index)};

		while (parent.isValid()) {
			setExpanded(parent, true);
			parent = m_filter->parent(parent);
		}
	}

	void BookmarksTreeView::search(const QString& string)
	{
		m_filter->setFilterFixedString(string);
	}

	void BookmarksTreeView::indexExpanded(const QModelIndex& parent)
	{
		BookmarkItem* item{m_model->item(m_filter->mapToSource(parent))};
		item->setExpanded(true);
	}

	void BookmarksTreeView::indexCollapsed(const QModelIndex& parent)
	{
		BookmarkItem* item{m_model->item(m_filter->mapToSource(parent))};
		item->setExpanded(false);
	}

	void BookmarksTreeView::restoreExpandedState(const QModelIndex& parent)
	{
		for (int i{0}; i < m_filter->rowCount(parent); ++i) {
			QModelIndex index{m_filter->index(i, 0, parent)};
			BookmarkItem* item{m_model->item(m_filter->mapToSource(index))};
			setExpanded(index, item->isExpanded());
			restoreExpandedState(index);
		}
	}

	void BookmarksTreeView::rowsInserted(const QModelIndex& parent, int start, int end)
	{
		restoreExpandedState(parent);

		QTreeView::rowsInserted(parent, start, end);
	}

	void BookmarksTreeView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
	{
		Q_UNUSED(selected)
		Q_UNUSED(deselected)

		emit bookmarksSelected(selectedBookmarks());
	}

	void BookmarksTreeView::contextMenuEvent(QContextMenuEvent* event)
	{
		emit contextMenuRequested(viewport()->mapToGlobal(event->pos()));
	}

	void BookmarksTreeView::mousePressEvent(QMouseEvent* event)
	{
		QTreeView::mousePressEvent(event);

		if (selectionModel()->selectedRows().count() == 1) {
			QModelIndex index{indexAt(event->pos())};
			Qt::MouseButtons buttons = event->buttons();
			Qt::KeyboardModifiers modifiers = event->modifiers();

			if (index.isValid()) {
				BookmarkItem* item{m_model->item(m_filter->mapToSource(index))};

				if (buttons == Qt::LeftButton && modifiers == Qt::ShiftModifier)
				emit bookmarkShiftActivated(item);
				else if (buttons == Qt::MiddleButton || (buttons == Qt::LeftButton && modifiers == Qt::ControlModifier))
				emit bookmarkCtrlActivated(item);
			}
		}
	}

	void BookmarksTreeView::mouseDoubleClickEvent(QMouseEvent* event)
	{
		QTreeView::mouseDoubleClickEvent(event);

		if (selectionModel()->selectedRows().count() == 1) {
			QModelIndex index{indexAt(event->pos())};

			if (index.isValid()) {
				BookmarkItem* item{m_model->item(m_filter->mapToSource(index))};
				Qt::MouseButtons buttons = event->buttons();
				Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();

				if (buttons == Qt::LeftButton && modifiers == Qt::NoModifier)
				emit bookmarkActivated(item);
				else if (buttons == Qt::LeftButton && modifiers == Qt::ShiftModifier)
				emit bookmarkShiftActivated(item);
			}
		}
	}

	void BookmarksTreeView::keyPressEvent(QKeyEvent* event)
	{
		QTreeView::keyPressEvent(event);

		if (selectionModel()->selectedRows().count() == 1) {
			QModelIndex index{selectionModel()->selectedRows()[0]};
			BookmarkItem* item{m_model->item(m_filter->mapToSource(index))};

			switch (event->key()) {
			case Qt::Key_Return:
			case Qt::Key_Enter:
				if (item->isFolder() && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::
					KeypadModifier))
					setExpanded(index, !isExpanded(index));
				else {
					Qt::KeyboardModifiers modifiers = event->modifiers();

					if (modifiers == Qt::NoModifier || modifiers == Qt::KeypadModifier)
					emit bookmarkActivated(item);
					else if (modifiers == Qt::ControlModifier)
					emit bookmarkCtrlActivated(item);
					else if (modifiers == Qt::ShiftModifier)
					emit bookmarkShiftActivated(item);
				}
				break;
			}
		}
	}
}
