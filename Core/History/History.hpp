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
#ifndef SIELOBROWSER_History_HPP
#define SIELOBROWSER_History_HPP

#include "SharedDefines.hpp"

#include <QObject>

#include <QUrl>
#include <QString>
#include <QVector>
#include <QDateTime>

#include "Database/SqlDatabase.hpp"

namespace Sn
{
class WebView;

class HistoryModel;

class SIELO_SHAREDLIB History: public QObject {
Q_OBJECT

public:
	History(QObject* parent);
	~History();

	struct HistoryEntry {
		qint64 id;
		qint64 count;
		QDateTime date;
		QUrl url;
		QString urlString;
		QString title;

		HistoryEntry() {}
	};

	HistoryModel *model();

	void addHistoryEntry(WebView* view);
	void addHistoryEntry(const QUrl& url, QString title);

	void deleteHistoryEntry(int index);
	void deleteHistoryEntry(const QList<int>& list);
	void deleteHistoryEntry(const QString& url, const QString& title);

	QList<int> indexesFromTimeRange(qint64 start, qint64 end);

	bool urlIsStored(const QString& url);

	QVector<HistoryEntry> mostVisited(int count);

	void clearHistory();
	bool isSaving() const { return m_isSaving; }
	void setSaving(bool state);

	void loadSettings();

	static QString titleCaseLocalizedMonth(int month);

signals:
	void historyEntryAdded(const HistoryEntry& entry);
	void historyEntryDeleted(const HistoryEntry& entry);
	void historyEntryEdited(const HistoryEntry& before, const HistoryEntry& after);

	void resetHistory();

private:
	bool m_isSaving{true};

	HistoryModel* m_model{nullptr};
};
}

#endif //SIELOBROWSER_History_HPP
