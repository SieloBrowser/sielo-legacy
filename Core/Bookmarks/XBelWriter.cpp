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

#include "Bookmarks/XBelWriter.hpp"


#include "Bookmarks/BookmarkNode.hpp"

namespace Sn {
XBelWriter::XBelWriter()
{
	setAutoFormatting(true);
}

bool XBelWriter::write(const QString& fileName, const BookmarkNode* root)
{
	QFile file{fileName};

	if (!root || !file.open(QFile::WriteOnly))
		return false;

	return write(&file, root);
}

bool XBelWriter::write(QIODevice* device, const BookmarkNode* root)
{
	setDevice(device);

	writeStartDocument();
	writeDTD(QLatin1String("<!DOCTYPE xbel>"));
	writeStartElement(QLatin1String("xbel"));
	writeAttribute(QLatin1String("version"), QLatin1String("1.0"));

	if (root->type() == BookmarkNode::Root) {
		for (auto& child : root->children())
			writeItem(child);
	}
	else
		writeItem(root);

	writeEndDocument();
	return true;
}

void XBelWriter::writeItem(const BookmarkNode* parent)
{
	switch (parent->type()) {
	case BookmarkNode::Folder:
		writeStartElement(QLatin1String("folder"));
		writeAttribute(QLatin1String("folded"), parent->expanded ? QLatin1String("no") : QLatin1String("yes"));
		writeTextElement(QLatin1String("title"), parent->title);

		for (auto& child : parent->children())
			writeItem(child);

		writeEndElement();
		break;
	case BookmarkNode::Bookmark:
		writeStartElement(QLatin1String("bookmark"));

		if (!parent->url.isEmpty())
			writeAttribute(QLatin1String("href"), parent->url);

		writeTextElement(QLatin1String("title"), parent->title);

		if (!parent->descritption.isEmpty())
			writeAttribute(QLatin1String("description"), parent->descritption);

		writeEndElement();
		break;
	case BookmarkNode::Separator:
		writeEmptyElement(QLatin1String("separator"));
		break;
	default:
		break;
	}
}
}