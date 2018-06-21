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
#ifndef SIELOBROWSER_WEBTAB_HPP
#define SIELOBROWSER_WEBTAB_HPP

#include <QVBoxLayout>
#include <QGridLayout>

#include <QWidget>
#include <QIcon>
#include <QPushButton>
#include <QSplitter>

#include <QMenu>
#include <QToolBar>

#include <QWebEngineHistory>

#include <QUrl>

#include <QShowEvent>

namespace Sn {
class BrowserWindow;

class TabWidget;

class WebInspector;
class TabbedWebView;

class TabIcon;
class MainTabBar;

class FloatingButton;
class AddressBar;

class WebTab: public QWidget {
Q_OBJECT

public:
	struct SavedTab {
		QString title{};
		QUrl url{};
		QIcon icon{};
		QByteArray history{};
		bool isPinned{};
		int zoomLevel{};

		SavedTab();
		SavedTab(WebTab* webTab);

		bool isValide() const;
		void clear();

		friend QDataStream& operator<<(QDataStream& stream, const SavedTab& tab);
		friend QDataStream& operator>>(QDataStream& stream, SavedTab& tab);
	};

	WebTab(BrowserWindow* window);

	WebInspector* inspector() const { return m_inspector; }
	TabbedWebView* webView() const { return m_webView; }
	TabIcon* tabIcon() const { return m_tabIcon; }
	AddressBar* addressBar() const { return m_addressBar; }
	MainTabBar* tabBar() const { return m_tabBar; }

	void takeAddressBar();

	QUrl url() const;
	QString title() const;
	QIcon icon(bool allowNull = false) const;
	QWebEngineHistory* history() const;
	int zoomLevel() const;

	void setZoomLevel(int level);

	void detach();
	void attach(TabWidget* tabWidget);

	void addToolBar(QToolBar* toolBar);
	void removeToolBar(QToolBar* toolBar);

	QByteArray historyData() const;
	void setHistoryData(const QByteArray& data);

	void stop();
	void reload();
	bool isLoading() const;

	bool isPinned() const;
	void setPinned(bool state);
	void togglePinned();

	bool isMuted() const;
	void setMuted(bool muted);
	void toggleMuted();

	int tabIndex() const;

	bool isCurrentTab() const;

	void showSearchToolBar();

	bool isRestored() const;
	void restoreTab(const SavedTab& tab);
	void p_restoreTab(const SavedTab& tab);
	void p_restoreTab(const QUrl& url, const QByteArray& history, int zoomLevel);

public slots:
	void sNewWindow();
	void sNewTab();
	void sGoHome();

	void showInspector();

private slots:
	void showNotification(QWidget* notif);
	void loadStarted();
	void loadFinished();
	void titleChanged(const QString& title);

	//	void aboutToShowHistoryNextMenu();
	void sRestore();

private:
	void showEvent(QShowEvent* event);

	QVBoxLayout* m_layout{nullptr};
	QSplitter* m_splitter{nullptr};

	QMenu* m_menuForward{nullptr};

	BrowserWindow* m_window{nullptr};
	WebInspector* m_inspector{nullptr};
	TabbedWebView* m_webView{nullptr};
	TabIcon* m_tabIcon{nullptr};
	MainTabBar* m_tabBar{nullptr};

	AddressBar* m_addressBar{nullptr};

	SavedTab m_savedTab{};
	bool m_isPinned{false};

	static bool s_pinningTab;
};
}
#endif //SIELOBROWSER_WEBTAB_HPP
