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
#ifndef SIELOBROWSER_BOOKMARKS_HPP
#define SIELOBROWSER_BOOKMARKS_HPP

#include "SharedDefines.hpp"

#include <QObject>

#include <QVariant>

namespace Sn
{
class AutoSaver;

class BookmarkItem;
class BookmarksModel;

class SIELO_SHAREDLIB Bookmarks: public QObject {
Q_OBJECT

public:
	Bookmarks(QObject* parent = nullptr);
	~Bookmarks();

	void loadSettings();

	bool showOnlyIconsInToolbar() const { return m_showOnlyIconsInToolbar; }
	bool showOnlyTextInToolbar() const { return m_showOnlyTextInToolbar; }

	BookmarkItem *rootItem() const { return m_root; }
	BookmarkItem *toolbarFolder() const { return m_folderToolbar; }
	BookmarkItem *menuFolder() const { return m_folderMenu; }
	BookmarkItem *unsortedFolder() const { return m_folderUnsorted; }
	BookmarkItem *lastUsedFolder() const { return m_lastFolder; }

	BookmarksModel *model() const { return m_model; }

	bool isBookmarked(const QUrl& url);
	bool canBeModified(BookmarkItem* item) const;

	QList<BookmarkItem*> searchBookmarks(const QUrl& url) const;
	QList<BookmarkItem*> searchBookmarks(const QString& string, int limit = -1,
	                                     Qt::CaseSensitivity sensitive = Qt::CaseInsensitive) const;
	QList<BookmarkItem*> searchKeyword(const QString& keyword) const;

	void addBookmark(BookmarkItem* parent, BookmarkItem* item);
	void insertBookmark(BookmarkItem* parent, int row, BookmarkItem* item);
	bool removeBookmark(BookmarkItem* item);
	void changeBookmark(BookmarkItem* item);

signals:
	void bookmarkAdded(BookmarkItem* item);
	void bookmarkRemoved(BookmarkItem* item);
	void bookmarkChanged(BookmarkItem* item);

	void showOnlyIconsInToolbarChanged(bool show);
	void showOnlyTextInToolbarChanged(bool show);

public slots:
	void save();

	void setShowOnlyIconsInToolbar(bool state);
	void setShowOnlyTextInToolbar(bool state);

private:
	void loadBookmarks();
	void saveBookmarks();

	QVariantMap writeFolder(BookmarkItem* folder);
	void readFolder(const QString& name, const QVariantMap& map, BookmarkItem* folder);

	void loadBookmarksFromMap(const QVariantMap& map);
	void readBookmarks(const QVariantList& list, BookmarkItem* parent);
	QVariantList writeBookmarks(BookmarkItem* parent);

	void search(QList<BookmarkItem*>* items, BookmarkItem* parent, const QUrl& url) const;
	void search(QList<BookmarkItem*>* items, BookmarkItem* parent, const QString& string, int limit,
	            Qt::CaseSensitivity sensitive) const;
	void searchKeyword(QList<BookmarkItem*>* items, BookmarkItem* parent, const QString& keyword) const;

	BookmarkItem* m_root{nullptr};
	BookmarkItem* m_folderToolbar{nullptr};
	BookmarkItem* m_folderMenu{nullptr};
	BookmarkItem* m_folderUnsorted{nullptr};
	BookmarkItem* m_lastFolder{nullptr};

	BookmarksModel* m_model{nullptr};
	AutoSaver* m_autoSaver{nullptr};

	bool m_showOnlyIconsInToolbar{false};
	bool m_showOnlyTextInToolbar{false};
};
}

#endif //SIELOBROWSER_BOOKMARKS_HPP
