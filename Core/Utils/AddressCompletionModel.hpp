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
#ifndef SIELOBROWSER_ADDRESSCOMPLETER_HPP
#define SIELOBROWSER_ADDRESSCOMPLETER_HPP

#include <QVector>
#include <QUrl>
#include <QIcon>

#include <QTimerEvent>

#include <QAbstractListModel>


namespace Sn {

class AddressCompletionModel: public QAbstractListModel {
Q_OBJECT

public:
	enum CompletionType {
		UnknownCompletionType = 0,
		HistoryCompletionType = 1,
		SearchSuggestionCompletionType = 2,
		LocalPathSuggestionsCompletionType = 4
	};

	Q_DECLARE_FLAGS(CompletionTypes, CompletionType)

	enum EntryType {
		UnknownType = 0,
		HeaderType,
		HistoryType,
		SearchSuggestionType,
		LocalPathType
	};

	enum EntryRole {
		TextRole = Qt::DisplayRole,
		UrlRole = Qt::StatusTipRole,
		TitleRole = Qt::UserRole,
		MatchRole,
		KeywordRole,
		TypeRole
		// TODO: TimeVisistedRole
	};

	struct CompletionEntry {
		QString text{};
		QString title{};
		QString match{};
		QString keyword{};
		QUrl url{};
		QIcon icon{};
		EntryType type = UnknownType;

		explicit CompletionEntry(const QUrl& urlValue, const QString& titleValue, const QString& matchValue,
								 const QIcon& iconValue, EntryType typeValue) :
			title(titleValue),
			match(matchValue),
			url(urlValue),
			icon(iconValue),
			type(typeValue)
		{
			// Empty
		}

		CompletionEntry()
		{
			// Empty
		}
	};

	AddressCompletionModel(QObject* parent = nullptr);

	void setTypes(CompletionTypes types);
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
	int rowCount(const QModelIndex& index = QModelIndex()) const Q_DECL_OVERRIDE;

	bool event(QEvent* event) Q_DECL_OVERRIDE;

signals:
	void completionReady(const QString& filter);

public slots:
	void setFilter(const QString& filter = QString());

protected:
	void timerEvent(QTimerEvent* event) Q_DECL_OVERRIDE;
	void updateModel();

private:
	QVector<CompletionEntry> m_completions;

	QString m_filter{};
	AddressCompletionModel::CompletionTypes m_types;

	int m_updateTimer{0};
	bool m_showCompletionCategories{true};
};
}

#endif //SIELOBROWSER_ADDRESSCOMPLETER_HPP
