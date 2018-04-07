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
#ifndef SIELOBROWSER_BOOKMARKMANAGER_HPP
#define SIELOBROWSER_BOOKMARKMANAGER_HPP

#include <QObject>

#include <QUndoStack>

namespace Sn {
class AutoSaver;
class BookmarkNode;
class BookmarksModel;
class RemoveBookmarksCommand;
class ChangeBookmarksCommand;

class BookmarksManager: public QObject {
Q_OBJECT

public:
	BookmarksManager(QObject* parent = nullptr);
	~BookmarksManager();

	void addBookmark(BookmarkNode* parent, BookmarkNode* node, int row = -1);
	void removeBookmark(BookmarkNode* node);

	void setTitle(BookmarkNode* node, const QString& newTitle);
	void setUrl(BookmarkNode* node, const QString& newUrl);
	void changeExpanded();

	BookmarkNode* bookmarks();
	BookmarksModel* bookmarksModel();

	QUndoStack* undoRedoStack() { return &m_commands; }

signals:
	void entryAdded(BookmarkNode* item);
	void entryRemoved(BookmarkNode* parent, int row, BookmarkNode* item);
	void entryChanged(BookmarkNode* item);

public slots:
	void importBookmarks();
	void exportBookmarks();

	void showBookmarks();

private slots:
	void save() const;

private:
	void load();

	bool m_loaded{false};

	AutoSaver* m_saveTimer{nullptr};
	BookmarkNode* m_bookmarkRootNode{nullptr};
	BookmarksModel* m_bookmarksModel{nullptr};

	QUndoStack m_commands;

	friend class RemoveBookmarksCommand;
	friend class ChangeBookmarksCommand;
};

}
#endif //SIELOBROWSER_BOOKMARKMANAGER_HPP
