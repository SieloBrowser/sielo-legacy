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

#include "HtmlImporter.hpp"

#include <QFileDialog>

#include "Utils/RegExp.hpp"

#include "Bookmarks/BookmarkItem.hpp"

namespace Sn
{
HtmlImporter::HtmlImporter(QObject* parent):
	BookmarksImporter(parent)
{
	// Empty
}

HtmlImporter::~HtmlImporter()
{
	// Empty
}

QString HtmlImporter::description() const
{
	return tr("You can import bookmarks from any browser that supports HTML exporting. "
		"This file has usually these suffixes");
}

QString HtmlImporter::standardPath() const
{
	return QString(".htm, .html");
}

QString HtmlImporter::getPath(QWidget* parent)
{
	const QString filter{tr("HTML Bookmarks") + QLatin1String(" (*.htm *.html)")};
	m_path = QFileDialog::getOpenFileName(parent, BookmarksImporter::tr("Choose file..."), QDir::homePath(), filter);

	return m_path;
}

bool HtmlImporter::prepareImport()
{
	m_file.setFileName(m_path);

	if (!m_file.open(QFile::ReadOnly)) {
		setError(tr("Unable to open file."));
		return false;
	}

	return true;
}

BookmarkItem *HtmlImporter::importBookmarks()
{
	QString bookmarks = QString::fromUtf8(m_file.readAll());
	m_file.close();

	bookmarks.replace(QLatin1String("<DL"), QLatin1String("<dl"));
	bookmarks.replace(QLatin1String("</DL"), QLatin1String("</dl"));
	bookmarks.replace(QLatin1String("<DT"), QLatin1String("<dt"));
	bookmarks.replace(QLatin1String("</DT"), QLatin1String("</dt"));
	bookmarks.replace(QLatin1String("<P"), QLatin1String("<p"));
	bookmarks.replace(QLatin1String("</P"), QLatin1String("</p"));
	bookmarks.replace(QLatin1String("<A"), QLatin1String("<a"));
	bookmarks.replace(QLatin1String("</A"), QLatin1String("</a"));
	bookmarks.replace(QLatin1String("HREF="), QLatin1String("href="));
	bookmarks.replace(QLatin1String("<H3"), QLatin1String("<h3"));
	bookmarks.replace(QLatin1String("</H3"), QLatin1String("</h3"));

	bookmarks = bookmarks.left(bookmarks.lastIndexOf(QLatin1String("</dl><p>")));
	int start{bookmarks.indexOf(QLatin1String("<dl><p>"))};

	BookmarkItem* root{new BookmarkItem(BookmarkItem::Folder)};
	root->setTitle("HTML Import");

	QList<BookmarkItem*> folders{};
	folders.append(root);

	while (start > 0) {
		QString string{bookmarks.mid(start)};

		int posOfFolder{string.indexOf(QLatin1String("<dt><h3"))};
		int posOfEndFolder{string.indexOf(QLatin1String("</dl><p>"))};
		int posOfLink{string.indexOf(QLatin1String("<dt><a"))};
		int nearest{snMin(posOfLink, snMin(posOfFolder, posOfEndFolder))};

		if (nearest == -1)
			break;

		if (nearest == posOfFolder) {
			RegExp rx("<dt><h3(.*)>(.*)</h3>");
			rx.setMinimal(true);
			rx.indexIn(string);

			QString folderName{rx.capture(2).trimmed()};
			BookmarkItem* folder{new BookmarkItem(BookmarkItem::Folder, folders.isEmpty() ? root : folders.last())};

			folder->setTitle(folderName);
			folders.append(folder);

			start += posOfFolder + rx.capture(0).size();
		}
		else if (nearest == posOfEndFolder) {
			if (!folders.isEmpty())
				folders.removeLast();

			start += posOfEndFolder + 8;
		}
		else {
			RegExp rx{ "<dt><a(.*)>(.*)</a>" };
			rx.setMinimal(true);
			rx.indexIn(string);

			QString arguments{rx.capture(1)};
			QString linkName{rx.capture(2).trimmed()};

			RegExp rx2("href=\"(.*)\"");
			rx2.setMinimal(true);
			rx2.indexIn(arguments);

			QUrl url{QUrl::fromEncoded(rx2.capture(1).trimmed().toUtf8())};

			start += posOfLink + rx.capture(0).size();

			if (url.isEmpty() || url.scheme() == QLatin1String("place") || url.scheme() == QLatin1String("about"))
				continue;

			BookmarkItem* b{new BookmarkItem(BookmarkItem::Url, folders.isEmpty() ? root : folders.last())};
			b->setTitle(linkName.isEmpty() ? url.toString() : linkName);
			b->setUrl(url);
		}
	}

	return root;
}

int HtmlImporter::snMin(int a, int b)
{
	if (a > -1 && b > -1)
		return qMin(a, b);

	if (a > -1)
		return a;

	return b;
}
}
