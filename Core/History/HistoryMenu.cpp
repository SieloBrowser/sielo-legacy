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

#include <ndb/query.hpp>

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

	QAction* action = addAction(Application::getAppIcon("arrow-left"), tr("&Back"), this, &HistoryMenu::goBack);

	action = addAction(Application::getAppIcon("arrow-right"), tr("&Forward"), this, &HistoryMenu::goForward);

	action = addAction(Application::getAppIcon("home"), tr("&Home"), this, &HistoryMenu::goHome);
	action->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Home));

	action = addAction(Application::getAppIcon("history"), tr("Show &All History"), this,
	                   &HistoryMenu::showHistoryManager);
	action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_H));

	addSeparator();

	m_menuMostVisited = new QMenu(tr("Most Visited"), this);
	m_menuClosedTabs = new QMenu(tr("Closed Tabs"), this);

	addMenu(m_menuMostVisited);
	addMenu(m_menuClosedTabs);

	connect(this, &QMenu::aboutToShow, this, &HistoryMenu::aboutToShow);
	connect(this, &QMenu::aboutToHide, this, &HistoryMenu::aboutToHide);
	connect(m_menuMostVisited, &QMenu::aboutToShow, this, &HistoryMenu::aboutToShowMostVisited);
	connect(m_menuClosedTabs, &QMenu::aboutToShow, this, &HistoryMenu::aboutToShowClosedTabs);
}

HistoryMenu::~HistoryMenu()
{
	// Empty
}

void HistoryMenu::setMainWindow(BrowserWindow* window)
{
	m_window = window;
}

void HistoryMenu::goBack()
{
	if (m_window)
		m_window->webView()->back();
}

void HistoryMenu::goForward()
{
	if (m_window)
		m_window->webView()->forward();
}

void HistoryMenu::goHome()
{
	if (m_window)
		m_window->loadUrl(m_window->homePageUrl());
}

void HistoryMenu::showHistoryManager()
{
	if (m_window)
		m_window->tabWidget()->openHistoryDialog();
}

void HistoryMenu::aboutToShow()
{
	TabbedWebView* view{m_window ? m_window->webView() : 0};

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

	for (auto& entry : ndb::query<dbs::navigation>() << ((history.url, history.title) << ndb::sort(ndb::desc(history.date)) << ndb::limit(10))) {
		const QUrl url{QString(entry[history.url])};
		QString title = entry[history.title];

		if (title.length() > 40)
			title = title.left(40) + QLatin1String("..");

		QAction* action{ new QAction(title) };
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

	foreach (const History::HistoryEntry& entry, mostVisited)
	{
		QString title{ entry.title };

		if (title.length() > 40)
			title = title.left(40) + QLatin1String("..");

		QAction* action{ new QAction(title) };
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

	if (!m_window)
		return;

	TabWidget* tabWidget{ m_window->tabWidget() };
	int i{ 0 };
	const QLinkedList<ClosedTabsManager::Tab> closedTabs = tabWidget->closedTabsManager()->allClosedTab();

	foreach (const ClosedTabsManager::Tab& tab, closedTabs)
	{
		QString title = tab.title;

		if (title.length() > 40)
			title = title.left(40) + QLatin1String("..");

		QAction* action = m_menuClosedTabs->addAction(tab.icon, title, tabWidget, SLOT(restoreClosedTab()));
		action->setData(i++);
	}

	if (m_menuClosedTabs->isEmpty()) {
		m_menuClosedTabs->addAction(tr("Empty"))->setEnabled(false);
	}
	else {
		m_menuClosedTabs->addSeparator();
		m_menuClosedTabs->addAction(tr("Restore All Closed Tabs"), tabWidget, &TabWidget::restoreAllClosedTabs);
		m_menuClosedTabs->addAction(tr("Clear list"), tabWidget, &TabWidget::clearClosedTabsList);
	}
}
void HistoryMenu::historyEntryActivated()
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
		openUrl(action->data().toUrl());
}
void HistoryMenu::openUrl(const QUrl& url)
{
	if (m_window)
		m_window->loadUrl(url);
}
}
