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
#ifndef SIELOBROWSER_HISTORYITEM_HPP
#define SIELOBROWSER_HISTORYITEM_HPP

#include "SharedDefines.hpp"

#include <QList>

#include <QIcon>

#include "History/History.hpp"

namespace Sn
{
class SIELO_SHAREDLIB HistoryItem {
public:
	HistoryItem(HistoryItem* parent = nullptr);
	~HistoryItem();

	void changeParent(HistoryItem* parent);
	HistoryItem *parent() const { return m_parent; }

	HistoryItem* child(int row) const;
	int childCount() const;

	void prependChild(HistoryItem* child);
	void appendChild(HistoryItem* child);
	void instertChild(int row, HistoryItem* child);

	void removeChild(int row);
	void removeChild(HistoryItem* child);

	int row();
	int indexOfChild(HistoryItem* child) const;

	bool isTopLevel() const;

	QIcon icon() const { return m_icon; }
	void setIcon(const QIcon& icon);

	qint64 startTimestamp() const;
	void setStartTimestamp(quint64 start);

	qint64 endTimestamp() const { return m_endTimestamp; }
	void setEndTimestamp(quint64 end);

	History::HistoryEntry historyEntry{};
	QString title{};
	bool canFetchMore{false};

private:
	HistoryItem* m_parent{nullptr};
	QList<HistoryItem*> m_children{};

	QIcon m_icon{};

	qint64 m_startTimestamp;
	qint64 m_endTimestamp;
};
}

#endif //SIELOBROWSER_HISTORYITEM_HPP
