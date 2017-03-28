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

#pragma once
#ifndef SIELOBROWSER_BOOKMARKSMODEL_HPP
#define SIELOBROWSER_BOOKMARKSMODEL_HPP

#include <QAbstractItemModel>
#include <QModelIndex>

#include <QMimeData>

#include <QVariant>

namespace Sn {
class BookmarksManager;
class BookmarkNode;

class BookmarksModel: public QAbstractItemModel {
Q_OBJECT

public:
	enum Roles {
		TypeRole = Qt::UserRole + 1,
		UrlRole = Qt::UserRole + 2,
		UrlStringRole = Qt::UserRole + 3,
		SeparatorRole = Qt::UserRole + 4
	};

	BookmarksModel(BookmarksManager* bookmarksManager, QObject* parent = nullptr);

	inline BookmarksManager* bookmarksManager() const { return m_bookmarksManager; }

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex index(BookmarkNode* node) const;
	QModelIndex parent(const QModelIndex& index = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	Qt::DropActions supportedDropActions() const;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	QMimeData* mimeData(const QModelIndexList& indexes) const;
	QStringList mimeTypes() const;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

	BookmarkNode* node(const QModelIndex& index) const;

public slots:
	void entryAdded(BookmarkNode* item);
	void entryRemoved(BookmarkNode* parent, int row, BookmarkNode* item);
	void entryChanged(BookmarkNode* item);

private:
	bool m_endMacro{false};

	BookmarksManager* m_bookmarksManager{nullptr};
};

}
#endif //SIELOBROWSER_BOOKMARKSMODEL_HPP
