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
#ifndef SIELOBROWSER_HISTORYMODEL_HPP
#define SIELOBROWSER_HISTORYMODEL_HPP

#include <QAbstractTableModel>

#include <QVariant>

namespace Sn {
struct HistoryItem;
class HistoryManager;

class HistoryModel: public QAbstractTableModel {
Q_OBJECT

public:
	enum Roles {
		DateRole = Qt::UserRole + 1,
		DateTimeRole = Qt::UserRole + 2,
		UrlRole = Qt::UserRole + 3,
		UrlStringRole = Qt::UserRole + 4
	};

	HistoryModel(HistoryManager* history, QObject* parent = nullptr);
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

public slots:
	void historyReset();
	void entryAdded(const HistoryItem& item);
	void entryRemoved(const HistoryItem& item);
	void entryUpdated(int offset);
private:
	HistoryManager* m_history{nullptr};
};
}

#endif //SIELOBROWSER_HISTORYMODEL_HPP
