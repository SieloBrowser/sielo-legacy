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
#ifndef SIELOBROWSER_BOOKMARKSMANAGER_HPP
#define SIELOBROWSER_BOOKMARKSMANAGER_HPP

#include "SharedDefines.hpp"

#include <QDialog>

#include <QGridLayout>
#include <QFormLayout>

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QSpacerItem>

#include <QPointer>

namespace Sn
{
class BrowserWindow;

class Bookmarks;
class BookmarkItem;
class BookmarksTreeView;

class SIELO_SHAREDLIB BookmarksManager: public QDialog {
Q_OBJECT

public:
	BookmarksManager(BrowserWindow* window, QWidget* parent = nullptr);
	~BookmarksManager();

	void setMainWindow(BrowserWindow* window);

public slots:
	void search(const QString& string);

private slots:
	void bookmarkActivated(BookmarkItem* item);
	void bookmarkCtrlActivated(BookmarkItem* item);
	void bookmarkShiftActivated(BookmarkItem* item);
	void bookmarksSelected(const QList<BookmarkItem*>& items);
	void createContextMenu(const QPoint& pos);

	void importBookmarks();
	void exportBookmarks();

	void openBookmark(BookmarkItem* item = 0);
	void openBookmarkInNewTab(BookmarkItem* item = 0);
	void openBookmarkInNewWindow(BookmarkItem* item = 0);
	void openBookmarkInNewPrivateWindow(BookmarkItem* item = 0);

	void addBookmark();
	void addFolder();
	void addSeparator();
	void deleteBookmarks();

	void bookmarkEdited();
	void descriptionEdited();
	void enableUpdates();

private:
	void setupUI();

	void updateEditBox(BookmarkItem* item);
	bool bookmarkEditable(BookmarkItem* item) const;
	void addBookmark(BookmarkItem* item);

	BookmarkItem *parentForNewBookmark() const;
	BrowserWindow *getWindow() const;

	void showEvent(QShowEvent* event);
	void keyPressEvent(QKeyEvent* event);

	QPointer<BrowserWindow> m_window{};

	Bookmarks* m_bookmarks{nullptr};
	BookmarkItem* m_selectedBookmark{nullptr};

	bool m_blockDescriptionChangedSignal{false};
	bool m_adjustHeaderSizesOnShow{true};

	QGridLayout* m_layout{nullptr};
	QFormLayout* m_editLayout{nullptr};

	QGroupBox* m_editBox{nullptr};

	QPushButton* m_importExport{ nullptr };
	QLineEdit* m_search{nullptr};
	BookmarksTreeView* m_view{nullptr};

	QLabel* m_titleDesc{nullptr};
	QLineEdit* m_title{nullptr};
	QLabel* m_addressDesc{nullptr};
	QLineEdit* m_address{nullptr};
	QLabel* m_keywordDesc{nullptr};
	QLineEdit* m_keyword{nullptr};
	QLabel* m_descriptionDesc{nullptr};
	QPlainTextEdit* m_description{nullptr};

};
}

#endif //SIELOBROWSER_BOOKMARKSMANAGER_HPP
