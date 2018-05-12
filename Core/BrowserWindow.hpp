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
#ifndef SIELOBROWSER_BROWSERWINDOW_HPP
#define SIELOBROWSER_BROWSERWINDOW_HPP

#include <QMainWindow>

#include <QVBoxLayout>
#include <QSplitter>

#include <QVector>

#include <QUrl>

#include <QResizeEvent>

#include "Utils/RestoreManager.hpp"
#include "Widgets/FloatingButton.hpp"

#include "Application.hpp"

namespace Sn {
class WebPage;
class WebTab;
class TabbedWebView;

class TabWidget;

class RootFloatingButton;
class StatusBarMessage;
class TitleBar;

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

	void setStartTab(WebTab* tab);
	void setStartPage(WebPage* page);

	void restoreWindowState(const RestoreManager::WindowData& data);

	void currentTabChanged(WebTab* oldTab);

	void createNewTabsSpace(TabsSpacePosition position, WebTab* tab, TabWidget* from = nullptr);
	void createNewTabsSpace(TabsSpacePosition position, Application::TabsSpaceType type, WebTab* tab = nullptr);
	void insertTabsSpace(TabsSpacePosition position, QWidget* widgetTabWidget, TabWidget* from);
	void closeTabsSpace(TabWidget* tabWidget);
	void autoResizeTabsSpace();

	void loadUrl(const QUrl& url);
	void loadUrlInNewTab(const QUrl& url);

	QUrl homePageUrl() const { return m_homePage; }

	TabbedWebView* webView() const;
	TabbedWebView* webView(int index) const;

	TabWidget* tabWidget() const;
	TabWidget* tabWidget(int index) const;
	int tabWidgetsCount() const;

	StatusBarMessage* statusBarMessage() const { return m_statusBarMessage; }
	TitleBar* titleBar() const { return m_titleBar; }

public slots:
	void setWindowTitle(const QString& title);

	void enterHtmlFullScreen();
	void toggleFullScreen();
	void tabsSpaceInFullView(QWidget* widget);
	void arrangeTabsSpaces();

	void bookmarkAllTabs();

protected:
	void resizeEvent(QResizeEvent* event);

private slots:
	void addTab();
	void postLaunch();

	void tabWidgetIndexChanged(TabWidget* tbWidget);
	void floatingButtonPatternChange(RootFloatingButton::Pattern pattern);

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

	QWidget* createWidgetTabWidget(WebTab* tab = nullptr, Application::TabsSpaceType type = Application::TST_Web);
	QWidget* createWidgetTabWidget(TabWidget* tabWidget, WebTab* tab = nullptr);

	QAction* m_restoreAction{nullptr};

	QUrl m_startUrl{};
	QUrl m_homePage{};
	Application::WindowType m_windowType{};
	WebTab* m_startTab{nullptr};
	WebPage* m_startPage{nullptr};
	int m_spaceBetweenTabsSpaces{7};

	QVBoxLayout* m_layout{nullptr};
	QSplitter* m_mainSplitter{nullptr};

	QVector<TabWidget*> m_tabWidgets;
	StatusBarMessage* m_statusBarMessage{nullptr};
	TitleBar* m_titleBar{nullptr};

	int m_currentTabWidget{0};

	RootFloatingButton* m_fButton{nullptr};
};

}

#endif //SIELOBROWSER_BROWSERWINDOW_HPP
