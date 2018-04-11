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

#include "Bookmarks/BookmarksCommands.hpp"


#include "Bookmarks/BookmarkManager.hpp"
#include "Bookmarks/BookmarkNode.hpp"

namespace Sn {

RemoveBookmarksCommand::RemoveBookmarksCommand(BookmarksManager* bookmarkManager, BookmarkNode* parent, int row) :
	QUndoCommand(BookmarksManager::tr("Remove Bookmark")),
	m_row(row),
	m_done(false),
	m_bookmarkManager(bookmarkManager),
	m_node(parent->children().value(row)),
	m_parent(parent)
{
	// Empty
}

RemoveBookmarksCommand::~RemoveBookmarksCommand()
{
	if (m_done && !m_node->parent())
		delete m_node;
}

void RemoveBookmarksCommand::undo()
{
	m_parent->add(m_node, m_row);
	emit m_bookmarkManager->entryAdded(m_node);

	m_done = false;
}

void RemoveBookmarksCommand::redo()
{
	m_parent->remove(m_node);
	emit m_bookmarkManager->entryRemoved(m_parent, m_row, m_node);
	m_done = true;
}

InsertBookmarksCommand::InsertBookmarksCommand(BookmarksManager* bookmarkManager, BookmarkNode* parent,
											   BookmarkNode* node, int row) :
	RemoveBookmarksCommand(bookmarkManager, parent, row)
{
	setText(BookmarksManager::tr("Insert Bookmark"));
	m_node = node;
}

ChangeBookmarksCommand::ChangeBookmarksCommand(BookmarksManager* bookmarkManager, BookmarkNode* node,
											   const QString& newValue, bool title) :
	QUndoCommand(),
	m_bookmarkManager(bookmarkManager),
	m_node(node),
	m_newValue(newValue),
	m_title(title)
{
	if (m_title) {
		m_oldValue = m_node->title;
		setText(BookmarksManager::tr("Name Change"));
	}
	else {
		m_oldValue = m_node->url;
		setText(BookmarksManager::tr("Address Change"));
	}
}

void ChangeBookmarksCommand::undo()
{
	if (m_title)
		m_node->title = m_oldValue;
	else
		m_node->url = m_oldValue;

	emit m_bookmarkManager->entryChanged(m_node);
}

void ChangeBookmarksCommand::redo()
{
	if (m_title)
		m_node->title = m_newValue;
	else
		m_node->url = m_newValue;

	emit m_bookmarkManager->entryChanged(m_node);
}
}