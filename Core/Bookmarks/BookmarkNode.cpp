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

#include "Bookmarks/BookmarkNode.hpp"

namespace Sn {

BookmarkNode::BookmarkNode(Type type, BookmarkNode* parent) :
	expanded(false),
	m_parent(parent),
	m_type(type)
{
	if (parent)
		parent->add(this);
}

BookmarkNode::~BookmarkNode()
{
	if (m_parent)
		m_parent->remove(this);

	qDeleteAll(m_children);
	m_parent = nullptr;
	m_type = BookmarkNode::Root;
}

void BookmarkNode::setType(Type type)
{
	m_type = type;
}

void BookmarkNode::add(BookmarkNode* child, int offset)
{
	Q_ASSERT(child->m_type != Root);

	if (child->m_parent)
		child->m_parent->remove(child);

	child->m_parent = this;

	if (offset == -1)
		offset = m_children.size();

	m_children.insert(offset, child);
}

void BookmarkNode::remove(BookmarkNode* child)
{
	child->m_parent = nullptr;
	m_children.removeAll(child);
}

bool BookmarkNode::operator==(const BookmarkNode& other)
{
	if (url != other.url || title != other.title || descritption != other.descritption || expanded != other.expanded
		|| m_type != other.m_type || m_children.count() != other.m_children.count())
		return false;

	for (int i{0}; i < m_children.count(); ++i)
		if (!((*(m_children[i]) == (*(other.m_children[i])))))
			return false;

	return true;
}
}