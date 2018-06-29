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

#include <QSettings>

#include <ndb/query.hpp>
#include <ndb/function.hpp>

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

	auto oquery = ndb::oquery<dbs::navigation>() << (history.url == url.toString());

	if (!oquery.has_result()) {
		ndb::query<dbs::navigation>() + (
			history.title = title,
			history.url = url.toString(),
			history.date = QDateTime::currentMSecsSinceEpoch(),
			history.count = 1
		);

		qint64 id = ndb::last_id<dbs::navigation>();

		HistoryEntry entry{};
		entry.id = id;
		entry.count = 1;
		entry.date = QDateTime::currentDateTime();
		entry.url = url;
		entry.urlString = url.toEncoded();
		entry.title = title;

		emit historyEntryAdded(entry);
	}
	else {
		qint64 id{oquery[0].id};
		qint64 count{oquery[0].count};
		QDateTime date{QDateTime::fromMSecsSinceEpoch(oquery[0].date)};
		QString oldTitle{oquery[0].title};

		ndb::query<dbs::navigation>() >> (
			(
				history.count = count + 1,
				history.date = QDateTime::currentMSecsSinceEpoch(),
				history.title = title
			)
			<< (history.url == url.toString())
		);

		HistoryEntry before{};
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
	for (int index : list) {
		auto query = ndb::oquery<dbs::navigation>() << (history.id == index);

		if (!query.has_result())
			return;

		HistoryEntry entry{query[0]};

		ndb::query<dbs::navigation>() - (history.id == index);
		ndb::query<dbs::navigation>() - (history.url == QString::fromUtf8(entry.url.toEncoded(QUrl::RemoveFragment)));

		emit historyEntryDeleted(entry);
	}
}

void History::deleteHistoryEntry(const QString& url, const QString& title)
{
	auto query = ndb::query<dbs::navigation>() << ((history.id) << (history.url == url && history.title ==
		title));

	if (query.has_result()) {
		int id = query[0][history.id];
		deleteHistoryEntry(id);
	}
}

QList<int> History::indexesFromTimeRange(qint64 start, qint64 end)
{
	QList<int> list{};

	if (start < 0 || end < 0)
		return list;

	for (auto& data : ndb::query<dbs::navigation>() << ((history.id) << ndb::range(
		     history.date, end, start)))
		list.append(data[history.id]);

	return list;
}

bool History::urlIsStored(const QString& url)
{
	auto query = ndb::query<dbs::navigation>() << (history.url == url);

	return query.has_result();
}

QVector<History::HistoryEntry> History::mostVisited(int count)
{
	QVector<HistoryEntry> list{};

	//auto oquery = ndb::oquery<dbs::navigation>() << ((ndb::sort(ndb::desc(history.count)) << ndb::limit(count)));

	//for (auto& data : oquery) {
	//	HistoryEntry entry{data};

	//	list.append(entry);
	//}

	for (auto& data : ndb::query<dbs::navigation>() << ((history.id, history.url, history.title, history.count, history.date) << (ndb::sort(ndb::desc(history.count)) << ndb::limit(count)))) {
		HistoryEntry entry{};

		entry.id = data[history.id];
		entry.date = QDateTime::fromMSecsSinceEpoch(data[history.date]);
		entry.url = QUrl(data[history.url]);
		entry.urlString = QUrl(data[history.url]).toEncoded();
		entry.title = data[history.title];
		
		list.append(entry);
	}

	return list;
}

void History::clearHistory()
{
	ndb::clear<dbs::navigation>(history);

	Application::instance()->webProfile()->clearAllVisitedLinks();

	emit resetHistory();
}

void History::setSaving(bool state)
{
	m_isSaving = state;
}

void History::loadSettings()
{
	QSettings settings{};

	settings.beginGroup("Web-Settings");

	m_isSaving = settings.value("allowHistory", true).toBool();

	settings.endGroup();
}
}
