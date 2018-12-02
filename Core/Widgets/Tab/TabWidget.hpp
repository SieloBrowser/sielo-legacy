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
#ifndef SIELOBROWSER_TABWIDGET_HPP
#define SIELOBROWSER_TABWIDGET_HPP

#include "SharedDefines.hpp"

#include <QByteArray>
#include <QVector>

#include <QMenu>
#include <QToolBar>

#include <QWebEngineFullScreenRequest>
#include <QWebEngineDownloadItem>
#include <QWebEngineView>

#include "Application.hpp"

#include "Widgets/Tab/TabStackedWidget.hpp"

#include "Web/LoadRequest.hpp"
#include "Web/Tab/WebTab.hpp"

namespace Sn {
class MainTabBar;
class TabIcon;
class MenuTabs;
class AddTabButton;

class ClosedTabsManager;
class ToolButton;
class AutoSaver;

class StatusBarMessage;

class FloatingButton;
class NavigationToolBar;

class TabbedWebView;

class SIELO_SHAREDLIB TabWidget: public TabStackedWidget {
Q_OBJECT

public:
	TabWidget(BrowserWindow* window, Application::TabsSpaceType type = Application::TST_Web, QWidget* parent = nullptr);
	~TabWidget();

	void loadSettings();

	QByteArray saveState();
	bool restoreState(const QVector<WebTab::SavedTab>& tabs, int currentTab, const QUrl& homeUrl);

	void setCurrentIndex(int index);
	void goToApplication(QWidget* w);
		
	void nextTab();
	void previousTab();
	void currentTabChanged(int index);

	int normalTabsCount() const;
	int pinnedTabsCount() const;
	int extraReservedWidth() const;

	WebTab* webTab(int index = -1) const;

	QAction* action(const QString& name) const;

	BrowserWindow* window() const { return m_window; }
	MainTabBar* tabBar() const { return m_tabBar; }
	NavigationToolBar* navigationToolBar() const { return m_navigationToolBar; }
	ClosedTabsManager* closedTabsManager() const { return m_closedTabsManager; }
	QList<WebTab*> allTabs(bool withPinned = true);
	Application::TabsSpaceType type() const { return m_tabsSpaceType; }

	bool canRestoreTab() const;
	bool isCurrentTabFresh() const { return m_currentTabFresh; }
	void setCurrentTabFresh(bool currentTabFresh);

	QUrl urlOnNewTab() const { return m_urlOnNewTab; }
	QUrl homeUrl() const { return m_homeUrl; }
	void setHomeUrl(const QString& newUrl);

	StatusBarMessage* statusBarMessage() const { return m_statusBarMessage; }
	QStackedWidget* addressBars() const { return m_addressBars; }
	ToolButton* buttonClosedTabs() const { return m_buttonClosedTabs; }
	AddTabButton* buttonAddTab() const { return m_buttonAddTab; }

	void moveTab(int from, int to);
	int pinUnPinTab(int index, const QString &title = QString());

	void detachTab(WebTab* tab);

signals:
	void changed();
	void tabInserted(int index);
	void tabRemoved(int index);
	void tabMoved(int from, int to);

	void focusIn(TabWidget*);

public slots:
	void addTab();

	int addView(const QUrl& url);
	int addView(const LoadRequest& request, const Application::NewTabTypeFlags& openFlags, bool selectLine = false,
				bool pinned = false);
	int addView(const LoadRequest& request, const QString& title = tr("New Tab"),
				const Application::NewTabTypeFlags& openFlags = Application::NTT_SelectedTab, bool selectLine = false,
				int position = -1, bool pinned = false);
	int addView(WebTab* tab, const Application::NewTabTypeFlags& openFlags);
	int insertView(int index, WebTab* tab, const Application::NewTabTypeFlags& openFlags);

	int addApplication(QWidget* application);
	int insertApplication(int index, QWidget* application);

	void addTabFromClipboard();
	int duplicateTab(int index);

	void closeTab(int index = -1); // Force
	void requestCloseTab(int index = -1);

	void reloadTab(int index);
	void reloadAllTabs();
	void reload();
	void reloadBypassCache();
	void stopTab(int index);
	void closeAllButCurrent(int index);
	void closeToRight(int index);
	void closeToLeft(int index);
	void detachTab(int index);
	void loadTab(int index);
	void unloadTab(int index);
//	void detachTab(int index, QPoint position);
	void restoreClosedTab(QObject* obj = nullptr);
	void restoreAllClosedTabs();
	void clearClosedTabsList();

	void showSource();

	void showInspector(WebTab* webTab = nullptr);

	void fullScreenRequested(QWebEngineFullScreenRequest request);
	void toggleFullScreen();

	void moveAddTabButton(int posX);

	void updateShowBookmarksBarText(bool visible);

	void tabBarOverFlowChanged(bool overflowed);

	void openBookmarksDialog();
	void openHistoryDialog();
private slots:
	void save();

	void aboutToShowTabsMenu();
	void aboutToShowClosedTabsMenu();

	void actionChangeIndex();
	void tabWasMoved(int before, int after);

private:
	WebTab* weTab() const;
	WebTab* weTab(int index) const;
	TabIcon* tabIcon(int index) const;

	bool validIndex(int index) const;
	void updateClosedTabsButton();

	void setupNavigationBar();

	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;

	AutoSaver* m_saveTimer{nullptr};

	BrowserWindow* m_window{nullptr};
	MainTabBar* m_tabBar{nullptr};
	QStackedWidget* m_addressBars{nullptr};
	ClosedTabsManager* m_closedTabsManager;
	Application::TabsSpaceType m_tabsSpaceType{};

	MenuTabs* m_menuTabs{nullptr};
	ToolButton* m_buttonListTabs{nullptr};
	ToolButton* m_buttonClosedTabs{nullptr};
	AddTabButton* m_buttonAddTab{nullptr};
	AddTabButton* m_buttonAddTab2{nullptr};
	ToolButton* m_buttonMainMenu{nullptr};

	StatusBarMessage* m_statusBarMessage{nullptr};

	QPointer<WebTab> m_lastBackgroundTab{};

	QMenu* m_menuClosedTabs{nullptr};
	NavigationToolBar* m_navigationToolBar{nullptr};
	QUrl m_urlOnNewTab{};
	QUrl m_homeUrl{};

	bool m_dontCloseWithOneTab{false};
	bool m_showClosedTabsButton{false};
	bool m_newTabAfterActive{false};
	bool m_newEmptyTabAfterActive{false};
	bool m_currentTabFresh{false};

	QWebEngineView* m_fullScreenView{nullptr};
	QWidget* m_oldParent{nullptr};
	QSize m_oldSize{};
};

}

#endif //SIELOBROWSER_TABWIDGET_HPP
