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

#ifndef SIELOBROWSER_TITLEBAR_HPP
#define SIELOBROWSER_TITLEBAR_HPP

#include "Bookmarks/BookmarksToolBar.hpp"
#include "Bookmarks/BookmarksModel.hpp"

#include "BrowserWindow.hpp"

#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include <QMouseEvent>
#include <QContextMenuEvent>

namespace Sn {
class BookmarksModel;

class TitleBar : public BookmarksToolBar {
Q_OBJECT

public:
	TitleBar(BookmarksModel* model, BrowserWindow* window, bool showBookmarks = true);
	~TitleBar();

	void setTitle(const QString& title);

	bool showBookmarks() const { return m_showBookmarks; }
	void setShowBookmark(bool show);

	bool isWindowMaximized() const;

signals:
	void toggleBookmarksBar(bool shown);

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void contextMenuEvent(QContextMenuEvent* event);

protected slots:
	void build() Q_DECL_OVERRIDE;

private slots:
	void closeWindow();
	void toggleMaximize();
	void minimize();

private:
	bool m_showBookmarks{true};
	bool m_isMaximized{false};

	QLabel* m_title{nullptr};
	QToolButton* m_closeButton{nullptr};
	QToolButton* m_toggleMaximize{nullptr};
	QToolButton* m_minimize{nullptr};

	QRect m_geometry{};
	QPoint m_offset{};
	BrowserWindow* m_window{nullptr};
};
}

#endif //SIELOBROWSER_TITLEBAR_HPP
