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

#include "Application.hpp"
#include "BrowserWindow.hpp"

#include "Bookmarks/BookmarksMenu.hpp"
#include "Bookmarks/BookmarksToolbar.hpp"

#include "MaquetteGrid/MaquetteGridMenu.hpp"

#include "History/HistoryMenu.hpp"

#include "Download/DownloadManager.hpp"

#include "Cookies/CookieManager.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Utils/Settings.hpp"

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

	m_toggleBookmarksAction = new QAction(Application::getAppIcon("toggle-bookmarks-bar"), m_tabWidget->window()->bookmarksToolBar()->isVisible() ? tr("Hide Bookmarks Bar") : tr("Show Bookmarks Bar"),
	                                      this);

	QAction* backAction = new QAction(Application::getAppIcon("arrow-left"), tr("Back"), this);
	QAction* nextAction = new QAction(Application::getAppIcon("arrow-right"), tr("Forward"), this);
	QAction* homeAction = new QAction(Application::getAppIcon("home"), tr("Home"), this);
	backAction->setShortcuts(QKeySequence::keyBindings(QKeySequence::Back));
	backAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	nextAction->setShortcuts(QKeySequence::keyBindings(QKeySequence::Forward));
	nextAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	homeAction->setShortcut(QKeySequence("Ctrl+Shift+H"));
	homeAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	m_sideBarsMenu = new QMenu(tr("Side Bar"), this);
	m_tabWidget->createSideBarsMenu(m_sideBarsMenu);

	m_viewMenu = new QMenu(tr("View"), this);
	m_viewMenu->addMenu(m_sideBarsMenu);
	m_viewMenu->addSeparator();

	m_bookmarksMenu = new BookmarksMenu(this);
	m_bookmarksMenu->insertAction(m_bookmarksMenu->actions()[3], m_toggleBookmarksAction);
	m_bookmarksMenu->setMainWindow(m_tabWidget->window());

	m_maquetteGridMenu = new MaquetteGridMenu(m_tabWidget);

	m_historyMenu = new HistoryMenu(this);
	m_historyMenu->setTabWidget(m_tabWidget);
	//m_historyMenu->setTitle(tr("&History"));
	//m_historyMenu->setInitialActions(QList<QAction*>() << backAction << nextAction << homeAction);

	m_toolsMenu = new QMenu(this);
	m_toolsMenu->setTitle(tr("&Tools"));

	m_pluginsMenu = new QMenu(tr("&Extensions"), this);
	m_pluginsMenu->menuAction()->setVisible(false);

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

	QAction* stopAction = createAction("Stop", m_viewMenu, QIcon(), tr("&Stop"), "Esc");
	QAction* reloadAction = createAction("Reload", m_viewMenu, QIcon(), tr("&Reload"), "F5");
	m_viewMenu->addSeparator();
	QAction* zoomInAction = createAction("ZoomIn", m_viewMenu, QIcon(), tr("Zoom &In"), "Ctrl++");
	QAction* zoomOutAction = createAction("ZoomOut", m_viewMenu, QIcon(), tr("Zoom &Out"), "Ctrl+-");
	QAction* zoomResetAction = createAction("ZoomReset", m_viewMenu, QIcon(), tr("Reset"), "Ctrl+0");
	m_viewMenu->addSeparator();
	QAction* showPageSourceAction = createAction("PageSource", m_viewMenu, QIcon(), tr("&Page Source"), "Ctrl+U");
	QAction* showFullScreenAction = createAction("FullScreen", m_viewMenu, QIcon(), tr("&FullScreen"), "F11");

	addMenu(m_viewMenu);
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
	m_toolsMenu->addMenu(m_pluginsMenu);
	addSeparator();
	QAction* showSettingsAction = createAction("ShowSettings",
	                                           this,
	                                           Application::getAppIcon("preferences", "preferences"),
	                                           tr("Pr&eferences"),
	                                           "Ctrl+,");
	QAction* showAboutSieloAction =
		createAction("ShowAboutSielo", this, Application::getAppIcon("ic_sielo"), tr("&About Sielo"));
	QAction* showPartnersAction = createAction("ShowPartners", this, QIcon(), tr("Partners"));
	QAction* showHelpUsAction =
		createAction("ShowHelpUs", this, Application::getAppIcon("heart"), tr("&Help Us"));
	QAction* openDiscord = createAction("OpenDiscord", this, Application::getAppIcon("discord"), tr("&Discord Server"));
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

	connect(stopAction, &QAction::triggered, this, &MainMenu::stop);
	connect(reloadAction, &QAction::triggered, this, &MainMenu::reload);
	connect(zoomInAction, &QAction::triggered, this, &MainMenu::zoomIn);
	connect(zoomOutAction, &QAction::triggered, this, &MainMenu::zoomOut);
	connect(zoomResetAction, &QAction::triggered, this, &MainMenu::zoomReset);
	connect(showPageSourceAction, &QAction::triggered, this, &MainMenu::showPageSource);
	connect(showFullScreenAction, &QAction::triggered, this, &MainMenu::showFullScreen);

	//connect(m_historyMenu, &HistoryMenu::openUrl, this, &MainMenu::openUrl);
	connect(showSiteInfoAction, &QAction::triggered, this, &MainMenu::showSiteInfo);
	connect(showDownloadManagerAction, &QAction::triggered, this, &MainMenu::showDownloadManager);
	connect(showCookiesManagerAction, &QAction::triggered, this, &MainMenu::showCookiesManager);

	connect(showSettingsAction, &QAction::triggered, this, &MainMenu::showSettings);
	connect(showAboutSieloAction, &QAction::triggered, this, &MainMenu::showAboutSielo);
	connect(showPartnersAction, &QAction::triggered, this, &MainMenu::showPartners);
	connect(showHelpUsAction, &QAction::triggered, this, &MainMenu::showHelpUs);
	connect(openDiscord, &QAction::triggered, this, &MainMenu::openDiscord);

	connect(quitAction, &QAction::triggered, this, &MainMenu::quit);

	connect(m_sideBarsMenu, &QMenu::aboutToShow, this, &MainMenu::aboutToShowSideBarMenu);
	connect(m_toolsMenu, &QMenu::aboutToShow, this, &MainMenu::aboutToShowToolsMenu);

	addActionsToTabWidget();
}

void MainMenu::initMenuBar(QMenuBar *menuBar)
{
	QMenu* menuFile{new QMenu(tr("&File"))};
	menuFile->addAction(m_actions["NewTab"]);
	menuFile->addAction(m_actions["NewWindow"]);
	menuFile->addAction(m_actions["NewPrivateWindow"]);
	menuFile->addAction(m_actions["OpenFile"]);
	menuFile->addSeparator();
	menuFile->addAction(m_actions["ShowSettings"]);

	QMenu* menuEdit{new QMenu(tr("&Edit"))};
	QAction* undoAction = createAction("Undo", menuEdit, QIcon(), tr("&Undo"), "Ctrl+Z");
	QAction* redoAction = createAction("Redo", menuEdit, QIcon(), tr("&Redo"), "Ctrl+Shift+Z");
	QAction* cutAction = createAction("Cut", menuEdit, QIcon(), tr("&Cut"), "Ctrl+X");
	QAction* copyAction = createAction("Copy", menuEdit, QIcon(), tr("C&opy"), "Ctrl+C");
	QAction* pastAction = createAction("Past", menuEdit, QIcon(), tr("&Past"), "Ctrl+P");
	menuEdit->addAction(m_actions["SelectAll"]);
	menuEdit->addAction(m_actions["Find"]);

	QMenu* menuHelp{new QMenu("&Help")};
	menuHelp->addAction(m_actions["ShowAboutSielo"]);
	menuHelp->addAction(m_actions["ShowHelpUs"]);
	menuHelp->addAction(m_actions["OpenDiscord"]),

	connect(undoAction, &QAction::triggered, this, &MainMenu::undo);
	connect(redoAction, &QAction::triggered, this, &MainMenu::redo);
	connect(cutAction, &QAction::triggered, this, &MainMenu::cut);
	connect(copyAction, &QAction::triggered, this, &MainMenu::copy);
	connect(pastAction, &QAction::triggered, this, &MainMenu::paste);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuEdit);
	menuBar->addMenu(m_viewMenu);
	menuBar->addMenu(m_historyMenu);
	menuBar->addMenu(m_bookmarksMenu);
	menuBar->addMenu(m_toolsMenu);
	menuBar->addMenu(menuHelp);
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
	Settings settings;
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
	Settings settings{};

	if (m_tabWidget->window()->bookmarksToolBar()->isVisible()) {
		updateShowBookmarksBarText(false);
		settings.setValue("Settings/ShowBookmarksToolBar", false);
		m_tabWidget->window()->bookmarksToolBar()->hide();
	}
	else {
		updateShowBookmarksBarText(true);
		settings.setValue("Settings/ShowBookmarksToolBar", true);
		m_tabWidget->window()->bookmarksToolBar()->show();
	}

	Application::instance()->saveSession();
}

void MainMenu::undo()
{
	if (m_tabWidget) {
		m_tabWidget->webTab()->webView()->editUndo();
	}
}

void MainMenu::redo()
{
	if (m_tabWidget) {
		m_tabWidget->webTab()->webView()->editRedo();
	}
}

void MainMenu::cut()
{
	if (m_tabWidget) {
		m_tabWidget->webTab()->webView()->editCut();
	}
}

void MainMenu::copy()
{
	if (m_tabWidget) {
		m_tabWidget->webTab()->webView()->editCopy();
	}
}

void MainMenu::paste()
{
	if (m_tabWidget) {
		m_tabWidget->webTab()->webView()->editPast();
	}
}

void MainMenu::selectAll()
{
	if (m_tabWidget)
		m_tabWidget->webTab()->webView()->editSelectAll();
}

void MainMenu::find()
{
	if (m_tabWidget)
		m_tabWidget->webTab()->showSearchToolBar();
}

void MainMenu::stop()
{
	if (m_tabWidget) {
		m_tabWidget->webTab()->webView()->stop();
	}
}

void MainMenu::reload()
{
	if (m_tabWidget) {
		m_tabWidget->webTab()->webView()->reload();
	}
}

void MainMenu::zoomIn()
{
	if (m_tabWidget) {
		m_tabWidget->webTab()->webView()->zoomIn();
	}
}

void MainMenu::zoomOut()
{
	if (m_tabWidget) {
		m_tabWidget->webTab()->webView()->zoomOut();
	}
}

void MainMenu::zoomReset()
{
	if (m_tabWidget) {
		m_tabWidget->webTab()->webView()->zoomReset();
	}
}

void MainMenu::showPageSource()
{
	if (m_tabWidget)
		m_tabWidget->webTab()->webView()->showSource();
}

void MainMenu::showFullScreen()
{
	if (m_tabWidget) {
		m_tabWidget->window()->toggleFullScreen();
	}
}

void MainMenu::webBack()
{
	if (m_tabWidget)
		m_tabWidget->webTab()->webView()->back();
}

void MainMenu::webForward()
{
	if (m_tabWidget)
		m_tabWidget->webTab()->webView()->forward();
}

void MainMenu::webHome()
{
	if (m_tabWidget)
		m_tabWidget->webTab()->sGoHome();
}

void MainMenu::openUrl(const QUrl& url)
{
	m_tabWidget->addView(url);
}

void MainMenu::showDownloadManager()
{
	Application::instance()->downloadManager()->show();
	Application::instance()->downloadManager()->raise();
}

void MainMenu::showCookiesManager()
{
	CookieManager* dialog{new CookieManager()};
	dialog->show();
}

void MainMenu::showSiteInfo()
{
	if (m_tabWidget && SiteInfo::canShowSiteInfo(m_tabWidget->webTab()->url())) {
		SiteInfo* info{new SiteInfo(m_tabWidget->webTab()->webView())};
		info->show();
	}
}

void MainMenu::showSettings()
{
	if (!m_preferences && m_tabWidget) {
		m_preferences = new PreferencesDialog(m_tabWidget, m_tabWidget);
		m_tabWidget->addApplication(m_preferences);
	}

	m_tabWidget->goToApplication(m_preferences);
	//m_preferences->show();
	//m_preferences->raise();
	//m_preferences->activateWindow();
}

void MainMenu::showAboutSielo()
{
	AboutDialog* dialog{new AboutDialog(m_tabWidget)};
	m_tabWidget->addApplication(dialog);
}

void MainMenu::showPartners()
{
	PartnerDialog* dialog{new PartnerDialog(m_tabWidget)};
	m_tabWidget->addApplication(dialog);
}

void MainMenu::showHelpUs()
{
	HelpUsDialog* dialog{new HelpUsDialog(m_tabWidget)};
	m_tabWidget->addApplication(dialog);
}

void MainMenu::openDiscord()
{
	m_tabWidget->addView(QUrl("https://discord.gg/7MVvDaS"));
}

void MainMenu::quit()
{
	Application::instance()->quitApplication();
}

void MainMenu::aboutToShowSideBarMenu()
{
	QMenu* menu = qobject_cast<QMenu*>(sender());
	Q_ASSERT(menu);

	if (m_tabWidget) {
		m_tabWidget->createSideBarsMenu(menu);
	}
}

void MainMenu::aboutToShowToolsMenu()
{
	if (!m_tabWidget)
		return;

	m_pluginsMenu->clear();
	Application::instance()->plugins()->populateExtensionsMenu(m_pluginsMenu, m_tabWidget);

	m_pluginsMenu->menuAction()->setVisible(!m_pluginsMenu->actions().isEmpty());
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
