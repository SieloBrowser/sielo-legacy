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
#ifndef SIELOBROWSER_HistoryModel_HPP
#define SIELOBROWSER_HistoryModel_HPP

#include "SharedDefines.hpp"

#include <QAbstractItemModel>
#include <QModelIndex>

#include <QVariant>

#include "History/History.hpp"

namespace Sn
{
class HistoryItem;

class SIELO_SHAREDLIB HistoryModel: public QAbstractItemModel {
Q_OBJECT

public:
	enum Roles {
		IdRole = Qt::UserRole + 1,
		TitleRole = Qt::UserRole + 2,
		UrlRole = Qt::UserRole + 3,
		UrlStringRole = Qt::UserRole + 4,
		IconRole = Qt::UserRole + 5,
		IsTopLevelRole = Qt::UserRole + 7,
		TimestampStartRole = Qt::UserRole + 8,
		TimestampEndRole = Qt::UserRole + 9,
		MaxRole = TimestampEndRole
	};

	HistoryModel(History* history);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex& index, int role) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role);

	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& child) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	bool canFetchMore(const QModelIndex& parent) const;
	void fetchMore(const QModelIndex& parent);

	bool hasChildren(const QModelIndex& parent) const;

	HistoryItem *itemFromIndex(const QModelIndex& index) const;

	void removeTopLevelIndexes(const QList<QPersistentModelIndex>& indexes);

private slots:
	void resetHistory();

	void historyEntryAdded(const History::HistoryEntry& entry);
	void historyEntryDeleted(const History::HistoryEntry& entry);
	void historyEntryEdited(const History::HistoryEntry& before, const History::HistoryEntry& after);

private:
	HistoryItem *findHistoryItem(const History::HistoryEntry& entry);
	void checkEmptyParentItem(HistoryItem* item);
	void init();

	HistoryItem* m_rootItem{nullptr};
	HistoryItem* m_todayItem{nullptr};
	History* m_history{nullptr};
};
}

#endif //SIELOBROWSER_HistoryModel_HPP
