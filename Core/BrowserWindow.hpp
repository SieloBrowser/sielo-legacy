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

#include "SharedDefines.hpp"

#include <QMainWindow>

#include <QVBoxLayout>
#include <QSplitter>

#include <QVector>

#include <QUrl>

#include <QResizeEvent>

#include "Widgets/Tab/TabsSpaceSplitter.hpp"
#include "Widgets/FloatingButton.hpp"

#include "Application.hpp"

namespace Sn {
class WebPage;
class WebTab;
class TabbedWebView;

class TabWidget;

class RootFloatingButton;
class TitleBar;

class MaquetteGridItem;

//! Represent a window of the browser.
/*!
 * This class provide various access to make operation in a window of Sielo.
 */
class SIELO_SHAREDLIB BrowserWindow: public QMainWindow {
Q_OBJECT

public:
	struct SavedWindow {
		QByteArray windowState{};
		QByteArray windowGeometry{};
		QVector<TabsSpaceSplitter::SavedTabsSpace> tabsSpaces{};

		SavedWindow();
		SavedWindow(BrowserWindow* window);
		SavedWindow(MaquetteGridItem* maquetteGridItem);

		bool isValid() const;
		void clear();

		friend QDataStream &operator<<(QDataStream &stream, const SavedWindow &window);
		friend QDataStream &operator>>(QDataStream &stream, SavedWindow &window);
	};

	/*!
	 * This constructor build the window.
	 * 
	 * This construct **should never be called manually**. Prefere using the `createWindow` methode from the Application class. 
	 */
	BrowserWindow(Application::WindowType type, const QUrl& url = QUrl());
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
	void loadWallpaperSettings();

	void setStartTab(WebTab* tab);
	void setStartPage(WebPage* page);

	void restoreWindowState(const SavedWindow& window);

	void currentTabChanged(WebTab* tab);

	/*!
	 * Load an url in the current tab.
	 */
	void loadUrl(const QUrl& url);
	/*!
	 * Load an url in a new tab
	 */
	void loadUrlInNewTab(const QUrl& url);

	/*!
	 * This return the URL for the home page (doosearch.sielo.app at default).
	 * @return The URL for the home page.
	 */
	QUrl homePageUrl() const { return m_homePage; }

	TabWidget* tabWidget() const;
	TabWidget* tabWidget(int index) const;
	TabsSpaceSplitter* tabsSpaceSplitter() const { return m_tabsSpaceSplitter; }

	TitleBar* titleBar() const { return m_titleBar; }

	const QImage* background();
	const QImage* processedBackground();

	void setCaption(const QWidget* widget);

signals:
	void mouseOver(bool state);
	void tabWidgetChanged(TabWidget* tabWidget);

public slots:
	void setWindowTitle(const QString& title);

	void enterHtmlFullScreen();
	void toggleFullScreen();

	void bookmarkPage();
	void bookmarkAllTabs();
	void addBookmark(const QUrl& url, const QString& title);

	void tabWidgetIndexChanged(TabWidget* tbWidget);

protected:
	void shotBackground();
	QImage applyBlur(const QImage *src, qreal radius, bool quality = true, bool alphaOnly = false, int transposed = 0);
	void paintEvent(QPaintEvent* event);
	void resizeEvent(QResizeEvent* event);
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mouseMoveEvent(QMouseEvent *e);

#ifdef Q_OS_WIN
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif

private slots:
	void addTab();
	void postLaunch();

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

#ifdef Q_OS_WIN
	long ncHitTest(const MSG* wMsg) const;
#endif

	const QWidget *m_captionWidget{ nullptr };

	QAction* m_restoreAction{nullptr};

	QUrl m_startUrl{};
	QUrl m_homePage{};
	Application::WindowType m_windowType{};
	WebTab* m_startTab{nullptr};
	WebPage* m_startPage{nullptr};

	TabsSpaceSplitter* m_tabsSpaceSplitter{nullptr};
	TitleBar* m_titleBar{nullptr};

	qreal m_blur_radius{ 0 };

	RootFloatingButton* m_fButton{nullptr};

	QTimer* m_backgroundTimer{nullptr};
	QImage m_currentBackground{};
	QImage* m_bg{ nullptr };
	QImage* m_blur_bg{ nullptr };
	bool m_upd_ss{ false };
};

}

#endif //SIELOBROWSER_BROWSERWINDOW_HPP
