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
#ifndef SIELO_BROWSER_MAINMENU_HPP
#define SIELO_BROWSER_MAINMENU_HPP

#include "SharedDefines.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include <QHash>
#include <QPointer>

namespace Sn
{
class TabWidget;
class PreferencesDialog;

class BookmarksMenu;
class MaquetteGridMenu;
class HistoryMenu;

class SIELO_SHAREDLIB MainMenu: public QMenu {
Q_OBJECT

public:
	MainMenu(TabWidget* tabWidget, QWidget* parent = nullptr);

	void initMenuBar(QMenuBar* menuBar);

	TabWidget* tabWidget() const { return m_tabWidget; }

	QAction *action(const QString& name) const;
	QAction *createAction(const QString& name, QMenu* menu, const QIcon& icon, const QString& trName,
	                      const QString& shortcut = QString());
public slots:
	void setTabWidget(TabWidget* tabWidget);
	void updateShowBookmarksBarText(bool visible);

private slots:
	void newTab();
	void newWindow();
	void newPrivateWindow();
	void openFile();
	void toggleBookmarksToolBar();

	// Edit menu
	void undo();
	void redo();
	void cut();
	void copy();
	void paste();
	void selectAll();
	void find();

	// View menu
	void stop();
	void reload();
	void zoomIn();
	void zoomOut();
	void zoomReset();
	void showPageSource();
	void showFullScreen();

	void webBack();
	void webForward();
	void webHome();

	void openUrl(const QUrl& url);

	// Tools menu
	void showDownloadManager();
	void showCookiesManager();
	void showSiteInfo();

	void showSettings();
	void showAboutSielo();
	void showPartners();
	void showHelpUs();
	void openDiscord();

	void quit();

	void aboutToShowSideBarMenu();
	void aboutToShowToolsMenu();

private:
	void addActionsToTabWidget();

	QAction* m_toggleBookmarksAction{nullptr};

	QMenu* m_sideBarsMenu{nullptr};
	QMenu* m_viewMenu{nullptr};
	BookmarksMenu* m_bookmarksMenu{nullptr};
	MaquetteGridMenu* m_maquetteGridMenu{nullptr};
	HistoryMenu* m_historyMenu{nullptr};
	QMenu* m_pluginsMenu{nullptr};
	QMenu* m_toolsMenu{nullptr};

	TabWidget* m_tabWidget{nullptr};

	QPointer<PreferencesDialog> m_preferences{};

	QHash<QString, QAction*> m_actions{};
};
}

#endif //SIELO_BROWSER_MAINMENU_HPP
