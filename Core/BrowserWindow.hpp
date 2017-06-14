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
#ifndef SIELOBROWSER_BROWSERWINDOW_HPP
#define SIELOBROWSER_BROWSERWINDOW_HPP

#include <QMainWindow>

#include <QVBoxLayout>
#include <QSplitter>

#include <QVector>

#include <QUrl>

#include "Utils/RestoreManager.hpp"

#include "Application.hpp"

namespace Sn {
class WebPage;
class WebTab;
class TabbedWebView;

class FloatingButton;

class TabWidget;

class BrowserWindow: public QMainWindow {
Q_OBJECT

public:
	enum TabsSpacePosition {
		TSP_Left,
		TSP_Right,
		TSP_Top,
		TSP_Bottom
	};

	BrowserWindow(Application::WindowType type, const QUrl& url = QUrl());
	~BrowserWindow();

	void loadSettings();

	QByteArray saveTabs();
	void restoreTabs();

	void setStartTab(WebTab* tab);
	void setStartPage(WebPage* page);

	void restoreWindowState(const RestoreManager::WindowData& data);

	void currentTabChanged(WebTab* oldTab);

	void createNewTabsSpace(TabsSpacePosition position, WebTab* tab);
	void createNewTabsSpace(TabsSpacePosition position, TabWidget* tabWidget);
	void closeTabsSpace(TabWidget* tabWidget);
	void autoResizeTabsSpace();

	QUrl homePageUrl() const { return m_homePage; }

	TabbedWebView* webView() const;
	TabbedWebView* webView(int index) const;

	TabWidget* tabWidget() const;
	TabWidget* tabWidget(int index) const;
	int tabWidgetsCount() const;

	FloatingButton* fButton() const { return m_fButton; }

	FloatingButton* fButtonNewWindow() const { return m_fButtonNewWindow; }
	FloatingButton* fButtonHome() const { return m_fButtonHome; }
	FloatingButton* fButtonNext() const { return m_fButtonNext; }
	FloatingButton* fButtonBack() const { return m_fButtonBack; }
	FloatingButton* fButtonNewTab() const { return m_fButtonNewTab; }

public slots:
	void enterHtmlFullScreen();

	void bookmarkAllTabs();

private slots:
	void addTab();
	void postLaunch();

	void tabWidgetIndexChanged(TabWidget* tbWidget);

	void newWindow();
	void goHome();
	void forward();
	void back();
	void newTab();
	void openAddBookmarkDialog();
private:
	void setupUi();
	void setupFloatingButton();

	void saveButtonState();

	QWidget* createWidgetTabWidget(WebTab* tab = nullptr);

	QAction* m_restoreAction{nullptr};

	QUrl m_startUrl{};
	QUrl m_homePage{};
	Application::WindowType m_windowType{};
	WebTab* m_startTab{nullptr};
	WebPage* m_startPage{nullptr};

	QVBoxLayout* m_layout{nullptr};
	QSplitter* m_mainSplitter{nullptr};

	QVector<TabWidget*> m_tabWidgets;

	int m_currentTabWidget{0};

	FloatingButton* m_fButton{nullptr};

	FloatingButton* m_fButtonAddBookmark{nullptr};
	FloatingButton* m_fButtonViewBookmarks{nullptr};
	FloatingButton* m_fButtonViewHistory{nullptr};
	FloatingButton* m_fButtonNewWindow{nullptr};
	FloatingButton* m_fButtonHome{nullptr};
	FloatingButton* m_fButtonNext{nullptr};
	FloatingButton* m_fButtonBack{nullptr};
	FloatingButton* m_fButtonNewTab{nullptr};

};

}

#endif //SIELOBROWSER_BROWSERWINDOW_HPP
