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

#include "BookmarksFilterModel.hpp"

#include "Bookmarks/BookmarksModel.hpp"
#include "Bookmarks/BookmarkItem.hpp"

namespace Sn
{
BookmarksFilterModel::BookmarksFilterModel(QAbstractItemModel* parent) :
	QSortFilterProxyModel(parent)
{
	setSourceModel(parent);
	setFilterCaseSensitivity(Qt::CaseInsensitive);

	m_filterTimer = new QTimer(this);
	m_filterTimer->setSingleShot(true);
	m_filterTimer->setInterval(300);

	connect(m_filterTimer, &QTimer::timeout, this, &BookmarksFilterModel::startFiltering);
}

BookmarksFilterModel::~BookmarksFilterModel()
{
	// Empty
}

void BookmarksFilterModel::setFilterFixedString(const QString& pattern)
{
	m_pattern = pattern;

	m_filterTimer->start();
}

bool BookmarksFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	const QModelIndex index{sourceModel()->index(sourceRow, 0, sourceParent)};

	if (index.data(BookmarksModel::TypeRole).toInt() == BookmarkItem::Folder)
		return true;

	return (index.data(BookmarksModel::TitleRole).toString().contains(m_pattern, filterCaseSensitivity()) ||
		index.data(BookmarksModel::UrlStringRole).toString().contains(m_pattern, filterCaseSensitivity()) ||
		index.data(BookmarksModel::DescriptionRole).toString().contains(m_pattern, filterCaseSensitivity()) ||
		index.data(BookmarksModel::KeywordRole).toString().compare(m_pattern, filterCaseSensitivity()) == 0);
}

void BookmarksFilterModel::startFiltering()
{
	QSortFilterProxyModel::setFilterFixedString(m_pattern);
}
}
