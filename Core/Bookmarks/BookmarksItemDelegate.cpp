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

#include "BookmarksItemDelegate.hpp"

#include <QApplication>

#include "Bookmarks/BookmarkItem.hpp"
#include "Bookmarks/BookmarksModel.hpp"

namespace Sn
{
BookmarksItemDelegate::BookmarksItemDelegate(QTreeView* parent) :
	QStyledItemDelegate(parent),
	m_tree(parent)
{
	Q_ASSERT(m_tree);
}

BookmarksItemDelegate::~BookmarksItemDelegate()
{
	// Empty
}

void BookmarksItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	if (index.data(BookmarksModel::TypeRole).toInt() == BookmarkItem::Separator) {
		QStyleOption opt{option};
		opt.state &= ~QStyle::State_Horizontal;

		if (m_tree->model()->columnCount(index) == 2) {
			if (index.column() == 1)
				opt.rect = m_lastRect;
			else {
				opt.rect.setWidth(opt.rect.width() + m_tree->columnWidth(1));
				m_lastRect = opt.rect;
			}
		}

		QApplication::style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, painter);
	}
}
}
