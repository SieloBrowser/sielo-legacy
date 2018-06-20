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

#include "BookmarkItem.hpp"

#include <QStyle>

#include "Utils/IconProvider.hpp"

#include "Application.hpp"

namespace Sn
{
BookmarkItem::BookmarkItem(Type type, BookmarkItem* parent) :
	m_type(type),
	m_parent(parent)
{
	if (m_parent)
		parent->addChild(this);
}

BookmarkItem::~BookmarkItem()
{
	qDeleteAll(m_children);
}

void BookmarkItem::setType(Type type)
{
	m_type = type;
}

bool BookmarkItem::isFolder() const
{
	return m_type == Folder;
}

bool BookmarkItem::isUrl() const
{
	return m_type == Url;
}

bool BookmarkItem::isSeparator() const
{
	return m_type == Separator;
}

QIcon BookmarkItem::icon()
{
	const int iconCacheTime{20 * 1000};

	switch (m_type) {
	case Url:
		if (m_iconTime.isNull() || m_iconTime.elapsed() > iconCacheTime) {
			if (m_iconTime.isNull() || m_iconTime.elapsed() > iconCacheTime) {
				m_icon = IconProvider::iconForUrl(m_url);
				m_iconTime.restart();
			}
			return m_icon;
		}
		return m_icon;
	case Folder:
		return QApplication::style()->standardIcon(QStyle::SP_DirIcon);
	default:
		return QIcon();
	}
}

void BookmarkItem::setIcon(const QIcon& icon)
{
	m_icon = icon;
}

QString BookmarkItem::urlString() const
{
	return QString::fromUtf8(m_url.toEncoded());
}

void BookmarkItem::setUrl(const QUrl& url)
{
	m_url = url;
}

void BookmarkItem::setTitle(const QString& title)
{
	m_title = title;
}

void BookmarkItem::setDescription(const QString& description)
{
	m_description = description;
}

void BookmarkItem::setKeyword(const QString& keyword)
{
	m_keyword = keyword;
}

void BookmarkItem::setVisitCount(int count)
{
	m_visitCount = count;
}

void BookmarkItem::updateVisitCount()
{
	++m_visitCount;
}

bool BookmarkItem::isExpanded() const
{
	return m_type == Root ? true : m_expanded;
}

void BookmarkItem::setExpanded(bool expanded)
{
	m_expanded = expanded;
}

void BookmarkItem::addChild(BookmarkItem* child, int index)
{
	if (child->m_parent)
		child->m_parent->removeChild(child);

	child->m_parent = this;

	if (index < 0)
		m_children.append(child);
	else
		m_children.insert(index, child);
}

void BookmarkItem::removeChild(BookmarkItem* child)
{
	child->m_parent = nullptr;
	m_children.removeOne(child);
}

BookmarkItem::Type BookmarkItem::typeFromString(const QString& string)
{
	if (string == QLatin1String("url"))
		return Url;

	if (string == QLatin1String("folder"))
		return Folder;

	if (string == QLatin1String("separator"))
		return Separator;

	return Invalid;
}

QString BookmarkItem::typeToString(BookmarkItem::Type type)
{
	switch (type) {
	case Url:
		return QString("url");

	case Folder:
		return QString("folder");

	case Separator:
		return QString("separator");

	default:
		return QString("invalid");
	}
}
}
