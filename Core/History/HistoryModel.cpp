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

#include "History/HistoryModel.hpp"

#include <QList>

#include <QFileInfo>

#include "History/HistoryItem.hpp"
#include "History/HistoryManager.hpp"

namespace Sn {

HistoryModel::HistoryModel(HistoryManager* history, QObject* parent) :
	QAbstractTableModel(parent),
	m_history(history)
{
	Q_ASSERT(history);

	connect(m_history, &HistoryManager::historyReset, this, &HistoryModel::historyReset);
	connect(m_history, &HistoryManager::entryAdded, this, &HistoryModel::entryAdded);
	connect(m_history, &HistoryManager::entryRemoved, this, &HistoryModel::entryRemoved);
	connect(m_history, &HistoryManager::entryUpdate, this, &HistoryModel::entryUpdated);
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
		case 0:
			return tr("Title");
		case 1:
			return tr("Address");
		}
	}

	return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant HistoryModel::data(const QModelIndex& index, int role) const
{
	QList<HistoryItem> list = m_history->history();

	if (index.row() < 0 || index.row() >= list.size())
		return QVariant();

	const HistoryItem& item = list[index.row()];

	switch (role) {
	case DateTimeRole:
		return item.dateTime;
	case DateRole:
		return item.dateTime.date();
	case UrlRole:
		return QUrl(item.url);
	case UrlStringRole:
		return item.url;
	case Qt::DisplayRole:
	case Qt::EditRole: {
		switch (index.column()) {
		case 0:
			if (item.title.isEmpty()) {
				QString page{QFileInfo(QUrl(item.url).path()).fileName()};

				if (!page.isEmpty())
					return page;
				return item.url;
			}
			return item.title;
		case 1:
			return item.url;
		}
	}
	case Qt::DecorationRole:
		if (index.column() == 0) {
			return QVariant(); //TODO: icon managment
		}
	}

	return QVariant();
}

int HistoryModel::columnCount(const QModelIndex& parent) const
{
	return (parent.isValid() ? 0 : 2);
}

int HistoryModel::rowCount(const QModelIndex& parent) const
{
	return (parent.isValid() ? 0 : m_history->history().count());
}

bool HistoryModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (parent.isValid())
		return false;

	int lastRow{row + count - 1};
	beginRemoveRows(parent, row, lastRow);
	QList<HistoryItem>& list = m_history->history();

	for (int i{lastRow}; i >= row; --i)
		list.removeAt(i);

	endRemoveRows();

	return true;
}

void HistoryModel::historyReset()
{
	beginResetModel();
	m_history->history().clear();
	endResetModel();
}

void HistoryModel::entryAdded(const HistoryItem& item)
{
	beginInsertRows(QModelIndex(), 0, 0);
	m_history->history().prepend(item);
	endInsertRows();
}

void HistoryModel::entryRemoved(const HistoryItem& item)
{
	int index{m_history->history().indexOf(item)};
	Q_ASSERT(index > -1);
	beginRemoveRows(QModelIndex(), index, index);
	m_history->history().takeAt(index);
	endRemoveRows();
}

void HistoryModel::entryUpdated(int offset)
{
	QModelIndex idx = index(offset, 0);
	emit dataChanged(idx, idx);
}
}