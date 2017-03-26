/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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
#ifndef SIELOBROWSER_HISTORYMANAGER_HPP
#define SIELOBROWSER_HISTORYMANAGER_HPP

#include <QList>
#include <QString>
#include <QUrl>

#include <QTimer>

namespace Sn {
struct HistoryItem;
class AutoSaver;

class HistoryManager: public QObject {
Q_OBJECT
	Q_PROPERTY(int historyLimit
				   READ
					   historyLimit
				   WRITE
				   setHistoryLimit)

public:
	explicit HistoryManager(QObject* parent = nullptr);
	~HistoryManager();

	bool historyContains(const QString& url) const;

	void addHistoryEntry(const QString& url);
	void removeHistoryEntry(const QString& url);

	void updateHistoryItem(const QUrl& url, const QString& title);

	int historyLimit() const { return m_historyLimit; }
	void setHistoryLimit(int limit);

	QList<HistoryItem>& history()  { return m_history; }
	void setHistory(const QList<HistoryItem>& history, bool loadedAndSorted = false);

	//TODO: Add history models getters (Model, FilterModel and TreeModel)

signals:
	void historyReset();
	void entryAdded(const HistoryItem& item);
	void entryRemoved(const HistoryItem& item);
	void entryUpdate(int offset);

public slots:
	void clear();
	void loadSettings();

protected:
	void addHistoryItem(const HistoryItem& item);
	void removeHistoryItem(const HistoryItem& item);

private slots:
	void save();
	void checkForExpired(bool removeExpiredEntriesDirectly = false);

private:
	void load();

	AutoSaver* m_saveTimer{nullptr};

	int m_historyLimit{30};

	QTimer m_expiredTimer{};
	QList<HistoryItem> m_history{};
	QString m_lastSavedUrl{};

	//TODO: Add history models (Model, FilterModel and TreeModel)

};
}

#endif //SIELOBROWSER_HISTORYMANAGER_HPP
