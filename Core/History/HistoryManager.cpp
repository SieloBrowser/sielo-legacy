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

#include "History/HistoryManager.hpp"

#include <QSettings>

#include <QDateTime>
#include <QAbstractItemModel>

#include <QByteArray>
#include <QIODevice>
#include <QFile>
#include <QDir>
#include <QTemporaryFile>
#include <QDataStream>

#include <QtDebug>
#include <QtCore/QBuffer>

#include "History/HistoryItem.hpp"
#include "History/HistoryModel.hpp"
#include "History/HistoryFilterModel.hpp"
#include "History/HistoryTreeModel.hpp"

#include "Utils/AutoSaver.hpp"

#include "Application.hpp"

namespace Sn {

static const unsigned int HISTORY_VERSION = 17;

HistoryManager::HistoryManager(QObject* parent) :
	QObject(parent),
	m_saveTimer(new AutoSaver(this)),
	m_historyLimit(30),
	m_historyModel(nullptr),
	m_historyFilterModel(nullptr),
	m_historyTreeModel(nullptr)
{
	m_expiredTimer.setSingleShot(true);

	connect(&m_expiredTimer, SIGNAL(timeout()), this, SLOT(checkForExpired()));

	connect(this, &HistoryManager::entryAdded, m_saveTimer, &AutoSaver::changeOccurred);
	connect(this, &HistoryManager::entryRemoved, m_saveTimer, &AutoSaver::changeOccurred);

	load();

	m_historyModel = new HistoryModel(this, this);
	m_historyFilterModel = new HistoryFilterModel(m_historyModel, this);
	m_historyTreeModel = new HistoryTreeModel(m_historyFilterModel, this);
}

HistoryManager::~HistoryManager()
{
	m_saveTimer->saveIfNeccessary();
}

bool HistoryManager::historyContains(const QString& url) const
{
	return m_historyFilterModel->historyContains(url);
}

void HistoryManager::addHistoryEntry(const QString& url)
{
	QUrl cleanUrl{url};

	cleanUrl.setPassword(QString());
	cleanUrl.setHost(cleanUrl.host().toLower());

	HistoryItem item{cleanUrl.toString(), QDateTime::currentDateTime()};

	addHistoryItem(item);
}

void HistoryManager::removeHistoryEntry(const QString& url)
{
	QUrl cleanUrl{url};

	cleanUrl.setPassword(QString());
	cleanUrl.setHost(cleanUrl.host().toLower());

	HistoryItem item{cleanUrl.toString(), QDateTime::currentDateTime()};

	removeHistoryItem(item);
}

void HistoryManager::updateHistoryItem(const QUrl& url, const QString& title)
{
	for (int i{0}; i < m_history.count(); ++i) {
		if (url == m_history[i].url) {
			m_history[i].title = title;
			m_saveTimer->changeOccurred();

			if (m_lastSavedUrl.isEmpty())
				m_lastSavedUrl = m_history[i].url;

			emit entryUpdate(i);
			break;
		}
	}
}

void HistoryManager::setHistoryLimit(int limit)
{
	if (m_historyLimit == limit)
		return;

	m_historyLimit = limit;
	checkForExpired();
	m_saveTimer->changeOccurred();
}

void HistoryManager::setHistory(const QList<HistoryItem>& history, bool loadedAndSorted)
{
	m_history = history;

	if (!loadedAndSorted)
		qSort(m_history.begin(), m_history.end());

	checkForExpired(loadedAndSorted);

	if (loadedAndSorted)
		m_lastSavedUrl = m_history.value(0).url;
	else {
		m_lastSavedUrl = QString();
		m_saveTimer->changeOccurred();
	}

	emit historyReset();
}

void HistoryManager::clear()
{
	m_lastSavedUrl = QString();

	emit historyReset();

	m_saveTimer->changeOccurred();
	m_saveTimer->saveIfNeccessary();
}

void HistoryManager::loadSettings()
{
	QSettings settings{};

	settings.beginGroup(QLatin1String("history"));

	m_historyLimit = settings.value(QLatin1String("historyLimit"), 30).toInt();
}

void HistoryManager::addHistoryItem(const HistoryItem& item)
{
	if (Application::instance()->privateBrowsing())
		return;

	emit entryAdded(item);

	if (m_history.count() == 1)
		checkForExpired();
}

void HistoryManager::removeHistoryItem(const HistoryItem& item)
{
	for (int i{m_history.count() - 1}; i >= 0; --i) {
		if (item.url == m_history[i].url)
			emit entryRemoved(m_history[i]);
	}
}

void HistoryManager::save()
{
	QSettings settings{};

	settings.beginGroup(QLatin1String("history"));
	settings.setValue(QLatin1String("historyLimit"), m_historyLimit);

	bool saveAll{m_lastSavedUrl.isEmpty()};
	int first{m_history.count() - 1};

	if (!saveAll) {
		for (int i{0}; i < m_history.count(); ++i) {
			if (m_history[i].url == m_lastSavedUrl) {
				first = i - 1;
				break;
			}
		}
	}

	if (first == m_history.count() - 1)
		saveAll = true;

	QString directory{Application::instance()->paths()[Application::P_Data]};

	if (!QFile::exists(directory)) {
		QDir dir{};
		dir.mkpath(directory);
	}

	QFile historyFile{directory + QLatin1String("/history")};
	QTemporaryFile temporaryFile{};
	temporaryFile.setAutoRemove(false);

	bool open{false};

	if (saveAll)
		open = temporaryFile.open();
	else
		open = historyFile.open(QFile::Append);

	if (!open) {
		qWarning() << "Unable to open history file for saving"
				   << (saveAll ? temporaryFile.fileName() : historyFile.fileName());
		return;
	}

	QDataStream out{saveAll ? &temporaryFile : &historyFile};

	for (int i{first}; i >= 0; --i) {
		QByteArray data{};
		QDataStream stream{&data, QIODevice::WriteOnly};
		HistoryItem item = m_history[i];

		stream << HISTORY_VERSION << item.url << item.dateTime << item.title;
		out << data;
	}

	temporaryFile.close();

	if (saveAll) {
		if (historyFile.exists() && !historyFile.remove())
			qWarning() << "History: error removing old history." << historyFile.errorString();
		if (!temporaryFile.rename(historyFile.fileName()))
			qWarning() << "History: error moving new history over old." << temporaryFile.errorString()
					   << historyFile.fileName();
	}

	m_lastSavedUrl = m_history.value(0).url;
}

void HistoryManager::checkForExpired(bool removeExpiredEntriesDirectly)
{
	if (m_historyLimit < 0 || m_history.isEmpty())
		return;

	QDateTime now{QDateTime::currentDateTime()};
	int nextTimeout{0};

	while (!m_history.isEmpty()) {
		QDateTime checkForExpired{m_history.last().dateTime};
		checkForExpired.setDate(checkForExpired.date().addDays(m_historyLimit));

		if (now.daysTo(checkForExpired) > 7)
			nextTimeout = 7 * 86400;
		else
			nextTimeout = now.secsTo(checkForExpired);

		if (nextTimeout > 0)
			break;

		const HistoryItem& item = m_history.last();

		m_lastSavedUrl = QString();

		if (removeExpiredEntriesDirectly)
			m_history.takeLast();
		else
			emit entryRemoved(item);

		if (nextTimeout > 0)
			m_expiredTimer.start(nextTimeout * 1000);
	}
}

void HistoryManager::load()
{
	loadSettings();

	QFile historyFile{Application::paths()[Application::P_Data] + QLatin1String("/history")};

	if (!historyFile.exists())
		return;
	if (!historyFile.open(QIODevice::ReadOnly)) {
		qWarning() << "Unable to open history file" << historyFile.fileName();
		return;
	}

	QList<HistoryItem> list{};
	QDataStream in{&historyFile};
	QDataStream stream{};
	QByteArray data{};
	QBuffer buffer{};

	bool needToSort{false};
	HistoryItem lastInsertedItem{};

	stream.setDevice(&buffer);

	while (!historyFile.atEnd()) {
		in >> data;

		buffer.close();
		buffer.setBuffer(&data);
		buffer.open(QIODevice::ReadOnly);

		quint32 version{};
		stream >> version;
		if (version != HISTORY_VERSION)
			continue;

		HistoryItem item{};
		stream >> item.url;
		stream >> item.dateTime;
		stream >> item.title;

		if (!item.dateTime.isValid())
			continue;

		if (item == lastInsertedItem) {
			if (lastInsertedItem.title.isEmpty() && !list.isEmpty())
				list[0].title = item.title;
			continue;
		}

		if (!needToSort && !list.isEmpty() && lastInsertedItem < item)
			needToSort = true;

		list.prepend(item);
		lastInsertedItem = item;
	}

	if (needToSort)
		qSort(list.begin(), list.end());

	setHistory(list, true);

	if (needToSort) {
		m_lastSavedUrl = QString();
		m_saveTimer->changeOccurred();
	}

}
}