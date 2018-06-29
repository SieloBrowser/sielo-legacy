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
#include "History/HistoryModel.hpp"
#include "History/HistoryFilterModel.hpp"

#include "Utils/IconProvider.hpp"

#include "Application.hpp"

namespace Sn
{
HistoryTreeView::HistoryTreeView(QWidget* parent) :
	QTreeView(parent),
	m_history(Application::instance()->history()),
	m_filter(new HistoryFilterModel(m_history->model()))
{
	setModel(m_filter);
	setUniformRowHeights(true);
	setAllColumnsShowFocus(true);

	connect(m_filter, &HistoryFilterModel::expandAllItems, this, &HistoryTreeView::expandAll);
	connect(m_filter, &HistoryFilterModel::collapseAllItems, this, &HistoryTreeView::collapseAll);
}

QUrl HistoryTreeView::selectedUrl() const
{
	const QList<QModelIndex> indexes = selectionModel()->selectedRows();

	if (indexes.count() != 1)
		return QUrl();

	return indexes[0].data(HistoryModel::UrlRole).toUrl();
}

void HistoryTreeView::search(const QString& string)
{
	m_filter->setFilterFixedString(string);
}

void HistoryTreeView::removeSelectedItems()
{
	QList<int> list{};
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QList<QPersistentModelIndex> topLevelIndexes{};

	foreach (const QModelIndex& index, selectedIndexes()) {
		if (index.column() > 0)
			continue;

		if (index.data(HistoryModel::IsTopLevelRole).toBool()) {
			qint64 start{index.data(HistoryModel::TimestampStartRole).toLongLong()};
			qint64 end{index.data(HistoryModel::TimestampEndRole).toLongLong()};

			list.append(m_history->indexesFromTimeRange(start, end));

			topLevelIndexes.append(index);
		}
		else {
			int id{index.data(HistoryModel::IdRole).toInt()};
			if (!list.contains(id))
				list.append(id);
		}
	}

	m_history->deleteHistoryEntry(list);
	m_history->model()->removeTopLevelIndexes(topLevelIndexes);

	QApplication::restoreOverrideCursor();
}

void HistoryTreeView::contextMenuEvent(QContextMenuEvent* event)
{
	emit contextMenuRequested(viewport()->mapToGlobal(event->pos()));
}

void HistoryTreeView::mousePressEvent(QMouseEvent* event)
{
	QTreeView::mousePressEvent(event);

	if (selectionModel()->selectedRows().count() == 1) {
		QModelIndex index{indexAt(event->pos())};
		Qt::MouseButtons buttons = event->buttons();
		Qt::KeyboardModifiers modifiers = event->modifiers();

		if (index.isValid() && !index.data(HistoryModel::IsTopLevelRole).toBool()) {
			const QUrl url{index.data(HistoryModel::UrlRole).toUrl()};

			if (buttons == Qt::LeftButton && modifiers == Qt::ShiftModifier)
			emit urlShiftActivated(url);
			else if (buttons == Qt::MiddleButton || (buttons == Qt::LeftButton && modifiers == Qt::ControlModifier))
			emit urlCtrlActivated(url);
		}
	}
}

void HistoryTreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
	QTreeView::mousePressEvent(event);

	if (selectionModel()->selectedRows().count() == 1) {
		QModelIndex index{indexAt(event->pos())};
		Qt::MouseButtons buttons = event->buttons();
		Qt::KeyboardModifiers modifiers = event->modifiers();

		if (index.isValid() && !index.data(HistoryModel::IsTopLevelRole).toBool()) {
			const QUrl url{index.data(HistoryModel::UrlRole).toUrl()};

			if (buttons == Qt::LeftButton && modifiers == Qt::NoModifier)
			emit urlActivated(url);
			else if (buttons == Qt::LeftButton && modifiers == Qt::ShiftModifier)
			emit urlShiftActivated(url);
		}
	}
}

void HistoryTreeView::keyPressEvent(QKeyEvent* event)
{
	QTreeView::keyPressEvent(event);

	if (selectionModel()->selectedRows().count() == 1) {
		QModelIndex index{selectionModel()->selectedRows()[0]};
		const QUrl url{index.data(HistoryModel::UrlRole).toUrl()};
		const bool isTopLevel{index.data(HistoryModel::IsTopLevelRole).toBool()};

		switch (event->key()) {
		case Qt::Key_Return:
		case Qt::Key_Enter:
			if (isTopLevel && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::KeypadModifier))
				setExpanded(index, !isExpanded(index));
			else {
				Qt::KeyboardModifiers modifiers = event->modifiers();

				if (modifiers == Qt::NoModifier || modifiers == Qt::KeypadModifier)
					emit urlActivated(url);
				else if (modifiers == Qt::ControlModifier)
					emit urlCtrlActivated(url);
				else if (modifiers == Qt::ShiftModifier)
					emit urlShiftActivated(url);
			}
			break;
		case Qt::Key_Delete:
			removeSelectedItems();
			break;
		}
	}
}

void HistoryTreeView::drawRow(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const
{
	bool itemTopLevel{ index.data(HistoryModel::IsTopLevelRole).toBool() };
	bool iconLoaded{ !index.data(HistoryModel::IconRole).value<QIcon>().isNull() };

	if (index.isValid() && !itemTopLevel && !iconLoaded) {
		const QPersistentModelIndex idx = index;
		model()->setData(idx, IconProvider::iconForUrl(index.data(HistoryModel::UrlRole).toUrl()), HistoryModel::IconRole);
	}

	QTreeView::drawRow(painter, options, index);
}
}
