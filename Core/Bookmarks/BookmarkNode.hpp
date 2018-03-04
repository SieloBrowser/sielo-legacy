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
#ifndef SIELOBROWSER_BOOKMARKNODE_HPP
#define SIELOBROWSER_BOOKMARKNODE_HPP

#include <QList>
#include <QString>

namespace Sn {

class BookmarkNode {
public:
	enum Type {
		Root,
		Folder,
		Bookmark,
		Separator
	};

	BookmarkNode(Type type = Root, BookmarkNode* parent = nullptr);
	~BookmarkNode();

	Type type() const { return m_type; }
	QList<BookmarkNode*> children() const { return m_children; }
	BookmarkNode* parent() const { return m_parent; }

	void setType(Type type);

	void add(BookmarkNode* child, int offset = -1);
	void remove(BookmarkNode* child);

	bool operator==(const BookmarkNode& other);

	QString url{};
	QString title{};
	QString descritption{};
	bool expanded{false};

private:
	BookmarkNode* m_parent{nullptr};
	Type m_type{};
	QList<BookmarkNode*> m_children;
};

}

#endif //SIELOBROWSER_BOOKMARKNODE_HPP
