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

#include "HistoryItem.hpp"

namespace Sn
{
HistoryItem::HistoryItem(HistoryItem* parent) :
	canFetchMore(false),
	m_parent(parent),
	m_startTimestamp(0),
	m_endTimestamp(0)
{
	if (m_parent)
		m_parent->appendChild(this);
}

HistoryItem::~HistoryItem()
{
	if (m_parent)
		m_parent->removeChild(this);

	qDeleteAll(m_children);
}

void HistoryItem::changeParent(HistoryItem* parent)
{
	if (m_parent)
		m_parent->removeChild(this);

	m_parent = parent;

	if (m_parent)
		m_parent->prependChild(this);
}

HistoryItem *HistoryItem::child(int row) const
{
	if (row >= 0 && m_children.count() > row)
		return m_children[row];

	return nullptr;
}

int HistoryItem::childCount() const
{
	return m_children.count();
}

void HistoryItem::prependChild(HistoryItem* child)
{
	if (m_children.contains(child))
		m_children.removeAll(child);

	child->m_parent = this;
	m_children.prepend(child);
}

void HistoryItem::appendChild(HistoryItem* child)
{
	if (m_children.contains(child))
		m_children.removeAll(child);

	child->m_parent = this;
	m_children.append(child);
}

void HistoryItem::instertChild(int row, HistoryItem* child)
{
	if (m_children.contains(child))
		m_children.removeAll(child);

	if (m_children.count() >= row) {
		child->m_parent = this;
		m_children.insert(row, child);
	}
}

void HistoryItem::removeChild(int row)
{
	if (row >= 0 && m_children.count() > row)
		removeChild(m_children[row]);
}

void HistoryItem::removeChild(HistoryItem* child)
{
	m_children.removeOne(child);
}

int HistoryItem::row()
{
	return m_parent ? m_parent->indexOfChild(this) : 0;
}

int HistoryItem::indexOfChild(HistoryItem* child) const
{
	return m_children.indexOf(child);
}

bool HistoryItem::isTopLevel() const
{
	return (m_startTimestamp != 0);
}

void HistoryItem::setIcon(const QIcon& icon)
{
	m_icon = icon;
}

qint64 HistoryItem::startTimestamp() const
{
	if (m_startTimestamp == -1)
		return QDateTime::currentDateTime().toMSecsSinceEpoch();

	return m_startTimestamp;
}

void HistoryItem::setStartTimestamp(quint64 start)
{
	m_startTimestamp = start;
}

void HistoryItem::setEndTimestamp(quint64 end)
{
	m_endTimestamp = end;
}

}
