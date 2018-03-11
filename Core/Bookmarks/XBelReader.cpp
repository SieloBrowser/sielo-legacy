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

#include "Bookmarks/XBelReader.hpp"

#include <QFile>

#include "Bookmarks/BookmarkNode.hpp"

namespace Sn {
XBelReader::XBelReader()
{
	// Emtpy
}

BookmarkNode* XBelReader::read(const QString& fileName)
{
	QFile file{fileName};

	if (!file.exists())
		return new BookmarkNode(BookmarkNode::Root);

	file.open(QFile::ReadOnly);
	return read(&file);
}

BookmarkNode* XBelReader::read(QIODevice* device)
{
	BookmarkNode* root{new BookmarkNode(BookmarkNode::Root)};

	setDevice(device);

	if (readNextStartElement()) {
		QString version{attributes().value(QLatin1String("version")).toString()};

		if (name() == QLatin1String("xbel") && (version.isEmpty() || version == QLatin1String("1.0")))
			readXBEL(root);
		else
			raiseError(QObject::tr("This file is not an XBEL version 1.0 file"));
	}

	return root;
}

void XBelReader::readXBEL(BookmarkNode* parent)
{
	Q_ASSERT(isStartElement() && name() == QLatin1String("xbel"));

	while (readNextStartElement()) {
		if (name() == QLatin1String("folder"))
			readFolder(parent);
		else if (name() == QLatin1String("bookmark"))
			readBookmarkNode(parent);
		else if (name() == QLatin1String("separator"))
			readSeparator(parent);
		else
			skipCurrentElement();
	}
}

void XBelReader::readTitle(BookmarkNode* parent)
{
	Q_ASSERT(isStartElement() && name() == QLatin1String("title"));
	parent->title = readElementText();
}

void XBelReader::readDescritption(BookmarkNode* parent)
{
	Q_ASSERT(isStartElement() && name() == QLatin1String("description"));
	parent->descritption = readElementText();
}

void XBelReader::readSeparator(BookmarkNode* parent)
{
	new BookmarkNode(BookmarkNode::Separator, parent);
	readNext();
}

void XBelReader::readFolder(BookmarkNode* parent)
{
	Q_ASSERT(isStartElement() && name() == QLatin1String("folder"));

	BookmarkNode* folder{new BookmarkNode(BookmarkNode::Folder, parent)};
	folder->expanded = attributes().value(QLatin1String("folded")) == QLatin1String("no");

	while (readNextStartElement()) {
		if (name() == QLatin1String("title"))
			readTitle(folder);
		else if (name() == QLatin1String("description"))
			readDescritption(folder);
		else if (name() == QLatin1String("folder"))
			readFolder(folder);
		else if (name() == "bookmark")
			readBookmarkNode(folder);
		else if (name() == "separator")
			readSeparator(folder);
		else
			skipCurrentElement();
	}
}

void XBelReader::readBookmarkNode(BookmarkNode* parent)
{
	Q_ASSERT(isStartElement() && name() == QLatin1String("bookmark"));

	BookmarkNode* bookmark{new BookmarkNode(BookmarkNode::Bookmark, parent)};
	bookmark->url = attributes().value(QLatin1String("href")).toString();

	while (readNextStartElement()) {
		if (name() == QLatin1String("title"))
			readTitle(bookmark);
		else if (name() == QLatin1String("description"))
			readDescritption(bookmark);
		else
			skipCurrentElement();
	}

	if (bookmark->title.isEmpty())
		bookmark->title = QObject::tr("Unknown title");
}
}