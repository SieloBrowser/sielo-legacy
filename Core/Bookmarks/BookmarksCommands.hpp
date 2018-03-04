/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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
#ifndef SIELOBROWSER_BOOKMARKSCOMMANDS_HPP
#define SIELOBROWSER_BOOKMARKSCOMMANDS_HPP

#include <QUndoCommand>

namespace Sn {
class BookmarksManager;
class BookmarkNode;

class RemoveBookmarksCommand: public QUndoCommand {
public:
	RemoveBookmarksCommand(BookmarksManager* bookmarkManager, BookmarkNode* parent, int row);
	~RemoveBookmarksCommand();

	void undo();
	void redo();

protected:
	int m_row{};
	bool m_done{false};

	BookmarksManager* m_bookmarkManager{nullptr};
	BookmarkNode* m_node{nullptr};
	BookmarkNode* m_parent{nullptr};
};

class InsertBookmarksCommand: public RemoveBookmarksCommand {

public:
	InsertBookmarksCommand(BookmarksManager* bookmarkManager, BookmarkNode* parent, BookmarkNode* node, int row);

	void undo() { RemoveBookmarksCommand::undo(); }
	void redo() { RemoveBookmarksCommand::redo(); }
};

class ChangeBookmarksCommand: public QUndoCommand {
public:
	ChangeBookmarksCommand(BookmarksManager* bookmarkManager, BookmarkNode* node, const QString& newValue, bool title);

	void undo();
	void redo();

private:
	BookmarksManager* m_bookmarkManager{nullptr};
	BookmarkNode* m_node{nullptr};

	QString m_oldValue{};
	QString m_newValue{};

	bool m_title{false};
};
}
#endif //SIELOBROWSER_BOOKMARKSCOMMANDS_HPP
