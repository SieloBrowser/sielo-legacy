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

#include "History/HistoryTreeModel.hpp"

#include <QDate>

#include <QIcon>

#include "History/HistoryModel.hpp"

namespace Sn {
HistoryTreeModel::HistoryTreeModel(QAbstractItemModel* sourceModel, QObject* parent) :
	QAbstractProxyModel(parent)
{
	setSourceModel(sourceModel);
}

QVariant HistoryTreeModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::EditRole || role == Qt::DisplayRole) {
		int start{static_cast<int>(index.internalId())};

		if (start == 0) {
			int offset{sourceDateRow(index.row())};

			if (index.column() == 0) {
				QModelIndex idx{sourceModel()->index(offset, 0)};
				QDate date{idx.data(HistoryModel::DateRole).toDate()};

				if (date == QDate::currentDate())
					return tr("Earlier Today");
				return date.toString(QLatin1String("dddd, MMMM d, yyyy"));
			}
			if (index.column() == 1)
				return tr("%1 items").arg(rowCount(index.sibling(index.row(), 0)));
		}
	}

	if (role == Qt::DecorationRole && index.column() == 0 && !index.parent().isValid())
		return QIcon(QLatin1String("")); //TODO: manage icons

	if (role == HistoryModel::DateRole && index.column() == 0 && index.internalId() == 0) {
		int offset{sourceDateRow(index.row())};
		QModelIndex idx{sourceModel()->index(offset, 0)};

		return idx.data(HistoryModel::DateRole);
	}

	return QAbstractProxyModel::data(index, role);
}

int HistoryTreeModel::columnCount(const QModelIndex& parent) const
{
	return sourceModel()->columnCount(mapToSource(parent));
}

int HistoryTreeModel::rowCount(const QModelIndex& parent) const
{
	if (parent.internalId() != 0 || parent.column() > 0 || !sourceModel())
		return 0;

	if (!parent.isValid()) {
		if (!m_sourceRowCache.isEmpty())
			return m_sourceRowCache.count();

		QDate currentDate{};
		int rows{0};
		int totalRows{sourceModel()->rowCount()};

		for (int i{0}; i < totalRows; ++i) {
			QDate rowDate{sourceModel()->index(i, 0).data(HistoryModel::DateRole).toDate()};

			if (rowDate != currentDate) {
				m_sourceRowCache.append(i);
				currentDate = rowDate;
				++rows;
			}
		}

		Q_ASSERT(m_sourceRowCache.count() == rows);

		return rows;
	}

	int start{sourceDateRow(parent.row())};
	int end{sourceDateRow(parent.row() + 1)};
	return (end - start);
}

QModelIndex HistoryTreeModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();

	if (m_sourceRowCache.isEmpty())
		rowCount(QModelIndex());

	QList<int>::iterator it;

	it = qLowerBound(m_sourceRowCache.begin(), m_sourceRowCache.end(), sourceIndex.row());

	if (*it != sourceIndex.row())
		--it;

	int dateRow{qMax(0, it - m_sourceRowCache.begin())};
	int row{sourceIndex.row() - m_sourceRowCache[dateRow]};

	return createIndex(row, sourceIndex.column(), dateRow + 1);
}

QModelIndex HistoryTreeModel::mapToSource(const QModelIndex& proxyIndex) const
{
	int offset{static_cast<int>(proxyIndex.internalId())};

	if (offset == 0)
		return QModelIndex();

	int startDateRow{sourceDateRow(offset - 1)};

	return sourceModel()->index(startDateRow + proxyIndex.row(), proxyIndex.column());
}

QModelIndex HistoryTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (row < 0 || column < 0 || column >= columnCount(parent) || parent.column() > 0)
		return QModelIndex();

	if (!parent.isValid())
		return createIndex(row, column);

	return createIndex(row, column, parent.row() + 1);
}

QModelIndex HistoryTreeModel::parent(const QModelIndex& index) const
{
	int offset{static_cast<int>(index.internalId())};

	if (offset == 0 || !index.isValid())
		return QModelIndex();

	return createIndex(offset - 1, 0);
}

bool HistoryTreeModel::hasChildren(const QModelIndex& parent) const
{
	QModelIndex grandParent{parent.parent()};

	if (!grandParent.isValid())
		return true;

	return false;
}

Qt::ItemFlags HistoryTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
}

bool HistoryTreeModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (row < 0 || count <= 0 || row + count > rowCount(parent))
		return false;

	if (parent.isValid()) {
		int offset{sourceDateRow(parent.row())};
		return sourceModel()->removeRows(offset + row, count);
	}
	else {
		for (int i{row + count - 1}; i >= row; --i) {
			QModelIndex dateParent{index(i, 0)};
			int offset{sourceDateRow(dateParent.row())};

			if (!sourceModel()->removeRows(offset, rowCount(dateParent)))
				return false;
		}
	}

	return true;
}

QVariant HistoryTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return sourceModel()->headerData(section, orientation, role);
}

void HistoryTreeModel::setSourceModel(QAbstractItemModel* newSourceModel)
{
	beginResetModel();

	if (sourceModel()) {
		disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &HistoryTreeModel::sourceReset);
		disconnect(sourceModel(), &QAbstractItemModel::layoutChanged, this, &HistoryTreeModel::sourceReset);
		disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &HistoryTreeModel::sourceRowsInserted);
		disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryTreeModel::sourceRowsRemoved);
		disconnect(sourceModel(), &QAbstractItemModel::dataChanged, this, &HistoryTreeModel::sourceDataChanged);
	}

	QAbstractProxyModel::setSourceModel(newSourceModel);

	if (sourceModel()) {
		connect(sourceModel(), &QAbstractItemModel::modelReset, this, &HistoryTreeModel::sourceReset);
		connect(sourceModel(), &QAbstractItemModel::layoutChanged, this, &HistoryTreeModel::sourceReset);
		connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &HistoryTreeModel::sourceRowsInserted);
		connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryTreeModel::sourceRowsRemoved);
		connect(sourceModel(), &QAbstractItemModel::dataChanged, this, &HistoryTreeModel::sourceDataChanged);
	}

	endResetModel();
}

void HistoryTreeModel::sourceReset()
{
	beginResetModel();
	m_sourceRowCache.clear();
	endResetModel();
}

void HistoryTreeModel::sourceRowsInserted(const QModelIndex& parent, int start, int end)
{
	Q_UNUSED(parent);
	Q_ASSERT(!parent.isValid());

	if (start != 0 || start != end) {
		beginResetModel();
		m_sourceRowCache.clear();
		endResetModel();
		return;
	}

	m_sourceRowCache.clear();

	QModelIndex treeIndex{mapFromSource(sourceModel()->index(start, 0))};
	QModelIndex treeParent{treeIndex.parent()};

	if (rowCount(treeParent) == 1) {
		beginInsertRows(QModelIndex(), 0, 0);
		endInsertRows();
	}
	else {
		beginInsertRows(treeParent, treeIndex.row(), treeIndex.row());
		endInsertRows();
	}
}

void HistoryTreeModel::sourceRowsRemoved(const QModelIndex& parent, int start, int end)
{
	Q_UNUSED(parent);
	Q_ASSERT(!parent.isValid());

	if (m_sourceRowCache.isEmpty())
		return;

	for (int i{end}; i >= start;) {
		QList<int>::iterator it;
		it = qLowerBound(m_sourceRowCache.begin(), m_sourceRowCache.end(), i);

		if (it == m_sourceRowCache.end()) {
			beginResetModel();
			m_sourceRowCache.clear();
			endResetModel();
			return;
		}

		if (*it != i)
			--it;

		int row{qMax(0, it - m_sourceRowCache.begin())};
		int offset{m_sourceRowCache[row]};
		QModelIndex dateParent{index(row, 0)};
		int rc{rowCount(dateParent)};

		if (i - rc + 1 == offset & start <= i - rc + 1) {
			beginRemoveRows(QModelIndex(), row, row);
			m_sourceRowCache.removeAt(row);
			i -= rc + 1;
		}
		else {
			beginRemoveRows(dateParent, i - offset, i - offset);
			++row;
			--i;
		}

		for (int j{row}; j < m_sourceRowCache.count(); ++j)
			--m_sourceRowCache[j];

		endRemoveRows();
	}
}

void HistoryTreeModel::sourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
										 const QVector<int> roles)
{
	emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
}

int HistoryTreeModel::sourceDateRow(int row) const
{
	if (row <= 0)
		return 0;

	if (m_sourceRowCache.isEmpty())
		rowCount(QModelIndex());

	if (row >= m_sourceRowCache.count()) {
		if (!sourceModel())
			return 0;

		return sourceModel()->rowCount();
	}

	return m_sourceRowCache[row];
}
}