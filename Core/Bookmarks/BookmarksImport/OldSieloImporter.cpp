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

#include "OldSieloImporter.hpp"

#include <QFileDialog>
#include <QDir>

#include "Bookmarks/BookmarkItem.hpp"

namespace Sn
{
OldSieloImporter::OldSieloImporter(QObject* parent):
	BookmarksImporter(parent)
{
	// Empty
}

OldSieloImporter::~OldSieloImporter()
{
	delete m_reader;
}

QString OldSieloImporter::description() const
{
	return tr("Old version of Sielo stores its bookmarks in <b>Bookmarks</b> text file. "
		"This file is usually located in");
}

QString OldSieloImporter::standardPath() const
{
#ifdef Q_OS_WIN
	return QString("%APPDATA%\\Feldrise\\Sielo");
#else
	return QDir::homePath() + QLatin1String("/.locale/share/Feldrise/Sielo/");
#endif
}

QString OldSieloImporter::getPath(QWidget* parent)
{
	m_path = QFileDialog::getOpenFileName(parent, tr("Choose file..."), standardPath(), "Bookmarks (bookmarks.xbel)");
	return m_path;
}

bool OldSieloImporter::prepareImport()
{
	m_file.setFileName(m_path);

	if (!m_file.open(QFile::ReadOnly)) {
		setError(tr("Unable to open file."));
		return false;
	}

	return true;
}

BookmarkItem *OldSieloImporter::importBookmarks()
{
	m_reader = new QXmlStreamReader(&m_file);

	BookmarkItem* root{ new BookmarkItem(BookmarkItem::Folder) };
	root->setTitle("Sielo Open Beta Import");

	if (m_reader->readNextStartElement()) {
		QString version{ m_reader->attributes().value(QLatin1String("version")).toString()};
		if (m_reader->name() == QLatin1String("xbel") && (version.isEmpty() || version == QLatin1String("1.0"))) 
			readXBEL(root);
		else {
			setError(tr("The file is not an XBEL version 1.0 file."));
			return nullptr;
		}
	}

	return root;
}

void OldSieloImporter::readXBEL(BookmarkItem* root)
{
	Q_ASSERT(m_reader->isStartElement() && m_reader->name() == QLatin1String("xbel"));

	while (m_reader->readNextStartElement()) {
		if (m_reader->name() == QLatin1String("folder"))
			readFolder(root);
		else if (m_reader->name() == QLatin1String("bookmark"))
			readBookmarkNode(root);
		else if (m_reader->name() == QLatin1String("separator"))
			readSeparator(root);
		else
			m_reader->skipCurrentElement();
	}
}
void OldSieloImporter::readTitle(BookmarkItem* parent)
{
	Q_ASSERT(m_reader->isStartElement() && m_reader->name() == QLatin1String("title"));

	parent->setTitle(m_reader->readElementText());
}
void OldSieloImporter::readDescription(BookmarkItem* parent)
{
	Q_ASSERT(m_reader->isStartElement() && m_reader->name() == QLatin1String("description"));

	parent->setDescription(m_reader->readElementText());
}
void OldSieloImporter::readSeparator(BookmarkItem* parent)
{
	new BookmarkItem(BookmarkItem::Separator, parent);

	m_reader->readNext();
}
void OldSieloImporter::readFolder(BookmarkItem* parent)
{
	Q_ASSERT(m_reader->isStartElement() && m_reader->name() == QLatin1String("folder"));

	BookmarkItem* folder{ new BookmarkItem(BookmarkItem::Folder, parent) };
	folder->setExpanded(m_reader->attributes().value(QLatin1String("folded")) == QLatin1String("no"));

	while (m_reader->readNextStartElement()) {
		if (m_reader->name() == QLatin1String("title"))
			readTitle(folder);
		else if (m_reader->name() == QLatin1String("description"))
			readDescription(folder);
		else if (m_reader->name() == QLatin1String("folder"))
			readFolder(folder);
		else if (m_reader->name() == QLatin1String("bookmark"))
			readBookmarkNode(folder);
		else if (m_reader->name() == QLatin1String("separator"))
			readSeparator(folder);
		else
			m_reader->skipCurrentElement();
	}
}
void OldSieloImporter::readBookmarkNode(BookmarkItem* parent)
{
	Q_ASSERT(m_reader->isStartElement() && m_reader->name() == QLatin1String("bookmark"));

	BookmarkItem* bookmark{ new BookmarkItem(BookmarkItem::Url, parent) };
	bookmark->setUrl(m_reader->attributes().value(QLatin1String("href")).toString());

	while (m_reader->readNextStartElement()) {
		if (m_reader->name() == QLatin1String("title"))
			readTitle(bookmark);
		else if (m_reader->name() == QLatin1String("description"))
			readDescription(bookmark);
		else m_reader->skipCurrentElement();
	}

	if (bookmark->title().isEmpty())
		bookmark->setTitle(tr("Unknown title"));
}
}
