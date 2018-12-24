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

#include "HistoryMenu.hpp"

#include <QAction>

#include <QSqlQuery>

#include "History/History.hpp"

#include "Database/SqlDatabase.hpp"

#include "Utils/ClosedTabsManager.hpp"
#include "Utils/IconProvider.hpp"

#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
HistoryMenu::HistoryMenu(QWidget* parent) :
	QMenu(parent)
{
	setTitle(tr("Hi&story"));

	QAction* actionBack = addAction(Application::getAppIcon("arrow-left"), tr("&Back"));
	QAction* actionForward = addAction(Application::getAppIcon("arrow-right"), tr("&Forward"));
	QAction* actionHome = addAction(Application::getAppIcon("home"), tr("&Home"));
	QAction* actionShowAllHistory = addAction(Application::getAppIcon("history"), tr("Show &All History"));

	addSeparator();

	m_menuMostVisited = new QMenu(tr("Most Visited"), this);
	m_menuClosedTabs = new QMenu(tr("Closed Tabs"), this);

	addMenu(m_menuMostVisited);
	addMenu(m_menuClosedTabs);

	connect(actionBack, &QAction::triggered, this, &HistoryMenu::goBack);
	connect(actionForward, &QAction::triggered, this, &HistoryMenu::goForward);
	connect(actionHome, &QAction::triggered, this, &HistoryMenu::goHome);
	connect(actionShowAllHistory, &QAction::triggered, this, &HistoryMenu::showHistoryManager);

	connect(this, &QMenu::aboutToShow, this, &HistoryMenu::aboutToShow);
	connect(this, &QMenu::aboutToHide, this, &HistoryMenu::aboutToHide);
	connect(m_menuMostVisited, &QMenu::aboutToShow, this, &HistoryMenu::aboutToShowMostVisited);
	connect(m_menuClosedTabs, &QMenu::aboutToShow, this, &HistoryMenu::aboutToShowClosedTabs);
}

HistoryMenu::~HistoryMenu()
{
	// Empty
}

void HistoryMenu::setTabWidget(TabWidget* tabWidget)
{
	m_tabWidget = tabWidget;
}

void HistoryMenu::goBack()
{
	if (m_tabWidget)
		m_tabWidget->webTab()->webView()->back();
}

void HistoryMenu::goForward()
{
	if (m_tabWidget)
		m_tabWidget->webTab()->webView()->forward();
}

void HistoryMenu::goHome()
{
	if (m_tabWidget)
		m_tabWidget->webTab()->load(m_tabWidget->homeUrl());
}

void HistoryMenu::showHistoryManager()
{
	if (m_tabWidget)
		m_tabWidget->openHistoryDialog();
}

void HistoryMenu::aboutToShow()
{
	TabbedWebView* view{m_tabWidget ? m_tabWidget->webTab()->webView() : nullptr};

	if (view) {
		actions()[0]->setEnabled(view->history()->canGoBack());
		actions()[1]->setEnabled(view->history()->canGoForward());
	}

	while (actions().count() != 7) {
		QAction* act = actions()[7];
		if (act->menu()) {
			act->menu()->clear();
		}

		removeAction(act);
		delete act;
	}

	addSeparator();

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.exec("SELECT title, url FROM history ORDER BY date DESC LIMIT 10");


	while (query.next()) {
		const QUrl url{query.value(1).toUrl()};
		QString title{query.value(0).toString()};

		if (title.length() > 40)
			title = title.left(40) + QLatin1String("..");

		QAction* action{new QAction(title)};
		action->setData(url);
		action->setIcon(IconProvider::iconForUrl(url));

		connect(action, &QAction::triggered, this, &HistoryMenu::historyEntryActivated);

		addAction(action);
	}
}

void HistoryMenu::aboutToHide()
{
	actions()[0]->setEnabled(true);
	actions()[1]->setEnabled(true);
}
void HistoryMenu::aboutToShowMostVisited()
{
	m_menuMostVisited->clear();

	const QVector<History::HistoryEntry> mostVisited = Application::instance()->history()->mostVisited(10);

	foreach(const History::HistoryEntry& entry, mostVisited)
	{
		QString title{entry.title};

		if (title.length() > 40)
			title = title.left(40) + QLatin1String("..");

		QAction* action{new QAction(title)};
		action->setData(entry.url);
		action->setIcon(IconProvider::iconForUrl(entry.url));

		connect(action, &QAction::triggered, this, &HistoryMenu::historyEntryActivated);

		m_menuMostVisited->addAction(action);
	}

	if (m_menuMostVisited->isEmpty())
		m_menuMostVisited->addAction(tr("Empty"))->setEnabled(false);
}
void HistoryMenu::aboutToShowClosedTabs()
{
	m_menuClosedTabs->clear();

	if (!m_tabWidget)
		return;

	int i{0};
	const QLinkedList<ClosedTabsManager::Tab> closedTabs = m_tabWidget->closedTabsManager()->allClosedTab();

	foreach(const ClosedTabsManager::Tab& tab, closedTabs)
	{
		QString title = tab.title;

		if (title.length() > 40)
			title = title.left(40) + QLatin1String("..");

		QAction* action = m_menuClosedTabs->addAction(tab.icon, title, m_tabWidget, SLOT(restoreClosedTab()));
		action->setData(i++);
	}

	if (m_menuClosedTabs->isEmpty()) {
		m_menuClosedTabs->addAction(tr("Empty"))->setEnabled(false);
	}
	else {
		m_menuClosedTabs->addSeparator();
		m_menuClosedTabs->addAction(tr("Restore All Closed Tabs"), m_tabWidget, &TabWidget::restoreAllClosedTabs);
		m_menuClosedTabs->addAction(tr("Clear list"), m_tabWidget, &TabWidget::clearClosedTabsList);
	}
}
void HistoryMenu::historyEntryActivated()
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
		openUrl(action->data().toUrl());
}
void HistoryMenu::openUrl(const QUrl& url)
{
	if (m_tabWidget)
		m_tabWidget->webTab()->load(url);
}
}
