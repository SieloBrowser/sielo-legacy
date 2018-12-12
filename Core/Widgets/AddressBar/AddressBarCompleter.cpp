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

#include "AddressBarCompleter.hpp"

#include <QWindow>

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarkItem.hpp"

#include "Widgets/AddressBar/AddressBar.hpp"
#include "Widgets/AddressBar/AddressBarCompleterModel.hpp"
#include "Widgets/AddressBar/AddressBarCompleterView.hpp"
#include "Widgets/AddressBar/AddressBarCompleterRefreshJob.hpp"

#include "History/History.hpp"

#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
AddressBarCompleterView* AddressBarCompleter::s_view = nullptr;
AddressBarCompleterModel* AddressBarCompleter::s_model = nullptr;

AddressBarCompleter::AddressBarCompleter(QObject* parent) :
	QObject(parent)
{
	if (!s_view) {
		s_model = new AddressBarCompleterModel();
		s_view = new AddressBarCompleterView();

		s_view->setModel(s_model);
	}
}

void AddressBarCompleter::setTabWidget(TabWidget* tabWidget)
{
	m_tabWidget = tabWidget;
}

void AddressBarCompleter::setAddressBar(AddressBar* addressBar)
{
	m_addressBar = addressBar;
}

void AddressBarCompleter::closePopup()
{
	m_popupClosed = true;
	s_view->close();
}

void AddressBarCompleter::complete(const QString& string)
{
	QString trimmedStr{string.trimmed()};

	if (trimmedStr.startsWith("!")) 
		return;

	m_popupClosed = false;

	emit cancelRefreshJob();

	AddressBarCompleterRefreshJob* job{new AddressBarCompleterRefreshJob(trimmedStr)};
	connect(job, &AddressBarCompleterRefreshJob::finished, this, &AddressBarCompleter::refreshJobFinished);
	connect(this, &AddressBarCompleter::cancelRefreshJob, job, &AddressBarCompleterRefreshJob::jobCancelled);

	// TODO: show search suggestion
}

void AddressBarCompleter::showMostVisited()
{
	m_addressBar->setFocus();
	complete(QString());
}

void AddressBarCompleter::refreshJobFinished()
{
	AddressBarCompleterRefreshJob* job = qobject_cast<AddressBarCompleterRefreshJob*>(sender());
	Q_ASSERT(job);

	if (!job->isCanceled() && job->timestamp() > m_lastRefreshTimestamp && !m_popupClosed) {
		s_model->setCompletions(job->completions());
		m_lastRefreshTimestamp = job->timestamp();

		showPopup();
		addSuggestions(m_oldSuggestions);

		if (!s_view->currentIndex().isValid() && s_model
		                                         ->index(0, 0).data(AddressBarCompleterModel::VisitSearchItemRole).toBool()) {
			m_ignoreCurrentChanged = true;
			s_view->setCurrentIndex(s_model->index(0, 0));
			m_ignoreCurrentChanged = false;
		}

		emit showDomainCompletion(job->domainCompletion());

		m_originalText = m_addressBar->text();
		s_view->setOriginalText(m_originalText);
	}

	job->deleteLater();
}

void AddressBarCompleter::slotPopupClosed()
{
	m_oldSuggestions.clear();

	disconnect(s_view, &AddressBarCompleterView::closed, this, &AddressBarCompleter::slotPopupClosed);
	disconnect(s_view, &AddressBarCompleterView::indexActivated, this, &AddressBarCompleter::indexActivated);
	disconnect(s_view, &AddressBarCompleterView::indexCtrlActivated, this, &AddressBarCompleter::indexCtrlActivated);
	disconnect(s_view, &AddressBarCompleterView::indexShiftActivated, this, &AddressBarCompleter::indexShiftActivated);
	disconnect(s_view, &AddressBarCompleterView::indexDeleteRequested, this, &AddressBarCompleter::indexDeleteRequested);
	disconnect(s_view->selectionModel(), &QItemSelectionModel::currentChanged, this, &AddressBarCompleter::currentChanged);

	emit popupClosed();
}

void AddressBarCompleter::addSuggestions(const QStringList& suggestions)
{
	QList<QStandardItem*> suggestionIems = s_model->suggestionItems();

	foreach (QStandardItem* item, suggestionIems) {
		s_model->takeRow(item->row());
		delete item;
	}

	QList<QStandardItem*> items{};

	foreach (const QString& suggestion, suggestions) {
		QStandardItem* item{new QStandardItem()};
		item->setText(suggestion);
		item->setData(suggestion, AddressBarCompleterModel::TitleRole);
		item->setData(suggestion, AddressBarCompleterModel::UrlRole);
		item->setData(m_suggestionsTerm, AddressBarCompleterModel::SearchStringRole);
		item->setData(true, AddressBarCompleterModel::SearchSuggestionRole);

		items.append(item);
	}

	s_model->addCompletions(items);
	m_oldSuggestions = suggestions;

	showPopup();
}

void AddressBarCompleter::currentChanged(const QModelIndex& index)
{
	if (m_ignoreCurrentChanged)
		return;

	QString completion{index.data().toString()};
	bool completeDomain{index.data(AddressBarCompleterModel::VisitSearchItemRole).toBool()};

	if (completeDomain && completion == m_originalText)
		completeDomain = false;

	if (completion.isEmpty()) {
		completeDomain = true;
		completion = m_originalText;
	}

	emit showCompletion(completion, completeDomain);
}

void AddressBarCompleter::indexActivated(const QModelIndex& index)
{
	Q_ASSERT(index.isValid());

	bool ok{};
	const int tabPos{index.data(AddressBarCompleterModel::TabPositionTabRole).toInt(&ok)};

	if (ok && tabPos > -1) {
		TabWidget* tabSpace = static_cast<TabWidget*>(index
		                                              .data(AddressBarCompleterModel::TabPositionTabsSpaceRole).value<void*>()
		);
		Q_ASSERT(tabSpace);

		switchToTab(tabSpace, tabPos);
		return;
	}

	if (index.data(AddressBarCompleterModel::BookmarkRole).toBool()) {
		BookmarkItem* bookmark{ static_cast<BookmarkItem*>(index.data(AddressBarCompleterModel::BookmarkItemRole).value<void*>()) };
		bookmark->updateVisitCount();
	}

	QString urlString{index.data(AddressBarCompleterModel::UrlRole).toString()};

	if (index.data(AddressBarCompleterModel::VisitSearchItemRole).toBool())
		urlString = m_originalText;

	loadString(urlString);
}

void AddressBarCompleter::indexCtrlActivated(const QModelIndex& index)
{
	Q_ASSERT(index.isValid());
	Q_ASSERT(m_tabWidget);

	if (index.data(AddressBarCompleterModel::BookmarkRole).toBool()) {
		BookmarkItem* bookmark{ static_cast<BookmarkItem*>(index.data(AddressBarCompleterModel::BookmarkItemRole).value<void*>()) };
		bookmark->updateVisitCount();
	}

	const QUrl url{index.data(AddressBarCompleterModel::UrlRole).toUrl()};
	const QString title{index.data(AddressBarCompleterModel::TitleRole).toString()};

	closePopup();

	emit clearCompletion();

	m_tabWidget->addView(url, title, Application::NTT_CleanSelectedTab);
}

void AddressBarCompleter::indexShiftActivated(const QModelIndex& index)
{
	Q_ASSERT(index.isValid());

	if (index.data(AddressBarCompleterModel::BookmarkRole).toBool()) {
		BookmarkItem* bookmark{ static_cast<BookmarkItem*>(index.data(AddressBarCompleterModel::BookmarkItemRole).value<void*>()) };
		bookmark->updateVisitCount();
	}

	const QString urlString{index.data(AddressBarCompleterModel::UrlRole).toString()};
	const int tabPos{index.data(AddressBarCompleterModel::TabPositionTabRole).toInt()};

	if (tabPos > -1) {
		loadString(urlString);
		return;
	}

	closePopup();

	emit clearCompletion();

	Application::instance()->createWindow(Application::WT_NewWindow, QUrl(urlString));
}

void AddressBarCompleter::indexDeleteRequested(const QModelIndex& index)
{
	if (!index.isValid())
		return;

	if (index.data(AddressBarCompleterModel::BookmarkRole).toBool()) {
		BookmarkItem* bookmark{ static_cast<BookmarkItem*>(index.data(AddressBarCompleterModel::BookmarkItemRole).value<void*>()) };
		Application::instance()->bookmarks()->removeBookmark(bookmark);
	}
	else {
		int id = index.data(AddressBarCompleterModel::IdRole).toInt();
		Application::instance()->history()->deleteHistoryEntry(id);
	}

	s_view->setUpdatesEnabled(false);
	s_model->removeRow(index.row(), index.parent());
	s_view->setUpdatesEnabled(true);

	if (s_model->rowCount() == 0)
		closePopup();
	else
		showPopup();
}

void AddressBarCompleter::switchToTab(TabWidget* tabWidget, int tab)
{
	Q_ASSERT(tabWidget);
	Q_ASSERT(tab >= 0);

	closePopup();

	emit clearCompletion();

	if (tabWidget->window()->isActiveWindow() || tabWidget->currentIndex() != tab) {
		tabWidget->setCurrentIndex(tab);

		tabWidget->window()->show();
		tabWidget->window()->activateWindow();
		tabWidget->window()->raise();
	}
	else
		tabWidget->webTab()->webView()->setFocus();
}

void AddressBarCompleter::loadString(const QString& url)
{
	closePopup();

	emit showCompletion(url, false);
	emit loadCompletion();
}

void AddressBarCompleter::showPopup()
{
	Q_ASSERT(m_addressBar);

	if (!m_addressBar->hasFocus() || s_model->rowCount() == 0) {
		s_view->close();
		return;
	}

	if (s_view->isVisible()) {
		adjustPopupSize();
		return;
	}

	QRect popupRect(m_addressBar->mapToGlobal(m_addressBar->pos()), m_addressBar->size());
	popupRect.setY(popupRect.bottom());

	s_view->setGeometry(popupRect);
	s_view->setFocusProxy(m_addressBar);
	s_view->setCurrentIndex(QModelIndex());

	connect(s_view, &AddressBarCompleterView::closed, this, &AddressBarCompleter::slotPopupClosed);
	connect(s_view, &AddressBarCompleterView::indexActivated, this, &AddressBarCompleter::indexActivated);
	connect(s_view, &AddressBarCompleterView::indexCtrlActivated, this, &AddressBarCompleter::indexCtrlActivated);
	connect(s_view, &AddressBarCompleterView::indexShiftActivated, this, &AddressBarCompleter::indexShiftActivated);
	connect(s_view, &AddressBarCompleterView::indexDeleteRequested, this, &AddressBarCompleter::indexDeleteRequested);
	connect(s_view->selectionModel(), &QItemSelectionModel::currentChanged, this, &AddressBarCompleter::currentChanged);

	if (m_addressBar->nativeParentWidget()) {
		s_view->createWinId();
		s_view->windowHandle()->setTransientParent(m_addressBar->nativeParentWidget()->windowHandle());
	}

	adjustPopupSize();
}

void AddressBarCompleter::adjustPopupSize()
{
	const int maxItemsCount{12};
	const int popupHeight{s_view->sizeHintForRow(0) * qMin(maxItemsCount, s_model->rowCount()) + 2 * s_view->frameWidth()};

	m_originalText = m_addressBar->text();
	s_view->setOriginalText(m_originalText);

	s_view->resize(s_view->width(), popupHeight);
	s_view->show();
}
}
