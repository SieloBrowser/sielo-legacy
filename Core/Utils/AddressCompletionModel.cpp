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

#include "AddressCompletionModel.hpp"

#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFileIconProvider>

#include <QMimeType>
#include <QMimeDatabase>

#include <QList>
#include <QtWidgets/QMessageBox>

#include "History/HistoryManager.hpp"

#include "Application.hpp"

namespace Sn {

AddressCompletionModel::AddressCompletionModel(QObject* parent) :
	QAbstractListModel(parent),
	m_types(HistoryCompletionType),
	m_updateTimer(0),
	m_showCompletionCategories(true)
{
	// Empty
}

void AddressCompletionModel::setTypes(CompletionTypes types)
{
	m_types = types;

	updateModel();
}

QVariant AddressCompletionModel::data(const QModelIndex& index, int role) const
{
	if (index.column() == 0 && index.row() >= 0 && index.row() < m_completions.count()) {
		switch (role) {
		case Qt::DecorationRole:
			return m_completions[index.row()].icon;
		case TextRole:
			return (m_completions[index.row()].text.isEmpty() ? m_completions[index.row()].url.toString()
															  : m_completions[index.row()].text);
		case UrlRole:
			return m_completions[index.row()].url;
		case TitleRole:
			return m_completions[index.row()].title;
		case KeywordRole:
			return m_completions[index.row()].keyword;
		case MatchRole:
			return (m_completions[index.row()].match.isEmpty() ? m_completions[index.row()].url.toString()
															   : m_completions[index.row()].match);
		case TypeRole:
			return static_cast<int>(m_completions[index.row()].type);
		}
	}

	return QVariant();
}

QVariant AddressCompletionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section)
	Q_UNUSED(orientation)
	Q_UNUSED(role)

	return QVariant();
}

Qt::ItemFlags AddressCompletionModel::flags(const QModelIndex& index) const
{
	if (!index.isValid() || m_completions[index.row()].type == AddressCompletionModel::HeaderType)
		return Qt::ItemNeverHasChildren;

	return (Qt::ItemFlags) (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren);
}

int AddressCompletionModel::rowCount(const QModelIndex& index) const
{
	return (index.isValid() ? 0 : m_completions.count());
}

bool AddressCompletionModel::event(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange && m_completions.count() > 0)
		updateModel();

	return QAbstractListModel::event(event);
}

void AddressCompletionModel::setFilter(const QString& filter)
{
	m_filter = filter;

	if (m_filter.isEmpty()) {
		if (m_updateTimer != 0) {
			killTimer(m_updateTimer);

			m_updateTimer = 0;
		}

		beginResetModel();

		m_completions.clear();

		endResetModel();

		emit completionReady(QString());
	}
	else if (m_updateTimer == 0)
		m_updateTimer = startTimer(50);
}

void AddressCompletionModel::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == m_updateTimer) {
		killTimer(m_updateTimer);

		m_updateTimer = 0;

		if (!m_filter.isEmpty()) {
			updateModel();

			emit completionReady(m_filter);
		}
	}
}

void AddressCompletionModel::updateModel()
{
	QVector<CompletionEntry> completions;
	completions.reserve(10);

	if (m_types.testFlag(SearchSuggestionCompletionType)) {
		const QString keyword(m_filter.section(QLatin1Char(' '), 0, 0));

		QString title{"Google"};
		QString text{m_filter};
		QIcon icon{Application::getAppIcon("google")};

		if (m_showCompletionCategories) {
			completions
				.append(CompletionEntry(QUrl(), tr("Search with %1").arg(title), QString(), QIcon(), HeaderType));
			title = QString();
		}

		CompletionEntry completionEntry{QUrl(), title, QString(), icon, SearchSuggestionType};
		completionEntry.text = text;
		completionEntry.keyword = keyword;

		completions.append(completionEntry);
	}

	if (m_types.testFlag(LocalPathSuggestionsCompletionType) && m_filter.contains(QDir::separator())) {
		const QString directory{m_filter.section(QDir::separator(), 0, -2) + QDir::separator()};
		const QString prefix{m_filter.section(QDir::separator(), -1, -1)};
		QString normalizedDirectory = directory;
		if (directory == QString(QLatin1Char('~')) || directory.startsWith(QLatin1Char('~') + QDir::separator())) {
			const QStringList locations(QStandardPaths::standardLocations(QStandardPaths::HomeLocation));

			if (!locations.isEmpty()) {
				normalizedDirectory = locations.first() + directory.mid(1);
			}
		}

		const QList<QFileInfo>
			entries(QDir(normalizedDirectory).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot));
		QFileIconProvider iconProvider{};
		bool wasAdded(!m_showCompletionCategories);

		for (int i{0}; i < entries.count(); ++i) {
			if (entries[i].fileName().startsWith(prefix, Qt::CaseInsensitive)) {
				const QString path{directory + entries[i].fileName()};
				const QMimeType type{QMimeDatabase().mimeTypeForFile(entries[i], QMimeDatabase::MatchExtension)};

				if (!wasAdded) {
					completions.append(CompletionEntry(QUrl(), tr("Local files"), QString(), QIcon(), HeaderType));
					wasAdded = true;
				}

				completions.append(CompletionEntry(QUrl::fromLocalFile(QDir::toNativeSeparators(path)),
												   path,
												   path,
												   QIcon::fromTheme(type.iconName(), iconProvider.icon(entries[i])),
												   LocalPathType));
			}
		}
	}

	if (m_types.testFlag(HistoryCompletionType)) {
		const QVector<HistoryManager::HistoryEntryMatch>
			entries{Application::instance()->historyManager()->findEntries(m_filter)};
		QVector<QString> urls;
		if (m_showCompletionCategories && !entries.isEmpty())
			completions.append(CompletionEntry(QUrl(), tr("History"), QString(), QIcon(), HeaderType));

		for (int i{0}; i < entries.count(); ++i) {
			QString url{QUrl(entries[i].item.url).host()};

			if (urls.contains(url))
				continue;

			QString pageTitle{};

			if (entries[i].item.title.isEmpty()) {
				QString page{QFileInfo(QUrl(entries[i].item.url).path()).fileName()};

				if (!page.isEmpty())
					pageTitle = page;
				else
					pageTitle = QUrl(url).topLevelDomain();
			}
			else {
				pageTitle = QUrl(url).topLevelDomain();
			}

			urls.append(url);
			completions.append(CompletionEntry(url,
											   pageTitle,
											   entries[i].match,
											   Application::getAppIcon("webpage"),
											   HistoryType));
		}
	}

	beginResetModel();

	m_completions = completions;

	endResetModel();
}

}