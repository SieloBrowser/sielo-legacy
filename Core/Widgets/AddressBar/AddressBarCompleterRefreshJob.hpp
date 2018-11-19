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
#ifndef SIELOBROWSER_ADDRESSBARCOMPLETERREFRESHJOB_HPP
#define SIELOBROWSER_ADDRESSBARCOMPLETERREFRESHJOB_HPP

#include "SharedDefines.hpp"

#include <QFutureWatcher>
#include <QStandardItem>

namespace Sn
{
class SIELO_SHAREDLIB AddressBarCompleterRefreshJob: public QObject {
	Q_OBJECT

public:
	AddressBarCompleterRefreshJob(const QString& searchString);

	qint64 timestamp() const { return m_timestamp; }
	QString searchString() const { return m_searchString; };
	bool isCanceled() const { return m_jobCancelled; };

	QList<QStandardItem*> completions() const { return m_items; }
	QString domainCompletion() const { return m_domainCompletion; }

signals:
	void finished();

public slots:
	void slotFinished();
	void jobCancelled();

private:
	enum Type {
		HistoryAndBookmarks = 0,
		History = 1,
		Bookmarks = 2,
		Nothing = 4
	};

	void runJob();
	void completeFromHistory();
	void completeMostVisited();

	QString createDomainCompletion(const QString &completion) const;

	QString m_searchString{};
	QString m_domainCompletion{};
	qint64 m_timestamp{};
	bool m_jobCancelled{ false };

	QList<QStandardItem*> m_items{};
	QFutureWatcher<void>* m_watcher{};
};
}

#endif //SIELOBROWSER_ADDRESSBARCOMPLETERREFRESHJOB_HPP