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
#ifndef SIELOBROWSER_HISTORYTREEMODEL_HPP
#define SIELOBROWSER_HISTORYTREEMODEL_HPP

#include <QAbstractProxyModel>
#include <QModelIndex>

#include <QVariant>
#include <QVector>
#include <QList>

namespace Sn {
class HistoryTreeModel: public QAbstractProxyModel {
Q_OBJECT
public:
	explicit HistoryTreeModel(QAbstractItemModel* sourceModel, QObject* parent = nullptr);
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	int columnCount(const QModelIndex& parent) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
	QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index = QModelIndex()) const;
	bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	void setSourceModel(QAbstractItemModel* newSourceModel);

private slots:
	void sourceReset();
	void sourceRowsInserted(const QModelIndex& parent, int start, int end);
	void sourceRowsRemoved(const QModelIndex& parent, int start, int end);
	void sourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> roles);

private:
	int sourceDateRow(int row) const;
	mutable QList<int> m_sourceRowCache;
};
}

#endif //SIELOBROWSER_HISTORYTREEMODEL_HPP
