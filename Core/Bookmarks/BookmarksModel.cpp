/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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

#include "Bookmarks/BookmarksModel.hpp"

#include <QApplication>
#include <QStyle>

#include <QByteArray>
#include <QBuffer>
#include <QIODevice>
#include <QDataStream>

#include "Bookmarks/BookmarkManager.hpp"
#include "Bookmarks/BookmarkNode.hpp"
#include "Bookmarks/XBelWriter.hpp"
#include "Bookmarks/XBelReader.hpp"

namespace Sn {

static const QString MIMETYPE = QLatin1String("application/bookmarks.xbel");

BookmarksModel::BookmarksModel(BookmarksManager* bookmarksManager, QObject* parent) :
	QAbstractItemModel(parent),
	m_endMacro(false),
	m_bookmarksManager(bookmarksManager)
{
	connect(m_bookmarksManager, &BookmarksManager::entryAdded, this, &BookmarksModel::entryAdded);
	connect(m_bookmarksManager, &BookmarksManager::entryRemoved, this, &BookmarksModel::entryRemoved);
	connect(m_bookmarksManager, &BookmarksManager::entryChanged, this, &BookmarksModel::entryChanged);
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

QVariant BookmarksModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.model() != this)
		return QVariant();

	const BookmarkNode* bookmarkNode{node(index)};

	switch (role) {
	case Qt::EditRole:
	case Qt::DisplayRole:
		if (bookmarkNode->type() == BookmarkNode::Separator) {
			switch (index.column()) {
			case 0:
				return QString(50, 0xB7);
			case 1:
				return QString();
			}
		}

		switch (index.column()) {
		case 0:
			return bookmarkNode->title;
		case 1:
			return bookmarkNode->url;
		}
		break;
	case BookmarksModel::UrlRole:
		return QUrl(bookmarkNode->url);
	case BookmarksModel::UrlStringRole:
		return bookmarkNode->url;
	case BookmarksModel::TypeRole:
		return bookmarkNode->type();
	case BookmarksModel::SeparatorRole:
		return (bookmarkNode->type() == BookmarkNode::Separator);
	case Qt::DecorationRole:
		if (index.column() == 0) {
			if (bookmarkNode->type() == BookmarkNode::Folder)
				return QApplication::style()->standardIcon(QStyle::SP_DirIcon);

			return QVariant(); //TODO: Manage favicons
		}
	}

	return QVariant();
}

int BookmarksModel::columnCount(const QModelIndex& parent) const
{
	return (parent.column() > 0) ? 0 : 2;
}

int BookmarksModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		return m_bookmarksManager->bookmarks()->children().count();

	const BookmarkNode* item = static_cast<BookmarkNode*>(parent.internalPointer());

	return item->children().count();
}

QModelIndex BookmarksModel::index(int row, int column, const QModelIndex& parent) const
{
	if (row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent))
		return QModelIndex();

	BookmarkNode* parentNode{node(parent)};

	return createIndex(row, column, parentNode->children()[row]);
}

QModelIndex BookmarksModel::index(BookmarkNode* node) const
{
	BookmarkNode* parent{node->parent()};

	if (!parent)
		return QModelIndex();

	return createIndex(parent->children().indexOf(node), 0, node);
}

QModelIndex BookmarksModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	BookmarkNode* itemNode{node(index)};
	BookmarkNode* parentNode{itemNode ? itemNode->parent() : nullptr};

	if (!parentNode || parentNode == m_bookmarksManager->bookmarks())
		return QModelIndex();

	BookmarkNode* grandParentNode{parentNode->parent()};
	int parentRow{grandParentNode->children().indexOf(parentNode)};

	Q_ASSERT(parentRow > 0);

	return createIndex(parentRow, 0, parentNode);
}

Qt::ItemFlags BookmarksModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	Qt::ItemFlags flags{Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled};
	BookmarkNode* bookmarkNode{node(index)};

	if (bookmarkNode->type() != BookmarkNode::Separator)
		flags |= Qt::ItemIsEditable;

	if (hasChildren(index))
		flags |= Qt::ItemIsDropEnabled;

	return flags;
}

Qt::DropActions BookmarksModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

bool BookmarksModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (row < 0 || count <= 0 || row + count > rowCount(parent))
		return false;

	BookmarkNode* bookmarkNode{node(parent)};

	for (BookmarkNode* node : bookmarkNode->children())
		m_bookmarksManager->removeBookmark(node);

	if (m_endMacro) {
		m_bookmarksManager->undoRedoStack()->endMacro();
		m_endMacro = false;
	}

	return true;
}

bool BookmarksModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid() || (flags(index) & Qt::ItemIsSelectable) == 0)
		return false;

	BookmarkNode* item{node(index)};

	switch (role) {
	case Qt::EditRole:
	case Qt::DisplayRole:
		if (index.column() == 0) {
			m_bookmarksManager->setTitle(item, value.toString());
			break;
		}

		if (index.column() == 1) {
			m_bookmarksManager->setUrl(item, value.toString());
			break;
		}

		return false;
	case BookmarksModel::UrlRole:
		m_bookmarksManager->setUrl(item, value.toUrl().toString());
		break;
	case BookmarksModel::UrlStringRole:
		m_bookmarksManager->setUrl(item, value.toString());
		break;
	default:
		break;
	}

	return true;
}

QMimeData* BookmarksModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData{new QMimeData()};
	QByteArray data{};
	QDataStream stream{&data, QIODevice::WriteOnly};

	for (QModelIndex index : indexes) {
		if (index.column() != 0 || !index.isValid())
			continue;

		QByteArray encodedData{};
		QBuffer buffer{&encodedData};
		XBelWriter writer{};
		const BookmarkNode* parentNode{node(index)};

		buffer.open(QIODevice::ReadWrite);
		writer.write(&buffer, parentNode);
		stream << encodedData;
	}

	mimeData->setData(MIMETYPE, data);
	return mimeData;
}

QStringList BookmarksModel::mimeTypes() const
{
	QStringList types;
	types << MIMETYPE;

	return types;
}

bool BookmarksModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
								  const QModelIndex& parent)
{
	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat(MIMETYPE) || column > 0)
		return false;

	QByteArray byteArray{data->data(MIMETYPE)};
	QDataStream stream(&byteArray, QIODevice::ReadOnly);

	if (stream.atEnd())
		return false;

	QUndoStack* undoStack{m_bookmarksManager->undoRedoStack()};
	undoStack->beginMacro(QLatin1String("Move Bookmarks"));

	while (!stream.atEnd()) {
		QByteArray encodedData{};
		stream >> encodedData;

		QBuffer buffer{&encodedData};
		buffer.open(QIODevice::ReadOnly);

		XBelReader reader{};
		BookmarkNode* rootNode{reader.read(&buffer)};

		for (BookmarkNode* bookmarkNode : rootNode->children()) {
			rootNode->remove(bookmarkNode);
			row = qMax(0, row);

			BookmarkNode* parentNode{node(parent)};

			m_bookmarksManager->addBookmark(parentNode, bookmarkNode, row);
			m_endMacro = true;
		}

		delete rootNode;
	}

	return true;
}

bool BookmarksModel::hasChildren(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return true;

	const BookmarkNode* parentNode{node(parent)};

	return (parentNode->type() == BookmarkNode::Folder);
}

BookmarkNode* BookmarksModel::node(const QModelIndex& index) const
{
	BookmarkNode* itemNode = static_cast<BookmarkNode*>(index.internalPointer());

	if (!itemNode)
		return m_bookmarksManager->bookmarks();

	return itemNode;
}

void BookmarksModel::entryAdded(BookmarkNode* item)
{
	Q_ASSERT(item && item->parent());

	int row{item->parent()->children().indexOf(item)};
	BookmarkNode* parent{item->parent()};

	parent->remove(item);
	beginInsertRows(index(parent), row, row);
	parent->add(item, row);
	endInsertRows();
}

void BookmarksModel::entryRemoved(BookmarkNode* parent, int row, BookmarkNode* item)
{
	parent->add(item, row);
	beginRemoveRows(index(parent), row, row);
	parent->remove(item);
	endRemoveRows();
}

void BookmarksModel::entryChanged(BookmarkNode* item)
{
	QModelIndex idx{index(item)};
	emit dataChanged(idx, idx);
}
}