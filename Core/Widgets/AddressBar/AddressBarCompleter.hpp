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
#ifndef SIELOBROWSER_ADDRESSBARCOMPLETER_HPP
#define SIELOBROWSER_ADDRESSBARCOMPLETER_HPP

#include "SharedDefines.hpp"

#include <QObject>

namespace Sn
{
class AddressBar;

class AddressBarCompleterModel;
class AddressBarCompleterView;

class TabWidget;

class SIELO_SHAREDLIB AddressBarCompleter: public QObject {
Q_OBJECT

public:
	AddressBarCompleter(QObject* parent = nullptr);

	void setTabWidget(TabWidget* tabWidget);
	void setAddressBar(AddressBar* addressBar);

	void closePopup();

signals:
	void showCompletion(const QString& completion, bool completeDomain);
	void showDomainCompletion(const QString& completion);
	void loadCompletion();
	void clearCompletion();
	void popupClosed();
	void cancelRefreshJob();

public slots:
	void complete(const QString& string);
	void showMostVisited();

private slots:
	void refreshJobFinished();
	void slotPopupClosed();
	void addSuggestions(const QStringList& suggestions);

	void currentChanged(const QModelIndex& index);
	void indexActivated(const QModelIndex& index);
	void indexCtrlActivated(const QModelIndex& index);
	void indexShiftActivated(const QModelIndex& index);
	void indexDeleteRequested(const QModelIndex& index);

private:
	void switchToTab(TabWidget* tabWidget, int tab);
	void loadString(const QString& url);

	void showPopup();
	void adjustPopupSize();

	TabWidget* m_tabWidget{nullptr};
	AddressBar* m_addressBar{nullptr};

	QString m_originalText{};
	QStringList m_oldSuggestions{};
	QString m_suggestionsTerm{};

	qint64 m_lastRefreshTimestamp{0};
	bool m_popupClosed{false};
	bool m_ignoreCurrentChanged{false};

	static AddressBarCompleterView* s_view;
	static AddressBarCompleterModel* s_model;
};
}

#endif //SIELOBROWSER_ADDRESSBARCOMPLETER_HPP
