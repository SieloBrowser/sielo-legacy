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

#include "History.hpp"

#include <QSqlQuery>

#include "Utils/Settings.hpp"

#include "Web/WebView.hpp"

#include "History/HistoryModel.hpp"

namespace Sn
{
QString History::titleCaseLocalizedMonth(int month)
{
	switch (month) {
	case 1:
		return tr("January");
	case 2:
		return tr("February");
	case 3:
		return tr("March");
	case 4:
		return tr("April");
	case 5:
		return tr("May");
	case 6:
		return tr("June");
	case 7:
		return tr("July");
	case 8:
		return tr("August");
	case 9:
		return tr("September");
	case 10:
		return tr("October");
	case 11:
		return tr("November");
	case 12:
		return tr("December");
	default:
		qWarning("Month number out of range!");
		return QString();
	}
}

History::History(QObject* parent) :
	QObject(parent)
{
	loadSettings();
}

History::~History()
{
	// Empty
}

HistoryModel *History::model()
{
	if (!m_model)
		m_model = new HistoryModel(this);

	return m_model;
}

void History::addHistoryEntry(WebView* view)
{
	if (!m_isSaving)
		return;

	const QUrl url{view->url()};
	const QString title{view->title()};

	addHistoryEntry(url, title);
}

void History::addHistoryEntry(const QUrl& url, QString title)
{
	if (!m_isSaving)
		return;

	const QStringList ignoredSchemes = {
		QStringLiteral("sielo"),
		QStringLiteral("view-source"),
		QStringLiteral("file"),
		QStringLiteral("data")
	};

	if (url.isEmpty() || ignoredSchemes.contains(url.scheme()))
		return;

	if (title.isEmpty())
		title = tr("Empty page");

	QSqlQuery query(SqlDatabase::instance()->database());
	query.prepare("SELECT id, count, date, title FROM history WHERE url=?");
	query.bindValue(0, url);
	query.exec();

	if (!query.next()) {
		query.prepare("INSERT INTO history (count, date, url, title) VALUES (1,?,?,?)");
		query.bindValue(0, QDateTime::currentMSecsSinceEpoch());
		query.bindValue(1, url);
		query.bindValue(2, title);
		query.exec();

		int id = query.lastInsertId().toInt();
		HistoryEntry entry;
		entry.id = id;
		entry.count = 1;
		entry.date = QDateTime::currentDateTime();
		entry.url = url;
		entry.urlString = url.toEncoded();
		entry.title = title;
		emit historyEntryAdded(entry);
	}
	else {
		int id = query.value(0).toInt();
		int count = query.value(1).toInt();
		QDateTime date = QDateTime::fromMSecsSinceEpoch(query.value(2).toLongLong());
		QString oldTitle = query.value(3).toString();

		query.prepare("UPDATE history SET count = count + 1, date=?, title=? WHERE url=?");
		query.bindValue(0, QDateTime::currentMSecsSinceEpoch());
		query.bindValue(1, title);
		query.bindValue(2, url);
		query.exec();

		HistoryEntry before;
		before.id = id;
		before.count = count;
		before.date = date;
		before.url = url;
		before.urlString = url.toEncoded();
		before.title = oldTitle;

		HistoryEntry after = before;
		after.count = count + 1;
		after.date = QDateTime::currentDateTime();
		after.title = title;

		emit historyEntryEdited(before, after);
	}
}

void History::deleteHistoryEntry(int index)
{
	QList<int> list{};
	list.append(index);

	deleteHistoryEntry(list);
}

void History::deleteHistoryEntry(const QList<int>& list)
{
	QSqlDatabase db = SqlDatabase::instance()->database();
	db.transaction();

	QList<QUrl> urls;

	for (int index : list) {
		QSqlQuery query{SqlDatabase::instance()->database()};
		query.prepare("SELECT count, date, url, title FROM history WHERE id=?");
		query.addBindValue(index);
		query.exec();

		if (!query.isActive() || !query.next())
			continue;

		HistoryEntry entry{};
		entry.id = index;
		entry.count = query.value(0).toInt();
		entry.date = QDateTime::fromMSecsSinceEpoch(query.value(1).toLongLong());
		entry.url = query.value(2).toUrl();
		entry.urlString = entry.url.toEncoded();
		entry.title = query.value(3).toString();

		query.prepare("DELETE FROM history WHERE id=?");
		query.addBindValue(index);
		query.exec();

		query.prepare("DELETE FROM icons WHERE url=?");
		query.addBindValue(entry.url.toEncoded(QUrl::RemoveFragment));
		query.exec();

		urls.append(entry.url);
		emit historyEntryDeleted(entry);
	}

	Application::instance()->webProfile()->clearVisitedLinks(urls);

	db.commit();
}

void History::deleteHistoryEntry(const QString& url, const QString& title)
{
	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT id FROM history WHERE url=? AND title=?");
	query.bindValue(0, url);
	query.bindValue(1, title);
	query.exec();

	if (query.next()) {
		const int id{query.value(0).toInt()};

		deleteHistoryEntry(id);
	}
}

QList<int> History::indexesFromTimeRange(qint64 start, qint64 end)
{
	QList<int> list{};

	if (start < 0 || end < 0)
		return list;

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT id FROM history WHERE date BETWEEN ? AND ?");
	query.addBindValue(end);
	query.addBindValue(start);
	query.exec();

	while (query.next())
		list.append(query.value(0).toInt());

	return list;
}

bool History::urlIsStored(const QString& url)
{
	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT id FROM history WHERE url=?");
	query.bindValue(0, url);
	query.exec();

	return query.next();
}

QVector<History::HistoryEntry> History::mostVisited(int count)
{
	QVector<HistoryEntry> list;

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare(QString("SELECT count, date, id, title, url FROM history ORDER BY count DESC LIMIT %1").arg(count));
	query.exec();

	while (query.next()) {
		HistoryEntry entry{};
		entry.count = query.value(0).toInt();
		entry.date = query.value(1).toDateTime();
		entry.id = query.value(2).toInt();
		entry.title = query.value(3).toString();
		entry.url = query.value(4).toUrl();
		list.append(entry);
	}

	return list;
}

void History::clearHistory()
{
	QSqlQuery query{SqlDatabase::instance()->database()};
	query.exec("DELETE FROM history");
	query.exec("VACUUM");

	Application::instance()->webProfile()->clearAllVisitedLinks();

	emit resetHistory();
}

void History::setSaving(bool state)
{
	m_isSaving = state;
}

void History::loadSettings()
{
	Settings settings{};

	settings.beginGroup("Web-Settings");

	m_isSaving = settings.value("allowHistory", true).toBool();

	settings.endGroup();
}
}
