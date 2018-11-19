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
#ifndef SIELOBROWSER_BOOKMARKSMODEL_HPP
#define SIELOBROWSER_BOOKMARKSMODEL_HPP

#include "SharedDefines.hpp"

#include <QAbstractItemModel>

#include <QModelIndex>

namespace Sn
{
class Bookmarks;
class BookmarkItem;

class SIELO_SHAREDLIB BookmarksModel: public QAbstractItemModel {
Q_OBJECT

public:
	enum Roles {
		TypeRole = Qt::UserRole + 1,
		UrlRole = Qt::UserRole + 2,
		UrlStringRole = Qt::UserRole + 3,
		TitleRole = Qt::UserRole + 4,
		IconRole = Qt::UserRole + 5,
		DescriptionRole = Qt::UserRole + 6,
		KeywordRole = Qt::UserRole + 7,
		VisitCountRole = Qt::UserRole + 8,
		ExpandedRole = Qt::UserRole + 9
	};

	BookmarksModel(BookmarkItem* root, Bookmarks* bookmarks, QObject* parent = nullptr);
	~BookmarksModel();

	void addBookmark(BookmarkItem* parent, int row, BookmarkItem* item);
	void removeBookmark(BookmarkItem* item);

	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	int rowCount(const QModelIndex& parent) const;
	int columnCount(const QModelIndex& parent) const;
	bool hasChildren(const QModelIndex& parent) const;

	Qt::DropActions supportedDropActions() const;
	QStringList mimeTypes() const;
	QMimeData *mimeData(const QModelIndexList& indexes) const;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

	QModelIndex parent(const QModelIndex& child) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex index(BookmarkItem* item, int column = 0) const;

	BookmarkItem *item(const QModelIndex& index) const;

private slots:
	void bookmarkChanged(BookmarkItem* item);

private:
	BookmarkItem* m_root{nullptr};
	Bookmarks* m_bookmarks{nullptr};

};
}

#endif //SIELOBROWSER_BOOKMARKSMODEL_HPP
