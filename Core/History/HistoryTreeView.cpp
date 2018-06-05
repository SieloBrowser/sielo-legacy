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

#include "HistoryTreeView.hpp"

#include "History/History.hpp"
#include "History/HistoryFilterModel.hpp"

#include "Application.hpp"

namespace Sn
{
HistoryTreeView::HistoryTreeView(QWidget* parent) :
	QTreeView(parent) {}

QUrl HistoryTreeView::selectedUrl() const { return QUrl(); }
void HistoryTreeView::search(const QString& string) {}
void HistoryTreeView::removeSelectedItems() {}
void HistoryTreeView::contextMenuEvent(QContextMenuEvent* event) {}
void HistoryTreeView::mouseMoveEvent(QMouseEvent* event) {}
void HistoryTreeView::mousePressEvent(QMouseEvent* event) {}
void HistoryTreeView::mouseReleaseEvent(QMouseEvent* event) {}
void HistoryTreeView::mouseDoubleClickEvent(QMouseEvent* event) {}
void HistoryTreeView::keyPressEvent(QKeyEvent* event) {}
void HistoryTreeView::drawRow(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const {}
}
