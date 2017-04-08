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
#ifndef SIELOBROWSER_TABWIDGET_HPP
#define SIELOBROWSER_TABWIDGET_HPP

#include <QByteArray>
#include <QVector>

#include <QMenu>

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

class TabbedWebView;

class TabWidget: public TabStackedWidget {
Q_OBJECT

public:
	TabWidget(BrowserWindow* window, QWidget* parent = nullptr);
	~TabWidget();

	QByteArray saveState();
	bool restoreState(const QVector<WebTab::SavedTab>& tabs, int currentTab);
	void closeRecoveryTab();

	void setCurrentIndex(int index);

	void nextTab();
	void previousTab();
	void currentTabChanged(int index);

	int normalTabsCount() const;
	int pinnedTabsCount() const;
	int lastTabIndex() const;
	int extraReservedWidth() const;

	MainTabBar* tabBar() const { return m_tabBar; }
	ClosedTabsManager* closedTabsManager() const { return m_closedTabsManager; }
	QList<WebTab*> allTabs(bool withPinned = true);
	bool canRestoreTab() const;
	bool isCurrentTabFresh() const { return m_currentTabFresh; }
	void setCurrentTabFresh(bool currentTabFresh);

	ToolButton* buttonClosedTabs() const { return m_buttonClosedTabs; }
	AddTabButton* buttonAddTab() const { return m_buttonAddTab; }

signals:
	void changed();

public slots:
	int addView(const LoadRequest& request, const Application::NewTabTypeFlags& openFlags, bool selectLine = false,
				bool pinned = false);
	int addView(const LoadRequest& request, const QString& title = tr("New Tab"),
				const Application::NewTabTypeFlags& openFlags = Application::NTT_SelectedTab, bool selectLine = false,
				int position = -1, bool pinned = false);
	int addView(WebTab* tab);

	void addTabFromClipboard();
	int duplicateTab(int index);

	void closeTab(int index = -1);
	void requestCloseTab(int index = -1);

	void reloadTab(int index);
	void reloadAllTabs();
	void stopTab(int index);
	void closeAllButCurrent(int index);
	void closeToRight(int index);
	void closeToLeft(int index);
	void detachTab(int index);
	void restoreClosedTab(QObject* obj = nullptr);
	void restoreAllClosedTabs();
	void clearClosedTabsList();

	void moveAddTabButton(int posX);

	void tabBarOverFlowChanged(bool overflowed);

private slots:
	void loadSettings();
	void save();

	void aboutToShowTabsMenu();
	void aboutToShowClosedTabsMenu();

	void actionChangeIndex();
	void tabMoved(int before, int after);

private:
	WebTab* weTab();
	WebTab* weTab(int index);
	TabIcon* tabIcon(int index);

	bool validIndex(int index) const;
	void updateClosedTabsButton();

	AutoSaver* m_saveTimer{nullptr};

	BrowserWindow* m_window{nullptr};
	MainTabBar* m_tabBar{nullptr};
	ClosedTabsManager* m_closedTabsManager;
	MenuTabs* m_menuTabs{nullptr};
	ToolButton* m_buttonListTabs{nullptr};
	ToolButton* m_buttonClosedTabs{nullptr};
	AddTabButton* m_buttonAddTab{nullptr};
	AddTabButton* m_buttonAddTab2{nullptr};

	QMenu* m_menuClosedTabs{nullptr};
	QUrl m_urlOnNewTab{};

	int m_lastTabIndex{-1};
	int m_lastBackgroundTabIndex{-1};
	bool m_dontCloseWithOneTab{false};
	bool m_showClosedTabsButton{false};
	bool m_newTabAfterActive{false};
	bool m_newEmptyTabAfterActive{false};
	bool m_currentTabFresh{false};

};

}

#endif //SIELOBROWSER_TABWIDGET_HPP