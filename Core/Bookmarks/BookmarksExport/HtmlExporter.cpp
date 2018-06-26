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

#include "HtmlExporter.hpp"

#include <QFileDialog>
#include <QDir>

#include "Bookmarks/BookmarkItem.hpp"

namespace Sn
{
HtmlExporter::HtmlExporter(QObject* parent) :
	BookmarksExporter(parent)
{
	// Empty
}

HtmlExporter::~HtmlExporter()
{
	// Empty
}

QString HtmlExporter::name() const
{
	return tr("HTML File") + QLatin1String(" (bookmarks.html)");
}

QString HtmlExporter::getPath(QWidget* parent)
{
	const QString defaultPath{QDir::homePath() + QLatin1String("/bookmarks.html")};
	const QString filter{BookmarksExporter::tr("HTML Bookmarks") + QLatin1String(" (.html)")};
	m_path = QFileDialog::getSaveFileName(parent, tr("Choose file..."), defaultPath, filter);

	return m_path;
}

bool HtmlExporter::exportBookmarks(BookmarkItem* root)
{
	QFile file(m_path);

	if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
		setError(tr("Cannot open file for writing!"));
		return false;
	}

	QTextStream stream{&file};
	stream.setCodec("UTF-8");

	stream << "<!DOCTYPE NETSCAPE-Bookmark-file-1>" << endl;
	stream << "<!-- This is an automatically generated file." << endl;
	stream << "     It will be read and overwritten." << endl;
	stream << "     DO NOT EDIT! -->" << endl;
	stream << "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=UTF-8\">" << endl;
	stream << "<TITLE>Bookmarks</TITLE>" << endl;
	stream << "<H1>Bookmarks</H1>" << endl;

	writeBookmark(root, stream, 0);

	return true;
}

void HtmlExporter::writeBookmark(BookmarkItem* item, QTextStream& stream, int level)
{
	Q_ASSERT(item);

	QString indent{};
	indent.fill(QLatin1Char(' '), level * 4);

	switch (item->type()) {
	case BookmarkItem::Url:
		stream << indent << "<DT><A HREF=\"" << item->urlString() << "\">" << item->title() << "</A>" << endl;
		break;
	case BookmarkItem::Separator:
		stream << indent << "<HR>" << endl;
		break;
	case BookmarkItem::Folder:
		stream << indent << "<DT><H3>" << item->title() << "</H3>" << endl;
		stream << indent << "<DL><p>" << endl;

		foreach(BookmarkItem* child, item->children())
			writeBookmark(child, stream, level + 1);

		stream << indent << "</DL><p>" << endl;
		break;
	case BookmarkItem::Root:
		stream << indent << "<DL><p>" << endl;

		foreach(BookmarkItem* child, item->children())
			writeBookmark(child, stream, level + 1);

		stream << indent << "</DL><p>" << endl;
		break;
	default:
		break;
	}
}
}
