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

#pragma once
#ifndef SIELOBROWSER_ADDRESSBARCOMPLETERMODEL_HPP
#define SIELOBROWSER_ADDRESSBARCOMPLETERMODEL_HPP

#include "SharedDefines.hpp"

#include <QStandardItemModel>

#include <QUrl>

#include <QSqlQuery>

#include "Database/SqlDatabase.hpp"

namespace Sn
{
class SIELO_SHAREDLIB AddressBarCompleterModel: public QStandardItemModel {
public:
	enum Role {
		IdRole = Qt::UserRole + 1,
		TitleRole,
		UrlRole,
		CountRole,
		BookmarkRole,
		BookmarkItemRole,
		SearchStringRole,
		TabPositionTabsSpaceRole,
		TabPositionTabRole,
		ImageRole,
		VisitSearchItemRole,
		SearchSuggestionRole
	};

	AddressBarCompleterModel(QObject* parent = nullptr);

	void setCompletions(const QList<QStandardItem*>& items);
	void addCompletions(const QList<QStandardItem*>& items);

	QList<QStandardItem*> suggestionItems() const;

	static QSqlQuery createHistoryQuery(const QString &searchString, int limit, bool exactMatch = false);
	static QSqlQuery createDomainQuery(const QString &text);

private:
	enum Type {
		HistoryAndBookmarks = 0,
		History = 1,
		Bookmarks = 2,
		Nothing = 4
	};

	void setTabPosition(QStandardItem* item) const;
	void refreshTabPositions() const;
};
}

#endif //SIELOBROWSER_ADDRESSBARCOMPLETERMODEL_HPP
