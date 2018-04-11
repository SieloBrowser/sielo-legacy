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
#ifndef SIELO_BROWSER_HISTORYMENUMODEL_HPP
#define SIELO_BROWSER_HISTORYMENUMODEL_HPP

#include <QAbstractProxyModel>
#include <QObject>

namespace Sn {
class HistoryTreeModel;

class HistoryMenuModel: public QAbstractProxyModel {
Q_OBJECT

public:
	HistoryMenuModel(HistoryTreeModel* sourceModel, QObject* parent = 0);

	int columnCount(const QModelIndex& parent) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
	QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index = QModelIndex()) const;

	int bumpedRows() const;

private:
	HistoryTreeModel* m_treeModel{nullptr};
};

}

#endif //SIELO_BROWSER_HISTORYMENUMODEL_HPP
