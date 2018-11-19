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
#ifndef SIELOBROWSER_BOOKMARKSTOOLBARBUTTON_HPP
#define SIELOBROWSER_BOOKMARKSTOOLBARBUTTON_HPP

#include "SharedDefines.hpp"

#include <QPushButton>

#include <QMenu>

namespace Sn
{
class BookmarkItem;

class BrowserWindow;

class SIELO_SHAREDLIB BookmarksToolbarButton: public QPushButton {
Q_OBJECT

public:
	BookmarksToolbarButton(BookmarkItem* bookmark, QWidget* parent = nullptr);
	~BookmarksToolbarButton();

	BookmarkItem *bookmark() const { return m_bookmark; }
	void setMainWindow(BrowserWindow* window);

	bool showOnlyIcon() const { return m_showOnlyIcon; }
	void setShowOnlyIcon(bool show);

	bool showOnlyText() const { return m_showOnlyText; }
	void setShowOnlyText(bool show);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

private slots:
	void createMenu();

	void menuAboutToShow();
	void bookmarkActivated(BookmarkItem* item = nullptr);
	void bookmarkCtrlActivated(BookmarkItem* item = nullptr);
	void bookmarkShiftActivated(BookmarkItem* item = nullptr);

	void openFolder(BookmarkItem* item);
	void openBookmark(BookmarkItem* item);
	void openBookmarkInNewTab(BookmarkItem* item);
	void openBookmarkInNewWindow(BookmarkItem* item);

private:
	void refresh();
	QString createTooltip() const;

	void enterEvent(QEvent* event);
	void leaveEvent(QEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	void addActionToMenu(QMenu* menu, BookmarkItem* item);
	void addUrlToMenu(QMenu* menu, BookmarkItem* bookmark);
	void addFolderToMenu(QMenu* menu, BookmarkItem* folder);
	void addSeparatorToMenu(QMenu* menu, BookmarkItem* separator);
	void addFolderContentsToMenu(QMenu* menu, BookmarkItem* folder);

	BookmarkItem* m_bookmark{nullptr};
	BrowserWindow* m_window{nullptr};

	bool m_showOnlyIcon{false};
	bool m_showOnlyText{false};
};
}

#endif //SIELOBROWSER_BOOKMARKSTOOLBARBUTTON_HPP
