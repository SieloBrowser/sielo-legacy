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

#include "HistoryMenuModel.hpp"


#include "History/HistoryTreeModel.hpp"

namespace Sn {

HistoryMenuModel::HistoryMenuModel(HistoryTreeModel* sourceModel, QObject* parent) :
	QAbstractProxyModel(parent),
	m_treeModel(sourceModel)
{
	setSourceModel(sourceModel);
}

int HistoryMenuModel::columnCount(const QModelIndex& parent) const
{
	return m_treeModel->columnCount(mapToSource(parent));
}

int HistoryMenuModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid()) {
		int folders{sourceModel()->rowCount()};
		int bumpedItems{bumpedRows()};

		if (bumpedItems <= 15 && bumpedItems == sourceModel()->rowCount(sourceModel()->index(0, 0)))
			--folders;

		return bumpedItems + folders;
	}

	if (parent.internalId() == quintptr(-1)) {
		if (parent.row() < bumpedRows())
			return 0;
	}

	QModelIndex index{mapToSource(parent)};
	int defaultCount{sourceModel()->rowCount(index)};

	if (index == sourceModel()->index(0, 0))
		return defaultCount - bumpedRows();

	return defaultCount;
}

QModelIndex HistoryMenuModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	Q_ASSERT(false);

	int sourceRow{m_treeModel->mapToSource(sourceIndex).row()};

	return createIndex(sourceIndex.row(), sourceIndex.column(), sourceRow);
}

QModelIndex HistoryMenuModel::mapToSource(const QModelIndex& proxyIndex) const
{
	if (!proxyIndex.isValid())
		return QModelIndex();

	if (proxyIndex.internalId() == quintptr(-1)) {
		int bumpedItems{bumpedRows()};

		if (proxyIndex.row() < bumpedItems)
			return m_treeModel->index(proxyIndex.row(), proxyIndex.column(), m_treeModel->index(0, 0));

		if (bumpedItems <= 15 && bumpedItems == sourceModel()->rowCount(m_treeModel->index(0, 0)))
			--bumpedItems;

		return m_treeModel->index(proxyIndex.row() - bumpedItems, proxyIndex.column());
	}

	QModelIndex historyIndex{m_treeModel->sourceModel()->index(proxyIndex.internalId(), proxyIndex.column())};
	QModelIndex treeIndex{m_treeModel->mapFromSource(historyIndex)};

	return treeIndex;
}

QModelIndex HistoryMenuModel::index(int row, int column, const QModelIndex& parent) const
{
	if (row < 0 || column < 0 || column >= columnCount(parent) || parent.column() > 0)
		return QModelIndex();

	if (!parent.isValid())
		return createIndex(row, column, quintptr(-1));

	QModelIndex treeIndexParent{mapToSource(parent)};
	int bumpedItems{0};

	if (treeIndexParent == m_treeModel->index(0, 0))
		bumpedItems = bumpedRows();

	QModelIndex treeIndex{m_treeModel->index(row + bumpedItems, column, treeIndexParent)};
	QModelIndex historyIndex{m_treeModel->mapToSource(treeIndex)};
	int historyRow{historyIndex.row()};

	if (historyRow == -1)
		historyRow = treeIndex.row();

	return createIndex(row, column, historyRow);
}

QModelIndex HistoryMenuModel::parent(const QModelIndex& index) const
{
	int offset{static_cast<int>(index.internalId())};

	if (offset == -1 || !index.isValid())
		return QModelIndex();

	QModelIndex historyIndex{m_treeModel->sourceModel()->index(index.internalId(), 0)};
	QModelIndex treeIndex{m_treeModel->mapFromSource(historyIndex)};
	QModelIndex treeIndexParent{treeIndex.parent()};

	int sourceRow{m_treeModel->mapToSource(treeIndexParent).row()};
	int bumpedItems{bumpedRows()};

	if (bumpedItems <= 15 && bumpedItems == sourceModel()->rowCount(sourceModel()->index(0, 0)))
		--bumpedItems;

	return createIndex(bumpedItems + treeIndexParent.row(), treeIndexParent.column(), sourceRow);
}

int HistoryMenuModel::bumpedRows() const
{
	QModelIndex first{m_treeModel->index(0, 0)};

	if (!first.isValid())
		return 0;

	return qMin(m_treeModel->rowCount(first), 15);
}

}