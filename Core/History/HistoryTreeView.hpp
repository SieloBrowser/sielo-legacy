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
#ifndef SIELOBROWSER_HistoryTreeView_HPP
#define SIELOBROWSER_HistoryTreeView_HPP

#include "SharedDefines.hpp"

#include <QWidget>
#include <QTreeView>

#include <QMouseEvent>

namespace Sn
{
class History;
class HistoryFilterModel;


class SIELO_SHAREDLIB HistoryTreeView: public QTreeView {
Q_OBJECT

public:
	HistoryTreeView(QWidget* parent = nullptr);

	QUrl selectedUrl() const;

signals:
	// Open url in current tab
	void urlActivated(const QUrl& url);
	// Open url in new tab
	void urlCtrlActivated(const QUrl& url);
	// Open url in new window
	void urlShiftActivated(const QUrl& url);
	// Context menu signal with point mapped to global
	void contextMenuRequested(const QPoint& point);

public slots:
	void search(const QString& string);
	void removeSelectedItems();

protected:
	void contextMenuEvent(QContextMenuEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);

	void drawRow(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const;

private:
	History* m_history{nullptr};
	HistoryFilterModel* m_filter{nullptr};
};
}

#endif //SIELOBROWSER_HistoryTreeView_HPP
