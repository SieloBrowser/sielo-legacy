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

#include "History/HistoryFilterModel.hpp"

#include "History/HistoryModel.hpp"

namespace Sn {

HistoryFilterModel::HistoryFilterModel(QAbstractItemModel* sourceModel, QObject* parent) :
	QAbstractProxyModel(parent),
	m_loaded(false)
{
	setSourceModel(sourceModel);
}

QModelIndex HistoryFilterModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	QString url{sourceIndex.data(HistoryModel::UrlStringRole).toString()};

	if (!m_historyHash.contains(url))
		return QModelIndex();

	int realRow{-1};
	int sourceModelRow{sourceModel()->rowCount() - sourceIndex.row()};

	for (int i{0}; i < m_sourceRow.count(); ++i) {
		if (m_sourceRow[i] == sourceModelRow) {
			realRow = i;
			break;
		}
	}

	if (realRow == -1)
		return QModelIndex();

	return createIndex(realRow, sourceIndex.column(), sourceModel()->rowCount() - sourceIndex.row());
}

QModelIndex HistoryFilterModel::mapToSource(const QModelIndex& proxyIndex) const
{
	int sourceRow{static_cast<int>(sourceModel()->rowCount() - proxyIndex.internalId())};

	return sourceModel()->index(sourceRow, proxyIndex.column());
}

void HistoryFilterModel::setSourceModel(QAbstractItemModel* newSourceModel)
{
	beginResetModel();

	if (sourceModel()) {
		disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &HistoryFilterModel::sourceReset);
		disconnect(sourceModel(), &QAbstractItemModel::dataChanged, this, &HistoryFilterModel::sourceDataChanged);
		disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &HistoryFilterModel::sourceRowsInserted);
		disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryFilterModel::sourceRowsRemoved);
	}

	QAbstractProxyModel::setSourceModel(newSourceModel);

	if (sourceModel()) {
		connect(sourceModel(), &QAbstractItemModel::modelReset, this, &HistoryFilterModel::sourceReset);
		connect(sourceModel(), &QAbstractItemModel::dataChanged, this, &HistoryFilterModel::sourceDataChanged);
		connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &HistoryFilterModel::sourceRowsInserted);
		connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryFilterModel::sourceRowsRemoved);
	}

	load();
	endResetModel();
}

QVariant HistoryFilterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return sourceModel()->headerData(section, orientation, role);
}

int HistoryFilterModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_historyHash.count();
}

int HistoryFilterModel::columnCount(const QModelIndex& parent) const
{
	return (parent.isValid()) ? 0 : 2;
}

QModelIndex HistoryFilterModel::index(int row, int column, const QModelIndex& parent) const
{
	if (row < 0 || row >= rowCount(parent) || column < 0 || column >= columnCount(parent))
		return QModelIndex();

	return createIndex(row, column, m_sourceRow[row]);
}

QModelIndex HistoryFilterModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return QModelIndex();
}

bool HistoryFilterModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (row < 0 || count <= 0 || row + count > rowCount(parent) || parent.isValid())
		return false;

	int lastRow{row + count - 1};
	if (sourceModel())
		disconnect(sourceModel(), &QAbstractProxyModel::rowsRemoved, this, &HistoryFilterModel::sourceRowsRemoved);

	beginRemoveRows(parent, row, lastRow);

	int oldCount{rowCount()};
	int start{sourceModel()->rowCount() - m_sourceRow.value(row)};
	int end{sourceModel()->rowCount() - m_sourceRow.value(lastRow)};

	sourceModel()->removeRows(start, end - start + 1);

	endRemoveRows();

	connect(sourceModel(), &QAbstractProxyModel::rowsRemoved, this, &HistoryFilterModel::sourceRowsRemoved);

	m_loaded = false;

	if (oldCount - 1 != rowCount()) {
		beginResetModel();
		endResetModel();
	}

	return true;
}

void HistoryFilterModel::sourceReset()
{
	beginResetModel();
	load();
	endResetModel();
}

void HistoryFilterModel::sourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
	emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}

void HistoryFilterModel::sourceRowsInserted(const QModelIndex& parent, int start, int end)
{
	Q_ASSERT(start == end && start == 0);
	Q_UNUSED(end);

	QModelIndex idx{sourceModel()->index(start, 0, parent)};
	QString url{idx.data(HistoryModel::UrlStringRole).toString()};

	if (m_historyHash.contains(url)) {
		int sourceRow{sourceModel()->rowCount() - m_historyHash[url]};
		int realRow{mapFromSource(sourceModel()->index(sourceRow, 0)).row()};

		beginRemoveRows(QModelIndex(), realRow, realRow);

		m_sourceRow.removeAt(realRow);
		m_historyHash.remove(url);

		endRemoveRows();
	}

	beginInsertRows(QModelIndex(), 0, 0);

	m_historyHash.insert(url, sourceModel()->rowCount() - start);
	m_sourceRow.insert(0, sourceModel()->rowCount());

	endInsertRows();
}

void HistoryFilterModel::sourceRowsRemoved(const QModelIndex& index, int start, int end)
{
	Q_UNUSED(index);
	Q_UNUSED(start);
	Q_UNUSED(end);
	sourceReset();
}

void HistoryFilterModel::load() const
{
	m_sourceRow.clear();
	m_historyHash.clear();
	m_historyHash.reserve(sourceModel()->rowCount());

	for (int i{0}; i < sourceModel()->rowCount(); ++i) {
		QModelIndex idx{sourceModel()->index(i, 0)};
		QString url{idx.data(HistoryModel::UrlStringRole).toString()};

		if (!m_historyHash.contains(url)) {
			m_sourceRow.append(sourceModel()->rowCount() - i);
			m_historyHash[url] = sourceModel()->rowCount() - i;
		}
	}
}

}