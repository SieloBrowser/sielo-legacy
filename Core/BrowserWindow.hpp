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

class MaquetteGridItem;

//! Represent a window of the browser.
/*!
 * This class provide various access to make operation in a window of Sielo.
 */
class BrowserWindow: public QMainWindow {
Q_OBJECT

public:
	//! Tabs Space Position.
	/* Position where a tabs space can be create. */
	enum TabsSpacePosition {
		TSP_Left, /*!< Create tabs space to the left. */
		TSP_Right, /*!< Create tabs space to the right. */
		TSP_Top, /*!< Create tabs space to the top. */
		TSP_Bottom /*!< Create tabs space to the bottom. */
	};

	/*!
	 * This constructor build the window.
	 * 
	 * This construct **should never be called manually**. Prefere using the `createWindow` methode from the Application class. 
	 */
	BrowserWindow(Application::WindowType type, const QUrl& url = QUrl());
	/*!
	 * This constrcutor allow to create a window an load a maquetteGrid.
	 * 
	 * It calls the first BrowserWindow constructor with "RestoredWindow". 
	 * Therefore, as the other constrcutor this construct **should never be called manually**. Prefere using the `createWindow` methode from the Application class. 
	 */
	BrowserWindow(MaquetteGridItem* maquetteGrid);
	~BrowserWindow();

	/*! 
	 * Load settings.
	 * 
	 *  - Home page.
	 *  - Padding between tabs space.
	 *  - Background.
	 *  - Show bookmarks toolbar.
	 */
	void loadSettings();

	/*!
	 * Return encoded data that can be stored as binary of current tabs spaces.
	 */
	QByteArray saveTabs();

	void setStartTab(WebTab* tab);
	void setStartPage(WebPage* page);

	void restoreWindowState(const RestoreManager::WindowData& data);

	void currentTabChanged(WebTab* oldTab);

	/*!
	 * Create a new tabs space.
	 * 
	 * This function create a new tabs space with the given tab to the given position.
	 * @param position The position where the tabs space must be created.
	 * @param tab The tab of the tabs space.
	 * @param from The tab widget who requested a new tabs space, if one.
	 */
	void createNewTabsSpace(TabsSpacePosition position, WebTab* tab, TabWidget* from = nullptr);
	void createNewTabsSpace(TabsSpacePosition position, Application::TabsSpaceType type, WebTab* tab = nullptr);
	/*!
	 * Insert a tabs space.
	 * 
	 * This function insert the tabs space into the window.
	 * @param position The position where the tabs space must be created.
	 * @param widgetTabWidget The tabs space itself (it generally have been builded with `createWidgetTabWidget`).
	 * @param from The tab widget who requested a new tabs space.
	 */
	void insertTabsSpace(TabsSpacePosition position, QWidget* widgetTabWidget, TabWidget* from);
	/*!
	 * Close a tabs space.
	 * 
	 * @param tabWidget The tab widget of the tabs space. All parents will be deduced.
	 */
	void closeTabsSpace(TabWidget* tabWidget);
	/*!
	 * Resize all tabs space to be a maximum equals.
	 */
	void autoResizeTabsSpace();

	/*!
	 * Load an url in the current tab.
	 */
	void loadUrl(const QUrl& url);
	/*!
	 * Load an url in a new tab
	 */
	void loadUrlInNewTab(const QUrl& url);

	/*!
	 * This return the maquetteGrid corresponding to the current session.
	 * @return The maquetteGrid corresponding to the current session.
	 */
	MaquetteGridItem* maquetteGridItem() const;

	QUrl homePageUrl() const { return m_homePage; }

	TabbedWebView* webView() const;
	TabbedWebView* webView(int index) const;

	TabWidget* tabWidget() const;
	TabWidget* tabWidget(int index) const;
	QVector<TabWidget*> tabWidgets() const { return m_tabWidgets; }
	int tabWidgetsCount() const;

	StatusBarMessage* statusBarMessage() const { return m_statusBarMessage; }
	TitleBar* titleBar() const { return m_titleBar; }

	const QImage* background();
	const QImage* processedBackground();

signals:
	void mouseOver(bool state);

public slots:
	void setWindowTitle(const QString& title);

	void enterHtmlFullScreen();
	void toggleFullScreen();
	void tabsSpaceInFullView(QWidget* widget);
	void arrangeTabsSpaces();

	void bookmarkPage();
	void bookmarkAllTabs();
	void addBookmark(const QUrl& url, const QString& title);

protected:
	void shotBackground();
	QImage applyBlur(const QImage *src, qreal radius, bool quality = true, bool alphaOnly = false, int transposed = 0);
	void paintEvent(QPaintEvent* event);
	void resizeEvent(QResizeEvent* event);
	void mouseMoveEvent(QMouseEvent *e);

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

	qreal m_blur_radius{ 0 };

	RootFloatingButton* m_fButton{nullptr};

	QImage* m_bg{ nullptr };
	QImage* m_blur_bg{ nullptr };
	bool m_upd_ss{ false };
};

}

#endif //SIELOBROWSER_BROWSERWINDOW_HPP
