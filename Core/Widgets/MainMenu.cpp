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

#include "MainMenu.hpp"

#include <QFileDialog>
#include <QMessageBox>

#include <QSettings>

#include "Application.hpp"
#include "BrowserWindow.hpp"

#include "Bookmarks/BookmarksMenu.hpp"

#include "MaquetteGrid/MaquetteGridMenu.hpp"

#include "History/HistoryMenu.hpp"

#include "Download/DownloadManager.hpp"

#include "Cookies/CookieManager.hpp"

#include "Widgets/AboutDialog.hpp"
#include "Widgets/HelpUsDialog.hpp"
#include "Widgets/TitleBar.hpp"
#include "Widgets/SiteInfo.hpp"
#include "Widgets/PartnerDialog.hpp"
#include "Widgets/Preferences/PreferencesDialog.hpp"
#include "Widgets/Tab/TabWidget.hpp"

#include "Web/Tab/TabbedWebView.hpp"

namespace Sn
{
MainMenu::MainMenu(TabWidget* tabWidget, QWidget* parent) :
	QMenu(parent),
	m_tabWidget(tabWidget)
{
	setObjectName("main-menu");

	m_toggleBookmarksAction = new QAction(Application::getAppIcon("toggle-bookmarks-bar"), tr("Show Bookmarks Bar"),
	                                      this);

	m_toggleBookmarksAction->setCheckable(true);

	QAction* backAction = new QAction(Application::getAppIcon("arrow-left"), tr("Back"), this);
	QAction* nextAction = new QAction(Application::getAppIcon("arrow-right"), tr("Forward"), this);
	QAction* homeAction = new QAction(Application::getAppIcon("home"), tr("Home"), this);
	backAction->setShortcuts(QKeySequence::keyBindings(QKeySequence::Back));
	backAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	nextAction->setShortcuts(QKeySequence::keyBindings(QKeySequence::Forward));
	nextAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	homeAction->setShortcut(QKeySequence("Ctrl+Shift+H"));
	homeAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	m_bookmarksMenu = new BookmarksMenu(this);
	m_bookmarksMenu->setMainWindow(m_tabWidget->window());

	m_maquetteGridMenu = new MaquetteGridMenu(m_tabWidget->window());

	m_historyMenu = new HistoryMenu(this);
	m_historyMenu->setMainWindow(m_tabWidget->window());
	//m_historyMenu->setTitle(tr("&History"));
	//m_historyMenu->setInitialActions(QList<QAction*>() << backAction << nextAction << homeAction);

	m_toolsMenu = new QMenu(this);
	m_toolsMenu->setTitle(tr("&Tools"));

	QAction* newTabAction =
		createAction("NewTab", this, Application::getAppIcon("tabbar-addtab", "tabs"), tr("New Tab"), "Ctrl+T");
	QAction* newWindowAction =
		createAction("NewWindow", this, Application::getAppIcon("new-window"), tr("&New Window"), "Ctrl+N");
	QAction* newPrivateWindowAction =
		createAction("NewPrivateWindow",
		             this,
		             Application::getAppIcon("anonymous", "tabs"),
		             tr("New &Private Window"),
		             "Ctrl+Shift+P");
	QAction* openFileAction =
		createAction("OpenFile", this, Application::getAppIcon("open-file"), tr("Open &File"), "Ctrl+O");
	addSeparator();
	QAction* selectAllAction = createAction("SelectAll",
	                                        this,
	                                        Application::getAppIcon("edit-select-all", "edit"),
	                                        tr("Select &All"),
	                                        "Ctrl+A");
	QAction* findAction = createAction("Find", this, Application::getAppIcon("search"), tr("&Find"), "Ctrl+F");
	addSeparator();
	addMenu(m_bookmarksMenu);
	addMenu(m_maquetteGridMenu);
	addMenu(m_historyMenu);
	addMenu(m_toolsMenu);
	QAction* showSiteInfoAction = createAction("ShowSiteInfo", m_toolsMenu, QIcon(), tr("Show Site Info"));
	QAction* showDownloadManagerAction =
		createAction("ShowDownloadManager",
		             m_toolsMenu,
		             Application::getAppIcon("downloads"),
		             tr("Download Manager"),
		             "Ctrl+Y");
	QAction
		* showCookiesManagerAction = createAction("ShowCookiesManager", m_toolsMenu, QIcon(),
		                                          tr("&Cookies Manager"));
	addSeparator();
	QAction* showSettingsAction = createAction("ShowSettings",
	                                           this,
	                                           Application::getAppIcon("preferences", "preferences"),
	                                           tr("Pr&eferences"),
	                                           QKeySequence(QKeySequence::Preferences).toString());
	QAction* showAboutSieloAction =
		createAction("ShowAboutSielo", this, Application::getAppIcon("ic_sielo"), tr("&About Sielo"));
	QAction* showPartnersAction = createAction("ShowPartners", this, QIcon(), tr("Partners"));
	QAction* showHelpUsAction =
		createAction("ShowHelpUs", this, Application::getAppIcon("heart"), tr("&Help Us"));
	addSeparator();
	QAction* quitAction = createAction("Quit", this, Application::getAppIcon("exit"), tr("Quit"), "Ctrl+Q");

	connect(newTabAction, &QAction::triggered, this, &MainMenu::newTab);
	connect(newWindowAction, &QAction::triggered, this, &MainMenu::newWindow);
	connect(newPrivateWindowAction, &QAction::triggered, this, &MainMenu::newPrivateWindow);
	connect(openFileAction, &QAction::triggered, this, &MainMenu::openFile);
	connect(m_toggleBookmarksAction, &QAction::triggered, this, &MainMenu::toggleBookmarksToolBar);

	connect(selectAllAction, &QAction::triggered, this, &MainMenu::selectAll);
	connect(findAction, &QAction::triggered, this, &MainMenu::find);

	connect(backAction, &QAction::triggered, this, &MainMenu::webBack);
	connect(nextAction, &QAction::triggered, this, &MainMenu::webForward);
	connect(homeAction, &QAction::triggered, this, &MainMenu::webHome);
	//connect(m_historyMenu, &HistoryMenu::openUrl, this, &MainMenu::openUrl);
	connect(showSiteInfoAction, &QAction::triggered, this, &MainMenu::showSiteInfo);
	connect(showDownloadManagerAction, &QAction::triggered, this, &MainMenu::showDownloadManager);
	connect(showCookiesManagerAction, &QAction::triggered, this, &MainMenu::showCookiesManager);

	connect(showSettingsAction, &QAction::triggered, this, &MainMenu::showSettings);
	connect(showAboutSieloAction, &QAction::triggered, this, &MainMenu::showAboutSielo);
	connect(showPartnersAction, &QAction::triggered, this, &MainMenu::showPartners);
	connect(showHelpUsAction, &QAction::triggered, this, &MainMenu::showHelpUs);

	connect(quitAction, &QAction::triggered, this, &MainMenu::quit);

	addActionsToTabWidget();
}

QAction *MainMenu::action(const QString& name) const
{
	return m_actions[name];
}

QAction *MainMenu::createAction(const QString& name, QMenu* menu, const QIcon& icon, const QString& trName,
                                const QString& shortcut)
{
	QAction* action{menu->addAction(icon, trName)};
	action->setShortcut(QKeySequence(shortcut));
	action->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	m_actions[name] = action;

	return action;
}

void MainMenu::setTabWidget(TabWidget* tabWidget)
{
	m_tabWidget = tabWidget;
}

void MainMenu::updateShowBookmarksBarText(bool visible)
{
	QSettings settings;
	settings.setValue(QLatin1String("ShowBookmarksToolBar"), visible);

	m_toggleBookmarksAction->setChecked(visible);
	m_toggleBookmarksAction->setText(!visible ? tr("Show Bookmarks Bar") : tr("Hide Bookmarks Bar"));
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
	const QString fileTypes{
		QString("%1(*.html *.htm *.shtml *.shtm *.xhtml);;"
			"%2(*.png *.jpg *.jpeg *.bmp *.gif *.svg *.tiff);;"
			"%3(*.txt);;"
			"%4(*.*)")
		.arg(tr("HTML files"), tr("Image files"), tr("Text files"), tr("All files"))
	};

	QString path{QFileDialog::getOpenFileName(m_tabWidget->window(), tr("Open file"), QString(), fileTypes)};

	if (!path.isEmpty() && m_tabWidget)
		m_tabWidget->addView(QUrl::fromLocalFile(path));
}

void MainMenu::toggleBookmarksToolBar()
{
	TitleBar* titleBar{m_tabWidget->window()->titleBar()};

	if (titleBar->showBookmarks()) {
		updateShowBookmarksBarText(false);
		titleBar->setShowBookmark(false);
	}
	else {
		updateShowBookmarksBarText(true);
		titleBar->setShowBookmark(true);
	}

	Application::instance()->saveSession();
}

void MainMenu::selectAll()
{
	if (m_tabWidget)
		m_tabWidget->weTab()->webView()->editSelectAll();
}

void MainMenu::find()
{
	if (m_tabWidget)
		m_tabWidget->weTab()->showSearchToolBar();
}

void MainMenu::webBack()
{
	if (m_tabWidget)
		m_tabWidget->weTab()->webView()->back();
}

void MainMenu::webForward()
{
	if (m_tabWidget)
		m_tabWidget->weTab()->webView()->forward();
}

void MainMenu::webHome()
{
	if (m_tabWidget)
		m_tabWidget->weTab()->sGoHome();
}

void MainMenu::openUrl(const QUrl& url)
{
	m_tabWidget->addView(url);
}

void MainMenu::showDownloadManager()
{
	DownloadManager* dialog{new DownloadManager(m_tabWidget)};
	dialog->show();
}

void MainMenu::showCookiesManager()
{
	CookieManager* dialog{new CookieManager()};
	dialog->show();
}

void MainMenu::showSiteInfo()
{
	if (m_tabWidget && SiteInfo::canShowSiteInfo(m_tabWidget->weTab()->url())) {
		SiteInfo* info{new SiteInfo(m_tabWidget->weTab()->webView())};
		info->show();
	}
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
	AboutDialog* dialog{new AboutDialog(m_tabWidget)};
	dialog->show();
}

void MainMenu::showPartners()
{
	PartnerDialog* dialog{new PartnerDialog(m_tabWidget)};
	dialog->show();
}

void MainMenu::showHelpUs()
{
	HelpUsDialog* dialog{new HelpUsDialog(m_tabWidget)};
	dialog->show();
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
