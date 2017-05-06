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
#ifndef CORE_APPLICATION_HPP
#define CORE_APPLICATION_HPP

#include <QApplication>
#include <QList>

#include <QPointer>

#include <QWebEngineProfile>

#include "Utils/RestoreManager.hpp"

namespace Sn {
class PluginProxy;
class HistoryManager;
class BookmarksManager;
class DownloadManager;
class HTML5PermissionsManager;
class NetworkManager;

class BrowserWindow;

class Application: public QApplication {
public:
	enum CommandLineAction {
		CL_NoAction,
		CL_OpenUrl,
		CL_OpenUrlInCurrentTab,
		CL_OpenUrlInNewWindow,
		CL_NewTab,
		CL_NewWindow,
		CL_StartPrivateBrowsing,
		CL_StartNewInstance,
		CL_ExitAction
	};
	enum ObjectName {
		ON_WebView,
		ON_TabBar,
		ON_BrowserWindow
	};

	enum WindowType {
		WT_FirstAppWindow,
		WT_NewWindow,
		WT_OtherRestoredWindow
	};

	enum NewTabType {
		NTT_SelectedTab = 1,
		NTT_NotSelectedTab = 2,
		NTT_CleanTab = 4,
		NTT_TabAtEnd = 8,
		NTT_NewEmptyTab = 16,
		/* ------------------------- */
			NTT_SelectedNewEmptyTab = NTT_SelectedTab | NTT_TabAtEnd | NTT_NewEmptyTab,
		NTT_SelectedTabAtEnd = NTT_SelectedTab | NTT_TabAtEnd,
		NTT_NotSelectedTabAtEnd = NTT_NotSelectedTab | NTT_TabAtEnd,
		NTT_CleanSelectedTabAtEnd = NTT_SelectedTab | NTT_TabAtEnd | NTT_CleanTab,
		NTT_CleanSelectedTab = NTT_CleanTab | NTT_SelectedTab,
		NTT_CleanNotSelectedTab = NTT_CleanTab | NTT_NotSelectedTab
	};
	Q_DECLARE_FLAGS(NewTabTypeFlags, NewTabType);

	enum Path {
		P_Data = 0,
		P_Plugin = 1,
		P_Themes = 2
	};

	enum AfterLaunch {
		OpenBlankPage = 0,
		OpenHomePage = 1,
		RestoreSession = 2
	};

	Application(int& argc, char** argv);
	~Application();

	void loadSettings();

	bool privateBrowsing() const { return m_privateBrowsing; }

	int windowCount() const;
	QList<BrowserWindow*> windows() const { return m_windows; }

	BrowserWindow* getWindow() const;
	BrowserWindow* createWindow(Application::WindowType type, const QUrl& startUrl = QUrl());

	AfterLaunch afterLaunch() const;

	bool restoreSession(BrowserWindow* window, RestoreData restoreData);
	void destroyRestoreManager();

	PluginProxy* plugins() { return m_plugins; }
	HistoryManager* historyManager();
	BookmarksManager* bookmarksManager();
	DownloadManager* downloadManager();
	HTML5PermissionsManager* permissionsManager();

	NetworkManager* networkManager() const { return m_networkManager; }
	RestoreManager* restoreManager() const { return m_restoreManager; }
	QWebEngineProfile* webProfile() const { return m_webProfile; }

	bool useTopToolBar() const { return m_useTopToolBar; }

	QString ensureUniqueFilename(const QString& name, const QString& appendFormat = QString("(%1)"));

	static QList<QString> paths();
	static Application* instance();

public slots:
	void saveSession();

	void reloadUserStyleSheet();

private slots:
	void postLaunch();

	void windowDestroyed(QObject* window);

private:
	enum PostLaunchAction {
		OpenNewTab
	};

	void setUserStyleSheet(const QString& filePath);

	void loadTheme(const QString& name);
	void loadThemeFromResources();

	bool m_privateBrowsing{false};
	bool m_isRestoring{false};
	bool m_useTopToolBar{false};

	PluginProxy* m_plugins{nullptr};
	HistoryManager* m_historyManager{nullptr};
	BookmarksManager* m_bookmarksManager{nullptr};
	DownloadManager* m_downloadManager{nullptr};
	HTML5PermissionsManager* m_permissionsManager{nullptr};

	NetworkManager* m_networkManager{nullptr};
	QWebEngineProfile* m_webProfile{nullptr};

	RestoreManager* m_restoreManager{nullptr};

	QList<BrowserWindow*> m_windows;
	QPointer<BrowserWindow> m_lastActiveWindow;

	QList<PostLaunchAction> m_postLaunchActions;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Application::NewTabTypeFlags);

}
#endif // CORE_APPLICATION_HPP
