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

#include "MainMenu.hpp"

#include <QFileDialog>
#include <QMessageBox>

#include "Application.hpp"
#include "BrowserWindow.hpp"

#include "Bookmarks/BookmarksMenu.hpp"
#include "Bookmarks/BookmarksDialog.hpp"
#include "Bookmarks/AddBookmarkDialog.hpp"
#include "Bookmarks/BookmarkManager.hpp"
#include "Bookmarks/BookmarksModel.hpp"

#include "Widgets/Preferences/PreferencesDialog.hpp"
#include "Widgets/Tab/TabWidget.hpp"

#include "Web/Tab/TabbedWebView.hpp"

namespace Sn {

MainMenu::MainMenu(TabWidget* tabWidget, QWidget* parent) :
	QMenu(parent),
	m_tabWidget(tabWidget)
{
	QAction* showAllBookmarksAction = new QAction(tr("Show All Bookmarks"), this);
	QAction* addBookmarksAction = new QAction(tr("Add Bookmark..."), this);
	addBookmarksAction->setShortcut(QKeySequence("Ctrl+D"));
	addBookmarksAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	m_bookmarksMenu = new BookmarksMenu(this);
	m_bookmarksMenu->setTitle(tr("&Bookmarks"));
	m_bookmarksMenu->setInitialActions(QList<QAction*>() << showAllBookmarksAction << addBookmarksAction);

	QAction* newTabAction = createAction("NewTab", this, QIcon(), tr("New Tab"), "Ctrl+T");
	QAction* newWindowAction = createAction("NewWindow", this, QIcon(), tr("&New Window"), "Ctrl+N");
	QAction* newPrivateWindowAction =
		createAction("NewPrivateWindow", this, QIcon(), tr("New &Private Window"), "Ctrl+Shift+P");
	QAction* openFileAction = createAction("OpenFile", this, QIcon(), tr("Open &File"), "Ctrl+O");
	addSeparator();
	QAction* selectAllAction = createAction("SelectAll", this, QIcon(), tr("Select &All"), "Ctrl+A");
	QAction* findAction = createAction("Find", this, QIcon(), tr("&Find"), "Ctrl+F");
	addSeparator();
	addMenu(m_bookmarksMenu);
	addSeparator();
	QAction* showSettingsAction = createAction("ShowSettings",
											   this,
											   QIcon(),
											   tr("Pr&eferences"),
											   QKeySequence(QKeySequence::Preferences).toString());
	QAction* showAboutSieloAction = createAction("ShowAboutSielo", this, QIcon(), tr("&About Sielo"));
	addSeparator();
	QAction* quitAction = createAction("Quit", this, QIcon(), tr("Quit"), "Ctrl+Q");

	connect(newTabAction, &QAction::triggered, this, &MainMenu::newTab);
	connect(newWindowAction, &QAction::triggered, this, &MainMenu::newWindow);
	connect(newPrivateWindowAction, &QAction::triggered, this, &MainMenu::newPrivateWindow);
	connect(openFileAction, &QAction::triggered, this, &MainMenu::openFile);

	connect(selectAllAction, &QAction::triggered, this, &MainMenu::selectAll);
	connect(findAction, &QAction::triggered, this, &MainMenu::find);

	connect(showAllBookmarksAction, &QAction::triggered, this, &MainMenu::showAllBookmarks);
	connect(addBookmarksAction, &QAction::triggered, this, &MainMenu::addBookmarks);
	connect(m_bookmarksMenu, &BookmarksMenu::openUrl, this, &MainMenu::openBookmark);

	connect(showSettingsAction, &QAction::triggered, this, &MainMenu::showSettings);
	connect(showAboutSieloAction, &QAction::triggered, this, &MainMenu::showAboutSielo);

	connect(quitAction, &QAction::triggered, this, &MainMenu::quit);

	addActionsToTabWidget();
}

QAction* MainMenu::action(const QString& name) const
{
	return m_actions[name];
}

QAction* MainMenu::createAction(const QString& name, QMenu* menu, const QIcon& icon, const QString& trName,
								const QString& shortcut)
{
	QAction* action{menu->addAction(icon, trName)};
	action->setShortcut(QKeySequence(shortcut));
	action->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	m_actions[name] = action;
}

void MainMenu::setTabWidget(TabWidget* tabWidget)
{
	m_tabWidget = tabWidget;
}

void MainMenu::newTab()
{
	if (m_tabWidget)
		m_tabWidget->addView(m_tabWidget->urlOnNewTab());
}

void MainMenu::newWindow()
{
	Application::instance()->createWindow(Application::WT_NewWindow);
}

void MainMenu::newPrivateWindow()
{
	Application::instance()->startPrivateBrowsing();
}

void MainMenu::openFile()
{
	const QString fileTypes{QString("%1(*.html *.htm *.shtml *.shtm *.xhtml);;"
										"%2(*.png *.jpg *.jpeg *.bmp *.gif *.svg *.tiff);;"
										"%3(*.txt);;"
										"%4(*.*)")
								.arg(tr("HTML files"), tr("Image files"), tr("Text files"), tr("All files"))};

	QString path{QFileDialog::getOpenFileName(m_tabWidget->window(), tr("Open file"), QString(), fileTypes)};

	if (!path.isEmpty() && m_tabWidget)
		m_tabWidget->addView(QUrl::fromLocalFile(path));
}

void MainMenu::selectAll()
{
	if (m_tabWidget)
		m_tabWidget->weTab()->webView()->editSelectAll();
}

void MainMenu::find()
{
	QMessageBox::critical(m_tabWidget->window(), tr("Error"), tr("Currently unavailable. We are working on it."));
}

void MainMenu::showAllBookmarks()
{
	BookmarksDialog* dialog{new BookmarksDialog(m_tabWidget, Application::instance()->bookmarksManager())};
	dialog->show();
}

void MainMenu::addBookmarks()
{
	AddBookmarkDialog* dialog{new AddBookmarkDialog(m_tabWidget->weTab()->url().toString(),
													m_tabWidget->weTab()->title(),
													m_tabWidget,
													Application::instance()->bookmarksManager())};
	dialog->show();
}

void MainMenu::openBookmark(const QUrl& url)
{
	m_tabWidget->addView(url);
}

void MainMenu::showSettings()
{
	if (!m_preferences && m_tabWidget)
		m_preferences = new PreferencesDialog(m_tabWidget, m_tabWidget);

	m_preferences->show();
	m_preferences->raise();
	m_preferences->activateWindow();
}

void MainMenu::showAboutSielo()
{
	QMessageBox::information(m_tabWidget->window(),
							 tr("Sielo information"),
							 tr("You are using Sielo %1. More information details will come in newer versions.")
								 .arg(Application::currentVersion));
}

void MainMenu::quit()
{
	Application::instance()->quitApplication();
}

void MainMenu::addActionsToTabWidget()
{
	QHashIterator<QString, QAction*> it(m_actions);

	while (it.hasNext()) {
		it.next();

		m_tabWidget->addAction(it.value());
	}
}

}