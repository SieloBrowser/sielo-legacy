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

#include "ChromeImporter.hpp"

#include <QDir>
#include <QFileDialog>

#include <QJsonDocument>

#include <QVariant>

#include "Bookmarks/BookmarkItem.hpp"

namespace Sn
{
ChromeImporter::ChromeImporter(QObject* parent) :
	BookmarksImporter(parent)
{
	// Empty
}

ChromeImporter::~ChromeImporter()
{
	// Empty
}

QString ChromeImporter::description() const
{
	return tr("Google Chrome stores its bookmarks in <b>Bookmarks</b> text file. "
		"This file is usually located in");
}

QString ChromeImporter::standardPath() const
{
#ifdef Q_OS_WIN
	return QString("%APPDATA%\\Local\\Google\\Chrome\\User Data\\Default");
#elif defined(Q_OS_OSX)
	return QDir::homePath() + QLatin1String("/Library/Application Support/Google/Chrome/");
#else
	return QDir::homePath() + QLatin1String("/.config/chrome/");
#endif
}

QString ChromeImporter::getPath(QWidget* parent)
{
	m_path = QFileDialog::getOpenFileName(parent, BookmarksImporter::tr("Choose file..."), standardPath(),
	                                      "Bookmarks (Bookmarks)");
	return m_path;
}

bool ChromeImporter::prepareImport()
{
	m_file.setFileName(m_path);

	if (!m_file.open(QFile::ReadOnly)) {
		setError(BookmarksImporter::tr("Unable to open file."));
		return false;
	}

	return true;
}

BookmarkItem *ChromeImporter::importBookmarks()
{
	const QByteArray data{m_file.readAll()};
	m_file.close();

	QJsonParseError err{};
	QJsonDocument json{QJsonDocument::fromJson(data, &err)};
	const QVariant res{json.toVariant()};

	if (err.error != QJsonParseError::NoError || res.type() != QVariant::Map) {
		setError(BookmarksImporter::tr("Cannot parse JSON file!"));
		return nullptr;
	}

	QVariantMap rootMap{res.toMap().value("roots").toMap()};

	BookmarkItem* root{new BookmarkItem(BookmarkItem::Folder)};
	root->setTitle("Chrome Import");

	BookmarkItem* toolbar{new BookmarkItem(BookmarkItem::Folder, root)};
	toolbar->setTitle(rootMap.value("bookmark_bar").toMap().value("name").toString());
	readBookmarks(rootMap.value("bookmark_bar").toMap().value("children").toList(), toolbar);

	BookmarkItem* other{new BookmarkItem(BookmarkItem::Folder, root)};
	other->setTitle(rootMap.value("other").toMap().value("name").toString());
	readBookmarks(rootMap.value("other").toMap().value("children").toList(), other);

	BookmarkItem* synced{new BookmarkItem(BookmarkItem::Folder, root)};
	synced->setTitle(rootMap.value("synced").toMap().value("name").toString());
	readBookmarks(rootMap.value("synced").toMap().value("synced").toList(), other);

	return root;
}

void ChromeImporter::readBookmarks(const QVariantList& list, BookmarkItem* parent)
{
	Q_ASSERT(parent);

	foreach(const QVariant &entry, list) {
		const QVariantMap map{entry.toMap()};
		const QString typeString{map.value("type").toString()};
		BookmarkItem::Type type{};

		if (typeString == QLatin1String("url"))
			type = BookmarkItem::Url;
		else if (typeString == QLatin1String("folder"))
			type = BookmarkItem::Folder;
		else
			continue;

		BookmarkItem* item{new BookmarkItem(type, parent)};
		item->setTitle(map.value("name").toString());

		if (item->isUrl())
			item->setUrl(QUrl::fromEncoded(map.value("url").toByteArray()));

		if (map.contains("children"))
			readBookmarks(map.value("children").toList(), item);
	}
}
}
