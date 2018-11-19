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
#ifndef SIELOBROWSER_BOOKMARKITEM_HPP
#define SIELOBROWSER_BOOKMARKITEM_HPP

#include "SharedDefines.hpp"

#include <QString>
#include <QUrl>

#include <QList>

#include <QIcon>
#include <QTime>

namespace Sn
{
class SIELO_SHAREDLIB BookmarkItem {
public:
	enum Type {
		Root,
		Url,
		Folder,
		Separator,
		Invalid
	};

	BookmarkItem(Type type, BookmarkItem* parent = nullptr);
	~BookmarkItem();

	Type type() const { return m_type; }
	void setType(Type type);

	bool isFolder() const;
	bool isUrl() const;
	bool isSeparator() const;

	BookmarkItem *parent() const { return m_parent; }
	QList<BookmarkItem*> children() const { return m_children; }

	QIcon icon();
	void setIcon(const QIcon& icon);

	QString urlString() const;

	QUrl url() const { return m_url; }
	void setUrl(const QUrl& url);

	QString title() const { return m_title; }
	void setTitle(const QString& title);

	QString description() const { return m_description; }
	void setDescription(const QString& description);

	QString keyword() const { return m_keyword; }
	void setKeyword(const QString& keyword);

	int visitCount() const { return m_visitCount; }
	void setVisitCount(int count);

	void updateVisitCount();

	bool isExpanded() const;
	void setExpanded(bool expanded);

	void addChild(BookmarkItem* child, int index = -1);
	void removeChild(BookmarkItem* child);

	static Type typeFromString(const QString& string);
	static QString typeToString(Type type);

private:
	Type m_type{};
	BookmarkItem* m_parent{nullptr};
	QList<BookmarkItem*> m_children{};

	QUrl m_url{};
	QString m_title{};
	QString m_description{};
	QString m_keyword{};

	QIcon m_icon{};
	QTime m_iconTime{};

	int m_visitCount{};
	bool m_expanded{};
};
}

#endif //SIELOBROWSER_BOOKMARKITEM_HPP
