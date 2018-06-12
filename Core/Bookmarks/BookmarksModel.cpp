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

#include "BookmarksModel.hpp"

#include <QMimeData>
#include <QDataStream>

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarkItem.hpp"

namespace Sn
{
BookmarksModel::BookmarksModel(BookmarkItem* root, Bookmarks* bookmarks, QObject* parent) :
	QAbstractItemModel(parent),
	m_root(root),
	m_bookmarks(bookmarks)
{
	if (m_bookmarks)
		connect(m_bookmarks, &Bookmarks::bookmarkChanged, this, &BookmarksModel::bookmarkChanged);
}

BookmarksModel::~BookmarksModel()
{
	// Empty
}

void BookmarksModel::addBookmark(BookmarkItem* parent, int row, BookmarkItem* item)
{
	Q_ASSERT(parent);
	Q_ASSERT(item);
	Q_ASSERT(row >= 0);
	Q_ASSERT(row <= parent->children().count());

	beginInsertRows(index(parent), row, row);
	parent->addChild(item, row);
	endInsertRows();
}

void BookmarksModel::removeBookmark(BookmarkItem* item)
{
	Q_ASSERT(item);
	Q_ASSERT(item->parent());

	int idx{item->parent()->children().indexOf(item)};

	beginRemoveRows(index(item->parent()), idx, idx);
	item->parent()->removeChild(item);
	endRemoveRows();
}

Qt::ItemFlags BookmarksModel::flags(const QModelIndex& index) const
{
	BookmarkItem* itm{item(index)};

	if (!index.isValid() || !itm)
		return Qt::NoItemFlags;

	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	if (itm->isFolder())
		flags |= Qt::ItemIsDropEnabled;
	if (m_bookmarks && m_bookmarks->canBeModified(itm))
		flags |= Qt::ItemIsDragEnabled;

	return flags;
}

QVariant BookmarksModel::data(const QModelIndex& index, int role) const
{
	BookmarkItem* itm{item(index)};

	if (!itm)
		return QVariant();

	switch (role) {
	case TypeRole:
		return itm->type();
	case UrlRole:
		return itm->url();
	case UrlStringRole:
		return itm->urlString();
	case TitleRole:
		return itm->title();
	case DescriptionRole:
		return itm->description();
	case KeywordRole:
		return itm->keyword();
	case VisitCountRole:
		return -1;
	case ExpandedRole:
		return itm->isExpanded();
	case Qt::ToolTipRole:
		if (index.column() == 0 && itm->isUrl())
			return QString("%1\n%2").arg(itm->title(), QString::fromUtf8(itm->url().toEncoded()));

		// fallthrough
	case Qt::DisplayRole:
		switch (index.column()) {
		case 0:
			return itm->title();
		case 1:
			return itm->url().toEncoded();
		default:
			return QVariant();
		}
	case Qt::DecorationRole:
		if (index.column() == 0)
			return itm->icon();
		return QVariant();
	default:
		return QVariant();
	}
}

QVariant BookmarksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
		case 0:
			return tr("Title");
		case 1:
			return tr("Address");
		}
	}

	return QAbstractItemModel::headerData(section, orientation, role);
}

int BookmarksModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0)
		return 0;

	BookmarkItem* itm{item(parent)};

	return itm->children().count();
}

int BookmarksModel::columnCount(const QModelIndex& parent) const
{
	if (parent.column() > 0)
		return 0;

	return 2;
}

bool BookmarksModel::hasChildren(const QModelIndex& parent) const
{
	BookmarkItem* itm{item(parent)};

	return !itm->children().isEmpty();
}

Qt::DropActions BookmarksModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

QStringList BookmarksModel::mimeTypes() const
{
	QStringList types{};

	types.append("sielo/bookmarks");

	return types;
}

QMimeData *BookmarksModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData{new QMimeData()};
	QByteArray encodedData{};
	QDataStream stream{&encodedData, QIODevice::WriteOnly};

	foreach(const QModelIndex& index, indexes) {
		if (index.isValid() && index.column() == 0 && !indexes.contains(index.parent()))
			stream << index.row() << reinterpret_cast<quintptr>(index.internalPointer());
	}

	mimeData->setData("sielo/bookmarks", encodedData);

	return mimeData;
}

bool BookmarksModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                                  const QModelIndex& parent)
{
	Q_UNUSED(column)

	if (action == Qt::IgnoreAction)
		return true;

	if (!m_bookmarks || !data->hasFormat("sielo/bookmarks") || !parent.isValid())
		return false;

	BookmarkItem* parentItm{item(parent)};
	Q_ASSERT(parentItm->isFolder());

	QByteArray encodedData{data->data("sielo/bookmarks")};
	QDataStream stream{&encodedData, QIODevice::ReadOnly};
	QList<BookmarkItem*> items{};

	while (!stream.atEnd()) {
		int row{};
		quintptr ptr{};

		stream >> row >> ptr;

		QModelIndex index{createIndex(row, 0, reinterpret_cast<void*>(ptr))};
		BookmarkItem* itm{item(index)};

		Q_ASSERT(index.isValid());
		Q_ASSERT(itm != m_bookmarks->rootItem());

		if (itm == parentItm)
			return false;

		items.append(itm);
	}

	row = qMax(row, 0);

	foreach(BookmarkItem* itm, items) {
		if (itm->parent() == parentItm && itm->parent()->children().indexOf(itm) < row)
			row--;

		m_bookmarks->removeBookmark(itm);
		m_bookmarks->insertBookmark(parentItm, row++, itm);
	}

	return true;
}

QModelIndex BookmarksModel::parent(const QModelIndex& child) const
{
	if (!child.isValid())
		return QModelIndex();

	BookmarkItem* itm{item(child)};

	return index(itm->parent());
}

QModelIndex BookmarksModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	BookmarkItem* parentItem{item(parent)};

	return createIndex(row, column, parentItem->children()[row]);
}

QModelIndex BookmarksModel::index(BookmarkItem* item, int column) const
{
	BookmarkItem* parent{item->parent()};

	if (!parent)
		return QModelIndex();

	return createIndex(parent->children().indexOf(item), column, item);
}

BookmarkItem *BookmarksModel::item(const QModelIndex& index) const
{
	BookmarkItem* itm{static_cast<BookmarkItem*>(index.internalPointer())};

	return itm ? itm : m_root;
}

void BookmarksModel::bookmarkChanged(BookmarkItem* item)
{
	QModelIndex idx{index(item)};

	emit dataChanged(idx, idx);
}
}
