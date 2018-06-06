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

#include "AddressBarCompleterModel.hpp"

#include <QPixmap>

#include <ndb/utility.hpp>

#include "History/History.hpp"

#include "Web/Tab/WebTab.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

constexpr auto& history = ndb::models::navigation.history;

namespace Sn
{
ndb::sqlite_query<dbs::navigation> AddressBarCompleterModel::createHistoryQuery(
	const QString& searchString, int limit, bool exactMatch)
{
	QStringList searchList;
	// TODO: Use ndb database name methode when it will be ok
	QString queryString = QString(
		"SELECT * FROM " + QString::fromStdString(ndb::name(history)) + " WHERE ");

	if (exactMatch) {
		queryString.append(QLatin1String("title LIKE ? OR url LIKE ? "));
	}
	else {
		searchList = searchString.split(QLatin1Char(' '), QString::SkipEmptyParts);
		const int slSize = searchList.size();
		for (int i = 0; i < slSize; ++i) {
			queryString.append(QLatin1String("(title LIKE ? OR url LIKE ?) "));
			if (i < slSize - 1) {
				queryString.append(QLatin1String("AND "));
			}
		}
	}

	queryString.append(QLatin1String("ORDER BY date DESC LIMIT ?"));

	std::string string = queryString.toStdString();
	ndb::sqlite_query<dbs::navigation> query{string};

	if (exactMatch) {
		query.bind(QString("%%1%").arg(searchString).toStdString());
		query.bind(QString("%%1%").arg(searchString).toStdString());
	}
	else {
		foreach(const QString &str, searchList) {
			query.bind(QString("%%1%").arg(str).toStdString());
			query.bind(QString("%%1%").arg(str).toStdString());
		}
	}

	return query;
}

ndb::sqlite_query<dbs::navigation> AddressBarCompleterModel::createDomainQuery(const QString& text)
{
	bool withoutWww{text.startsWith(QLatin1Char('w')) && !text.startsWith(QLatin1String("www."))};
	QString queryString = "SELECT url FROM history WHERE ";

	if (withoutWww)
		queryString.append(QLatin1String("url NOT LIKE ? AND url NOT LIKE ? AND "));
	else
		queryString.append(QLatin1String("url LIKE ? OR url LIKE ? OR "));


	queryString.append(QLatin1String("(url LIKE ? OR url LIKE ?) ORDER BY date DESC LIMIT 1"));

	ndb::sqlite_query<dbs::navigation> query{queryString.toStdString()};

	if (withoutWww) {
		query.bind(QString("http://www.%").toStdString());
		query.bind(QString("https://www.%").toStdString());
		query.bind(QString("http://%1%").arg(text).toStdString());
		query.bind(QString("https://%1%").arg(text).toStdString());
	}
	else {
		query.bind(QString("http://%1%").arg(text).toStdString());
		query.bind(QString("https://%1%").arg(text).toStdString());
		query.bind(QString("http://www.%1%").arg(text).toStdString());
		query.bind(QString("https://www.%1%").arg(text).toStdString());
	}

	return query;
}

AddressBarCompleterModel::AddressBarCompleterModel(QObject* parent) :
	QStandardItemModel(parent)
{
	// Empty
}

void AddressBarCompleterModel::setCompletions(const QList<QStandardItem*>& items)
{
	clear();
	addCompletions(items);
}

void AddressBarCompleterModel::addCompletions(const QList<QStandardItem*>& items)
{
	foreach (QStandardItem* item, items) {
		item->setIcon(QPixmap::fromImage(item->data(ImageRole).value<QImage>()));
		setTabPosition(item);

		if (item->icon().isNull())
			item->setIcon(Application::getAppIcon("webpage"));

		appendRow(QList<QStandardItem*>{item});
	}
}

QList<QStandardItem*> AddressBarCompleterModel::suggestionItems() const
{
	QList<QStandardItem*> items{};

	for (int i{0}; i < rowCount(); ++i) {
		QStandardItem* it{item(i)};

		if (it->data(SearchSuggestionRole).toBool())
			items.append(it);
	}

	return items;
}

void AddressBarCompleterModel::setTabPosition(QStandardItem* item) const
{
	const QUrl url{item->data(UrlRole).toUrl()};
	const QList<BrowserWindow*> windows = Application::instance()->windows();

	foreach (BrowserWindow* window, windows) {
		QVector<TabWidget*> tabWidgets = window->tabWidgets();
		foreach (TabWidget* tabWidget, tabWidgets) {
			QList<WebTab*> tabs = tabWidget->allTabs();
			for (int i{0}; i < tabs.count(); ++i) {
				WebTab* tab{tabs[i]};

				if (tab->url() == url) {
					item->setData(QVariant::fromValue<void*>(static_cast<void*>(tabWidget)), TabPositionTabsSpaceRole);
					item->setData(i, TabPositionTabRole);
				}
			}
		}
	}
}

void AddressBarCompleterModel::refreshTabPositions() const
{
	for (int row{0}; row < rowCount(); ++row) {
		QStandardItem* itm{item(row)};
		if (itm)
			setTabPosition(itm);
	}
}
}
