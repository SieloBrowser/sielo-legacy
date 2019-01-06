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
#ifndef CORE_APPLICATION_HPP
#define CORE_APPLICATION_HPP

#include "SharedDefines.hpp"

#include <QApplication>
#include <QList>

#include <QPointer>

#include <QFont>

#include "3rdparty/SingleApplication/singleapplication.h"
#include "3rdparty/Piwik/piwiktracker.h"

#include <QWebEngine/WebProfile.hpp>
#include <QWebEngine/DownloadItem.hpp>

namespace Sn
{
class AutoFill;
class CookieJar;
class PluginProxy;
class History;
class Bookmarks;
class MaquetteGrid;
class DownloadManager;
class HTML5PermissionsManager;
class NetworkManager;

class SideBarInterface;

struct RestoreData;
class RestoreManager;

class BrowserWindow;

class MaquetteGridItem;

//! The This class manage Sielo instance, windows and settings.
/*!
 * This application give access to all settings and informations on Sielo. There is usually only **one** instance of this class for the entire browser
 *
 * This class will also hande OS signals like new tab request, links open request, etc.
 *
 */
class SIELO_SHAREDLIB Application: public SingleApplication {
	Q_OBJECT

public:
	//! Command line actions
	/*! All command line option that can be requested to Sielo. This can be request from OS. */
	enum CommandLineAction {
		CL_NoAction,
		/*!< No action requested */
		CL_OpenUrl,
		/*!< We want to open a URL */
		CL_OpenUrlInCurrentTab,
		/*!< We want to open the url in the current tab */
		CL_OpenUrlInNewWindow,
		/*!< We want to open the url in a new window */
		CL_StartWithProfile,
		/*!< We want to start with a profile */
		CL_NewTab,
		/*!< We want to open a new tab */
		CL_NewWindow,
		/*!< We want to open a new window */
		CL_StartPrivateBrowsing,
		/*!< We want to open a new private browsing window */
		CL_StartNewInstance,
		/*!< We want to start a new instance of Sielo */
		CL_ExitAction /*!< We want to close Sielo */
	};

	//! Object name
	/*! Name of object accessible for applications. It can be used when one of this object emit a signal for all applications running. */
	enum ObjectName {
		ON_WebView, /*!< Web view object */
		ON_TabBar, /*!< Tab bar object */
		ON_TabWidget, /*!< Tab space object */
		ON_BrowserWindow /*!< Browser window object */
	};

	//! Window type
	/*! Describe the type of the window. */
	enum WindowType {
		WT_FirstAppWindow,
		/*!< The window is the first window of the application */
		WT_NewWindow,
		/*!< The window is a simple new window */
		WT_OtherRestoredWindow /*!< The window is a restored window */
	};

	//! Tabs space type
	/*! Type of the tabs space */
	enum TabsSpaceType {
		TST_Web,
		/*!< The tabs space run application */
		TST_Application /*!< The tabs space run web pages */
	};

	//! Tab type
	/*! Type of the new tab */
	enum NewTabType {
		NTT_SelectedTab = 1,
		/*!< The tab will be selected */
		NTT_NotSelectedTab = 2,
		/*!< The tab will not be selected */
		NTT_CleanTab = 4,
		/*!< The tab will be clean */
		NTT_TabAtEnd = 8,
		/*!< The tab will be at the end */
		NTT_NewEmptyTab = 16,
		/*!< The tab will be blank */
		/* ------------------------- */
		NTT_SelectedNewEmptyTab = NTT_SelectedTab | NTT_TabAtEnd | NTT_NewEmptyTab,
		NTT_SelectedTabAtEnd = NTT_SelectedTab | NTT_TabAtEnd,
		NTT_NotSelectedTabAtEnd = NTT_NotSelectedTab | NTT_TabAtEnd,
		NTT_CleanSelectedTabAtEnd = NTT_SelectedTab | NTT_TabAtEnd | NTT_CleanTab,
		NTT_CleanSelectedTab = NTT_CleanTab | NTT_SelectedTab,
		NTT_CleanNotSelectedTab = NTT_CleanTab | NTT_NotSelectedTab
	};

	Q_DECLARE_FLAGS(NewTabTypeFlags, NewTabType);

	//! After launch action
	/*! Action that should be exectued after Sielo startup */
	enum AfterLaunch {
		OpenBlankPage = 0,
		/*!< Open a blank page */
		OpenHomePage = 1,
		/*!< Open the home page */
		RestoreSession = 2,
		/*!< Restore the previous session */
		OpenSavedSession = 3 /*!< Restore the saved session */
	};

	/*!
	 * This constructor call functions to load SQL database and load settings.
	 *
	 * It will also determine command line options given and check for updates.
	 */
	Application(int& argc, char** argv);
	/*!
	 * The destructor first request to window to save the current session.
	 *
	 * The it remove any window system resources thar were allocated by this application, and free some managers.
	 */
	~Application();

	void loadSettings();
	void loadWebSettings();
	void loadApplicationSettings();
	void loadThemesSettings();
	void loadPluginsSettings();
	void loadTranslationSettings();
	void updateToProfiles();

	void translateApplication();
	QString currentLanguageFile() const { return m_languageFile; }
	QString currentLanguage() const;

	/*!
	 * Apply a theme to Sielo.
	 * @param name Name of the theme we want to apply.
	 * @param lightness Needed to let the theme know if it should load light or dark icons.
	 */
	void loadTheme(const QString& name, const QString& lightness = "dark");
	QString parseSSS(QString& sss, const QString& relativePath, const QString& lightness);
	QString parseSSSBackground(QString& sss, const QString& relativePath);
	QString parseSSSColor(QString& sss, const QString& lightness);
	QString getBlurredBackgroundPath(const QString& defaultBackground, int radius);
	QImage blurImage(const QImage& image, const QRect& rect, int radius, bool alphaOnly = false);

	bool privateBrowsing() const { return m_privateBrowsing; }
	bool isPortable() const { return m_isPortable; }
	bool is32bit() const { return m_is32bit; }
	bool isStartingAfterCrash() const { return m_startingAfterCrash; }
	bool isClosing() const { return m_isClosing; }

	int windowCount() const;

	QList<BrowserWindow*> windows() const { return m_windows; }

	BrowserWindow *getWindow() const;
	/*!
	 * Create a new window in this instance.
	 * @param type The type of the new window.
	 * @param startUrl The start url for the new window (if it's empty, it will take the home page url).
	 * @return The new window created.
	 */
	BrowserWindow *createWindow(Application::WindowType type, const QUrl& startUrl = QUrl());
	/*!
	 * Create a new window from a maquetteGrid in this instance.
	 * @param item The maquetteGrid to load.
	 * @return The new window create.
	 */
	BrowserWindow *createWindow(MaquetteGridItem* item);

	AfterLaunch afterCrashLaunch() const { return m_afterCrashLaunch; }
	AfterLaunch afterLaunch() const;

	void openSession(BrowserWindow* window, RestoreData& restoreData);
	bool restoreSession(BrowserWindow* window, RestoreData restoreData);
	void destroyRestoreManager();

	void addSidebar(const QString& id, SideBarInterface* sideBarInterface);
	void removeSidebar(SideBarInterface* sideBarInterface);
	QHash<QString, SideBarInterface*> sidebars() const { return m_sidebars; }

	PluginProxy *plugins() const { return m_plugins; }
	AutoFill *autoFill() const { return m_autoFill; }
	CookieJar *cookieJar();
	History *history();
	Bookmarks *bookmarks();
	MaquetteGrid *maquetteGrid();
	DownloadManager *downloadManager();
	HTML5PermissionsManager *permissionsManager();
	NetworkManager *networkManager() const { return m_networkManager; }
	RestoreManager *restoreManager() const { return m_restoreManager; }

	Engine::WebProfile *webProfile();

	PiwikTracker *piwikTraker() { return m_piwikTracker; }

	bool fullyLoadThemes() const { return m_fullyLoadThemes; }
	bool showFloatingButton() const { return m_showFloatingButton; }
	bool hideToolbarControls() const { return m_hideToolbarControls; }
	bool hideBookmarksHistoryActions() const { return m_hideBookmarksHistoryActions; }
	bool floatingButtonFoloweMouse() const { return m_floatingButtonFoloweMouse; }

	void startAfterCrash();

	QFont morpheusFont() const { return m_morpheusFont; }
	QFont normalFont() const { return m_normalFont; }

	bool copyPath(const QString& fromDir, const QString& toDir, bool coverFileIfExist = true);
	QString readFile(const QString& filename);

	/*!
	 * Process a command in Sielo. There is serval command available for now :
	 *
	 *  - site : open Sielo site.
	 *  - github : open the Sielo repository.
	 *  - witcher [enable/disable] : enable or disable witcher font in the browser.
	 *  - easteregg : open a random crasy site.
	 * @param command The command name.
	 * @param args The command arguments.
	 */
	void processCommand(const QString& command, const QStringList args);

	static QString currentVersion;
	static Application *instance();
	static QIcon getAppIcon(const QString& name, const QString& defaultDire = "other", const QString& format = ".png");
	static QString getFileNameFromUrl(const QUrl &url);
	static QByteArray readAllFileByteContents(const QString& filename);
	static QString ensureUniqueFilename(const QString& name, const QString& appendFormat = QString("(%1)"));
	static void removeDirectory(const QString& directory);

signals:
	void activeWindowChanged(BrowserWindow* window);

public slots:
	/*!
	 * Add a new tab to the current tabs space.
	 * @param url The url open in the new tab. By default it's the user new tab url.
	 */
	void addNewTab(const QUrl& url = QUrl());
	/*!
	 * Start a new private browsing instance.
	 * @param startUrl Url open in the new private browsing window. By default it's the user home page url.
	 */
	void startPrivateBrowsing(const QUrl& startUrl = QUrl());

	/*!
	 * Save application settings.
	 */
	void saveSettings();
	/*!
	 * Save current session.
	 * @param saveForHome If true, the saved session will be the user saved session, else, it will be the restore session.
	 */
	void saveSession(bool saveForHome = false);

	void reloadUserStyleSheet();

	void quitApplication();

private slots:
	void postLaunch();

	void messageReceived(quint32 instanceId, QByteArray messageBytes);
	void windowDestroyed(QObject* window);
	void onFocusChanged();

	void downloadRequested(Engine::DownloadItem* download);

private:
	enum PostLaunchAction {
		OpenNewTab
	};

	void setUserStyleSheet(const QString& filePath);

	void loadThemeFromResources(QString name = "sielo-default", bool loadAtEnd = true);

	QString m_languageFile{};

	bool m_privateBrowsing{false};
	bool m_isPortable{true};
	bool m_is32bit{false};
	bool m_startingAfterCrash{false};
	bool m_isRestoring{false};
	bool m_isClosing{false};
	bool m_fullyLoadThemes{true};
	bool m_showFloatingButton{false};
	bool m_hideToolbarControls{false};
	bool m_hideBookmarksHistoryActions{false};
	bool m_floatingButtonFoloweMouse{true};
	bool m_databaseConnected{false};

	AfterLaunch m_afterCrashLaunch{AfterLaunch::OpenHomePage};

	PluginProxy* m_plugins{nullptr};
	AutoFill* m_autoFill{nullptr};
	CookieJar* m_cookieJar{nullptr};
	History* m_history{nullptr};
	Bookmarks* m_bookmarks{nullptr};
	MaquetteGrid* m_maquetteGrid{nullptr};
	DownloadManager* m_downloadManager{nullptr};
	HTML5PermissionsManager* m_permissionsManager{nullptr};

	NetworkManager* m_networkManager{nullptr};
	Engine::WebProfile* m_webProfile{nullptr};

	RestoreManager* m_restoreManager{nullptr};

	QList<BrowserWindow*> m_windows;
	QPointer<BrowserWindow> m_lastActiveWindow;

	QList<PostLaunchAction> m_postLaunchActions;
	QHash<QString, SideBarInterface*> m_sidebars;

	PiwikTracker* m_piwikTracker{nullptr};

	QFont m_morpheusFont{};
	QFont m_normalFont{};
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Application::NewTabTypeFlags);
}
#endif // CORE_APPLICATION_HPP
