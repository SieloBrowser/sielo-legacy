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

#include "AddressBarCompleterRefreshJob.hpp"

#include <QtConcurrent/QtConcurrentRun>

#include <QDateTime>

#include <algorithm>

#include <ndb/query.hpp>

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarkItem.hpp"

#include "History/History.hpp"

#include "Utils/IconProvider.hpp"

#include "Widgets/AddressBar/AddressBarCompleterModel.hpp"

#include "Application.hpp"

namespace Sn
{
AddressBarCompleterRefreshJob::AddressBarCompleterRefreshJob(const QString& searchString):
	QObject(),
	m_searchString(searchString),
	m_timestamp(QDateTime::currentMSecsSinceEpoch())
{
	m_watcher = new QFutureWatcher<void>(this);
	connect(m_watcher, &QFutureWatcher<void>::finished, this, &AddressBarCompleterRefreshJob::slotFinished);

	QFuture<void> future = QtConcurrent::run(this, &AddressBarCompleterRefreshJob::runJob);
	m_watcher->setFuture(future);
}

void AddressBarCompleterRefreshJob::slotFinished()
{
	emit finished();
}

void AddressBarCompleterRefreshJob::jobCancelled()
{
	m_jobCancelled = true;
}

void AddressBarCompleterRefreshJob::runJob()
{
	if (m_jobCancelled || Application::instance()->isClosing() || !Application::instance())
		return;

	if (m_searchString.isEmpty())
		completeMostVisited();
	else
		completeFromHistory();

	foreach(QStandardItem* item, m_items) {
		if (m_jobCancelled)
			return;

		const QUrl url = item->data(AddressBarCompleterModel::UrlRole).toUrl();
		item->setData(IconProvider::imageForUrl(url), AddressBarCompleterModel::ImageRole);
	}

	if (m_jobCancelled)
		return;

	if (!m_searchString.isEmpty()) {
		if (!(m_searchString.isEmpty() || m_searchString == QLatin1String("www."))) {
			// TODO: waiting for fix
			//ndb::sqlite_query<dbs::navigation> query = AddressBarCompleterModel::createDomainQuery(m_searchString);

			bool withoutWww{m_searchString.startsWith(QLatin1Char('w')) && !m_searchString.startsWith(QLatin1String("www."))};
			QString queryString = "SELECT " + QString::fromStdString(ndb::name(history.url)) + " FROM " + QString::
				fromStdString(ndb::name(history)) + " WHERE ";

			if (withoutWww)
				queryString.append(
					QString::fromStdString(ndb::name(history.url)) + " NOT LIKE ? AND " + QString::
					fromStdString(ndb::name(history.url)) + " NOT LIKE ? AND ");
			else
				queryString.append(
					QString::fromStdString(ndb::name(history.url)) + " LIKE ? OR " + QString::fromStdString(ndb::name(history.url)) +
					" LIKE ? OR ");

			queryString.append(
				"(" + QString::fromStdString(ndb::name(history.url)) + " LIKE ? OR " + QString::
				fromStdString(ndb::name(history.url)) + " LIKE ?) ORDER BY " + QString::fromStdString(ndb::name(history.date)) +
				" DESC LIMIT 1");

			ndb::sqlite_query<dbs::navigation> query{queryString.toStdString()};

			if (withoutWww) {
				query.bind(QString("http://www.%"));
				query.bind(QString("https://www.%"));
				query.bind(QString("http://%1%").arg(m_searchString));
				query.bind(QString("https://%1%").arg(m_searchString));
			}
			else {
				query.bind(QString("http://%1%").arg(m_searchString));
				query.bind(QString("https://%1%").arg(m_searchString));
				query.bind(QString("http://www.%1%").arg(m_searchString));
				query.bind(QString("https://www.%1%").arg(m_searchString));
			}

			auto result = query.exec();
			if (result.has_result())
				m_domainCompletion = createDomainCompletion(QUrl(result[0][history.url]).host());
		}
	}

	if (m_jobCancelled)
		return;

	if (!m_searchString.isEmpty()) {
		QStandardItem* item{new QStandardItem()};
		item->setText(m_searchString);
		item->setData(m_searchString, AddressBarCompleterModel::UrlRole);
		item->setData(m_searchString, AddressBarCompleterModel::SearchStringRole);
		item->setData(QVariant(true), AddressBarCompleterModel::VisitSearchItemRole);

		if (!m_domainCompletion.isEmpty()) {
			// TODO: icon
			item->setData(Application::getAppIcon("google"), AddressBarCompleterModel::ImageRole);
		}

		m_items.prepend(item);
	}
}

void AddressBarCompleterRefreshJob::completeFromHistory()
{
	QList<QUrl> urlList;
	// TODO: add this settings in preferences
	Type showType = History;

	if (showType == HistoryAndBookmarks || showType == Bookmarks) {
		const int bookmarksLimit = 10;
		QList<BookmarkItem*> bookmarks = Application::instance()->bookmarks()->searchBookmarks(m_searchString, bookmarksLimit);

		foreach(BookmarkItem* bookmark, bookmarks) {
			Q_ASSERT(bookmark->isUrl());

			if (bookmark->keyword() == m_searchString) 
				continue;

			QStandardItem* item = new QStandardItem();
			item->setText(bookmark->url().toEncoded());
			item->setData(-1, AddressBarCompleterModel::IdRole);
			item->setData(bookmark->title(), AddressBarCompleterModel::TitleRole);
			item->setData(bookmark->url(), AddressBarCompleterModel::UrlRole);
			item->setData(bookmark->visitCount(), AddressBarCompleterModel::CountRole);
			item->setData(true, AddressBarCompleterModel::BookmarkRole);
			item->setData(QVariant::fromValue<void*>(static_cast<void*>(bookmark)), AddressBarCompleterModel::BookmarkItemRole);
			item->setData(m_searchString, AddressBarCompleterModel::SearchStringRole);

			urlList.append(bookmark->url());
			m_items.append(item);
		}
	}

	std::sort(m_items.begin(), m_items.end(), [](const QStandardItem* item1, const QStandardItem* item2) {
		int i1Count{item1->data(AddressBarCompleterModel::CountRole).toInt()};
		int i2Count{item2->data(AddressBarCompleterModel::CountRole).toInt()};

		return i1Count > i2Count;
	});

	if (showType == HistoryAndBookmarks || showType == History) {
		const int historyLimit{20};

		// TODO: waiting for fix
		//ndb::sqlite_query<dbs::navigation>& query = AddressBarCompleterModel::createHistoryQuery(m_searchString, historyLimit);

		QStringList searchList;
		// TODO: Use ndb database name methode when it will be ok
		QString queryString = QString(
			"SELECT * FROM " + QString::fromStdString(ndb::name(history)) + " WHERE ");

		searchList = m_searchString.split(QLatin1Char(' '), QString::SkipEmptyParts);
		const int slSize = searchList.size();
		for (int i = 0; i < slSize; ++i) {
			queryString.append(
				"(" + QString::fromStdString(ndb::name(history.title)) + " LIKE ? OR " + QString::fromStdString(
					ndb::name(history.url)) + " LIKE ?) ");
			if (i < slSize - 1) {
				queryString.append(QLatin1String("AND "));
			}
		}

		queryString.append("ORDER BY " + QString::fromStdString(ndb::name(history.date)) + " DESC LIMIT ?");

		ndb::sqlite_query<dbs::navigation> query{queryString.toStdString()};

		foreach(const QString &str, searchList) {
			QString bind = QString("%%1%").arg(str);
			query.bind(QString("%%1%").arg(str));
			query.bind(QString("%%1%").arg(str));
		}

		query.bind(historyLimit);

		for (auto& entry : query.exec<ndb::objects::history>()) {
			const QUrl url{QUrl(entry.url)};

			if (urlList.contains(url))
				continue;

			QStandardItem* item{new QStandardItem()};
			item->setText(url.toEncoded());
			item->setData(entry.id, AddressBarCompleterModel::IdRole);
			item->setData(entry.title, AddressBarCompleterModel::TitleRole);
			item->setData(url, AddressBarCompleterModel::UrlRole);
			item->setData(entry.count, AddressBarCompleterModel::CountRole);
			item->setData(QVariant(false), AddressBarCompleterModel::BookmarkRole);
			item->setData(m_searchString, AddressBarCompleterModel::SearchStringRole);

			m_items.append(item);
		}
	}
}

void AddressBarCompleterRefreshJob::completeMostVisited()
{
	for (auto& entry : ndb::oquery<dbs::navigation>() << (ndb::sort(ndb::desc(history.count)) << ndb::limit(15))) {
		QStandardItem* item{new QStandardItem()};
		const QUrl url{QUrl(entry.url)};

		item->setText(url.toEncoded());
		item->setData(entry.id, AddressBarCompleterModel::IdRole);
		item->setData(entry.title, AddressBarCompleterModel::TitleRole);
		item->setData(url, AddressBarCompleterModel::UrlRole);
		item->setData(QVariant(false), AddressBarCompleterModel::BookmarkRole);

		m_items.append(item);
	}
}

QString AddressBarCompleterRefreshJob::createDomainCompletion(const QString& completion) const
{
	if (m_searchString.startsWith(QLatin1String("www.")) && !completion.startsWith(QLatin1String("www.")))
		return QLatin1String("www.") + completion;

	if (!m_searchString.startsWith(QLatin1String("www.")) && completion.startsWith(QLatin1String("www.")))
		return completion.mid(4);

	return completion;
}
}
