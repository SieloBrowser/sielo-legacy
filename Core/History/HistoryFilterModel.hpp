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
#ifndef SIELOBROWSER_HISTORYFILTERMODEL_HPP
#define SIELOBROWSER_HISTORYFILTERMODEL_HPP

#include <QModelIndex>

#include <QAbstractProxyModel>
#include <QAbstractItemModel>

#include <QList>
#include <QHash>

namespace Sn {

class HistoryFilterModel: public QAbstractProxyModel {
Q_OBJECT

public:
	HistoryFilterModel(QAbstractItemModel* sourceModel, QObject* parent = nullptr);

	inline bool historyContains(const QString& url) const
	{
		load();
		return m_historyHash.contains(url);
	}

	QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
	QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
	void setSourceModel(QAbstractItemModel* newSourceModel);
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index = QModelIndex()) const;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

private slots:
	void sourceReset();
	void sourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void sourceRowsInserted(const QModelIndex& parent, int start, int end);
	void sourceRowsRemoved(const QModelIndex& index, int start, int end);

private:
	void load() const;

	mutable QList<int> m_sourceRow{};
	mutable QHash<QString, int> m_historyHash{};
	mutable bool m_loaded{false};
};
}
#endif //SIELOBROWSER_HISTORYFILTERMODEL_HPP
