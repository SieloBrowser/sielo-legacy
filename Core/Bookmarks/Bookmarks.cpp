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

#include "Bookmarks.hpp"

#include <QSaveFile>

#include <QJsonParseError>
#include <QJsonDocument>

#include "Utils/AutoSaver.hpp"
#include "Utils/DataPaths.hpp"
#include "Utils/Settings.hpp"

#include "Bookmarks/BookmarkItem.hpp"
#include "Bookmarks/BookmarksModel.hpp"

#include "Application.hpp"

namespace Sn
{
Bookmarks::Bookmarks(QObject* parent) :
	QObject(parent),
	m_autoSaver(new AutoSaver(this))
{
	m_root = new BookmarkItem(BookmarkItem::Root);

	m_folderToolbar = new BookmarkItem(BookmarkItem::Folder, m_root);
	m_folderToolbar->setTitle(tr("Bookmarks Toolbar"));
	m_folderToolbar->setDescription(tr("Bookmarks located in Bookmarks Toolbar"));

	m_folderMenu = new BookmarkItem(BookmarkItem::Folder, m_root);
	m_folderMenu->setTitle(tr("Bookmarks menu"));
	m_folderToolbar->setDescription(tr("Bookmarks located in Bookmarks Menu"));

	m_folderUnsorted = new BookmarkItem(BookmarkItem::Folder, m_root);
	m_folderUnsorted->setTitle(tr("Unsorted Bookmarks"));
	m_folderUnsorted->setDescription(tr("All other Bookmarks"));

	loadBookmarks();

	m_lastFolder = m_folderUnsorted;
	m_model = new BookmarksModel(m_root, this, this);

	loadSettings();
}

Bookmarks::~Bookmarks()
{
	m_autoSaver->saveIfNeccessary();
	delete m_root;
}

void Bookmarks::loadSettings()
{
	Settings settings{};

	settings.beginGroup("Bookmarks");

	m_showOnlyIconsInToolbar = settings.value("showOnlyIconsInToolbar", false).toBool();
	m_showOnlyTextInToolbar = settings.value("showOnlyTextInToolbar", false).toBool();

	settings.endGroup();
}

bool Bookmarks::isBookmarked(const QUrl& url)
{
	return !searchBookmarks(url).isEmpty();
}

bool Bookmarks::canBeModified(BookmarkItem* item) const
{
	Q_ASSERT(item);

	return item != m_root &&
		item != m_folderToolbar &&
		item != m_folderMenu &&
		item != m_folderUnsorted;
}

QList<BookmarkItem*> Bookmarks::searchBookmarks(const QUrl& url) const
{
	QList<BookmarkItem*> items{};

	search(&items, m_root, url);

	return items;
}

QList<BookmarkItem*> Bookmarks::
searchBookmarks(const QString& string, int limit, Qt::CaseSensitivity sensitive) const
{
	QList<BookmarkItem*> items{};

	search(&items, m_root, string, limit, sensitive);

	return items;
}

QList<BookmarkItem*> Bookmarks::searchKeyword(const QString& keyword) const
{
	QList<BookmarkItem*> items{};

	searchKeyword(&items, m_root, keyword);

	return items;
}

void Bookmarks::addBookmark(BookmarkItem* parent, BookmarkItem* item)
{
	Q_ASSERT(parent);
	Q_ASSERT(parent->isFolder());
	Q_ASSERT(item);

	insertBookmark(parent, parent->children().count(), item);
}

void Bookmarks::insertBookmark(BookmarkItem* parent, int row, BookmarkItem* item)
{
	Q_ASSERT(parent);
	Q_ASSERT(parent->isFolder());
	Q_ASSERT(item);

	m_lastFolder = parent;
	m_model->addBookmark(parent, row, item);

	emit bookmarkAdded(item);

	m_autoSaver->changeOccurred();
}

bool Bookmarks::removeBookmark(BookmarkItem* item)
{
	if (!canBeModified(item))
		return false;

	m_model->removeBookmark(item);

	emit bookmarkRemoved(item);

	m_autoSaver->changeOccurred();

	return true;
}

void Bookmarks::changeBookmark(BookmarkItem* item)
{
	Q_ASSERT(item);

	emit bookmarkChanged(item);

	m_autoSaver->changeOccurred();
}

void Bookmarks::save()
{
	Settings settings{};

	settings.beginGroup("Bookmarks");

	settings.setValue("showOnlyIconsInToolbar", m_showOnlyIconsInToolbar);
	settings.setValue("showOnlyTextInToolbar", m_showOnlyTextInToolbar);

	settings.endGroup();

	saveBookmarks();
}

void Bookmarks::setShowOnlyIconsInToolbar(bool state)
{
	m_showOnlyIconsInToolbar = state;
	emit showOnlyIconsInToolbarChanged(state);
	m_autoSaver->changeOccurred();
}

void Bookmarks::setShowOnlyTextInToolbar(bool state)
{
	m_showOnlyTextInToolbar = state;
	emit showOnlyTextInToolbarChanged(state);
	m_autoSaver->changeOccurred();
}

void Bookmarks::loadBookmarks()
{
	const QString bookmarksFile{DataPaths::currentProfilePath() + QLatin1String("/bookmarks.json")};
	const QString backupFile{bookmarksFile + QLatin1String(".old")};
	QJsonParseError error{};
	QJsonDocument json = QJsonDocument::fromJson(Application::readAllFileByteContents(bookmarksFile), &error);
	const QVariant res = json.toVariant();

	if (error.error != QJsonParseError::NoError || res.type() != QVariant::Map) {
		if (QFile(bookmarksFile).exists()) {
			qWarning() << "Bookmarks::init() Error parsing bookmarks! Using default bookmarks!";
			qWarning() << "Bookmarks::init() Your bookmarks have been backed up in" << backupFile;

			// Backup the user bookmarks
			QFile::remove(backupFile);
			QFile::copy(bookmarksFile, backupFile);
		}

		json = QJsonDocument::fromJson(Application::readAllFileByteContents(QStringLiteral(":data/bookmarks.json")), &error);
		const QVariant data = json.toVariant();

		Q_ASSERT(error.error == QJsonParseError::NoError);
		Q_ASSERT(data.type() == QVariant::Map);

		loadBookmarksFromMap(data.toMap().value("roots").toMap());

		m_autoSaver->changeOccurred();
	}
	else
		loadBookmarksFromMap(res.toMap().value("roots").toMap());
}

void Bookmarks::saveBookmarks()
{
	QVariantMap bookmarksMap{};
	QVariantMap map{};

	QVariantMap toolbarMap{writeFolder(m_folderToolbar)};
	QVariantMap menuMap{writeFolder(m_folderMenu)};
	QVariantMap unsortedMap{writeFolder(m_folderUnsorted)};

	bookmarksMap.insert("bookmarks_bar", toolbarMap);
	bookmarksMap.insert("bookmarks_menu", menuMap);
	bookmarksMap.insert("other", unsortedMap);

	map.insert("version", 1);
	map.insert("roots", bookmarksMap);

	const QJsonDocument json{QJsonDocument::fromVariant(map)};
	const QByteArray data{json.toJson()};

	if (data.isEmpty()) {
		qWarning() << "Bookmarks::saveBookmarks() Error serializing bookmarks";
		return;
	}

	QSaveFile file{DataPaths::currentProfilePath() + QLatin1String("/bookmarks.json")};

	if (!file.open(QFile::WriteOnly))
		qWarning() << "Bookmarks::saveBookmarks() Error opening bookmarks file for writing!";

	file.write(data);
	file.commit();
}

QVariantMap Bookmarks::writeFolder(BookmarkItem* folder)
{
	QVariantMap map{};

	map.insert("children", writeBookmarks(folder));
	map.insert("expanded", folder->isExpanded());
	map.insert("name", folder->title());
	map.insert("description", folder->description());
	map.insert("type", "folder");

	return map;
}

void Bookmarks::readFolder(const QString& name, const QVariantMap& map, BookmarkItem* folder)
{
	readBookmarks(map.value(name).toMap().value("children").toList(), folder);
	folder->setExpanded(map.value(name).toMap().value("expanded").toBool());
}

void Bookmarks::loadBookmarksFromMap(const QVariantMap& map)
{
	readFolder("bookmarks_bar", map, m_folderToolbar);
	readFolder("bookmarks_menu", map, m_folderMenu);
	readFolder("other", map, m_folderUnsorted);
}

void Bookmarks::readBookmarks(const QVariantList& list, BookmarkItem* parent)
{
	Q_ASSERT(parent);

	foreach (const QVariant& entry, list) {
		const QVariantMap map{entry.toMap()};
		BookmarkItem::Type type{BookmarkItem::typeFromString(map.value("type").toString())};

		if (type == BookmarkItem::Invalid)
			continue;

		BookmarkItem* item{new BookmarkItem(type, parent)};

		switch (type) {
		case BookmarkItem::Url:
			item->setUrl(QUrl::fromEncoded(map.value("url").toByteArray()));
			item->setTitle(map.value("name").toString());
			item->setDescription(map.value("description").toString());
			item->setKeyword(map.value("keyword").toString());
			item->setVisitCount(map.value("visit_count").toInt());
			break;
		case BookmarkItem::Folder:
			item->setTitle(map.value("name").toString());
			item->setDescription(map.value("description").toString());
			item->setExpanded(map.value("expanded").toBool());
			break;
		default:
			break;

		}

		if (map.contains("children"))
			readBookmarks(map.value("children").toList(), item);
	}
}

QVariantList Bookmarks::writeBookmarks(BookmarkItem* parent)
{
	Q_ASSERT(parent);

	QVariantList list{};

	foreach(BookmarkItem* child, parent->children()) {
		QVariantMap map{};
		map.insert("type", BookmarkItem::typeToString(child->type()));

		switch (child->type()) {
		case BookmarkItem::Url:
			map.insert("url", child->urlString());
			map.insert("name", child->title());
			map.insert("description", child->description());
			map.insert("keyword", child->keyword());
			map.insert("visit_count", child->visitCount());
			break;
		case BookmarkItem::Folder:
			map.insert("name", child->title());
			map.insert("description", child->description());
			map.insert("expanded", child->isExpanded());
			break;
		default:
			break;
		}

		if (!child->children().isEmpty())
			map.insert("children", writeBookmarks(child));

		list.append(map);
	}

	return list;
}

void Bookmarks::search(QList<BookmarkItem*>* items, BookmarkItem* parent, const QUrl& url) const
{
	Q_ASSERT(items);
	Q_ASSERT(parent);

	switch (parent->type()) {
	case BookmarkItem::Root:
	case BookmarkItem::Folder:
		foreach(BookmarkItem* child, parent->children())
			search(items, child, url);
		break;
	case BookmarkItem::Url:
		if (parent->url() == url)
			items->append(parent);
		break;
	default:
		break;
	}
}

void Bookmarks::search(QList<BookmarkItem*>* items, BookmarkItem* parent, const QString& string, int limit,
                       Qt::CaseSensitivity sensitive) const
{
	Q_ASSERT(items);
	Q_ASSERT(parent);

	if (limit == items->count())
		return;

	switch (parent->type()) {
	case BookmarkItem::Root:
	case BookmarkItem::Folder:
		foreach(BookmarkItem* child, parent->children())
			search(items, child, string, limit, sensitive);
		break;
	case BookmarkItem::Url:
		if (parent->title().contains(string, sensitive) ||
			parent->urlString().contains(string, sensitive) ||
			parent->description().contains(string, sensitive) ||
			parent->keyword().compare(string, sensitive) == 0)
			items->append(parent);
		break;
	default:
		break;
	}
}

void Bookmarks::searchKeyword(QList<BookmarkItem*>* items, BookmarkItem* parent, const QString& keyword) const
{
	Q_ASSERT(items);
	Q_ASSERT(parent);

	switch (parent->type()) {
	case BookmarkItem::Root:
	case BookmarkItem::Folder:
		foreach(BookmarkItem* child, parent->children())
			searchKeyword(items, child, keyword);
		break;
	case BookmarkItem::Url:
		if (parent->keyword() == keyword)
			items->append(parent);
		break;
	default:
		break;
	}
}
}
