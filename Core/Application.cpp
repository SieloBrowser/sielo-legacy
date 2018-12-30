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

#include "Application.hpp"

#include <QStandardPaths>
#include <QDir>

#include <QStyle>
#include <QGraphicsBlurEffect>

#include <QTranslator>
#include <QLibraryInfo>

#include <QProcess>
#include <QThreadPool> 

#include <QDesktopServices>
#include <QFontDatabase>

#include <QMessageBox>

#include <QWebEngineSettings>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <AdBlock/Manager.hpp>

#include "BrowserWindow.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Password/AutoFill/AutoFill.hpp"

#include "Cookies/CookieJar.hpp"

#include "History/History.hpp"
#include "Bookmarks/Bookmarks.hpp"
#include "MaquetteGrid/MaquetteGrid.hpp"
#include "MaquetteGrid/MaquetteGridManager.hpp"
#include "Download/DownloadManager.hpp"

#include "Database/SqlDatabase.hpp"
#include "Database/ProfileManager.hpp"

#include "Utils/RegExp.hpp"
#include "Utils/CommandLineOption.hpp"
#include "Utils/DataPaths.hpp"
#include "Utils/Updater.hpp"
#include "Utils/RestoreManager.hpp"
#include "Utils/Settings.hpp"
#include "Utils/SideBarManager.hpp"

#include "Web/WebPage.hpp"
#include "Web/Scripts.hpp"
#include "Web/HTML5Permissions/HTML5PermissionsManager.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Network/NetworkManager.hpp"

#include "Widgets/TitleBar.hpp"
#include "Widgets/NavigationControlDialog.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"
#include "Widgets/Tab/TabBar.hpp"
#include "Widgets/AddressBar/AddressBar.hpp"
#include "Widgets/Preferences/Appearance.hpp"
#include "Widgets/SideBar/SideBarInterface.hpp"

namespace Sn
{
QString Application::currentVersion = QString("1.17.14");

// Static member
Application *Application::instance()
{
	return dynamic_cast<Application*>(QCoreApplication::instance());
}

QIcon Application::getAppIcon(const QString& name, const QString& directory, const QString& format)
{
	// Return icon from active theme folder (in %data%/themes/%ativetheme%/%logo-path%
	// Else, it return the default icon from icon.qrc file
	return QIcon::fromTheme(name, QIcon(":icons/" + directory + '/' + name + format));
}

QByteArray Application::readAllFileByteContents(const QString& filename)
{
	QFile file{filename};

	if (!filename.isEmpty() && file.open(QFile::ReadOnly)) {
		const QByteArray byteArray{file.readAll()};

		file.close();

		return byteArray;
	}

	return QByteArray();
}

QString Application::getFileNameFromUrl(const QUrl &url)
{
	QString fileName{url.toString(QUrl::RemoveFragment | QUrl::RemoveQuery | QUrl::RemoveScheme | QUrl::RemovePort)};

	if (fileName.endsWith(QLatin1Char('/'))) 
		fileName = fileName.mid(0, fileName.length() - 1);

	if (fileName.indexOf(QLatin1Char('/')) != -1) {
		int pos{fileName.lastIndexOf(QLatin1Char('/'))};

		fileName = fileName.mid(pos);
		fileName.remove(QLatin1Char('/'));
	}

	fileName.replace(QLatin1Char('/'), QLatin1Char('-'));
	fileName.remove(QLatin1Char('\\'));
	fileName.remove(QLatin1Char(':'));
	fileName.remove(QLatin1Char('*'));
	fileName.remove(QLatin1Char('?'));
	fileName.remove(QLatin1Char('"'));
	fileName.remove(QLatin1Char('<'));
	fileName.remove(QLatin1Char('>'));
	fileName.remove(QLatin1Char('|'));

	if (fileName.isEmpty()) {
		fileName = url.host();

		fileName.replace(QLatin1Char('/'), QLatin1Char('-'));
		fileName.remove(QLatin1Char('\\'));
		fileName.remove(QLatin1Char(':'));
		fileName.remove(QLatin1Char('*'));
		fileName.remove(QLatin1Char('?'));
		fileName.remove(QLatin1Char('"'));
		fileName.remove(QLatin1Char('<'));
		fileName.remove(QLatin1Char('>'));
		fileName.remove(QLatin1Char('|'));
	}

	return fileName;
}

QString Application::ensureUniqueFilename(const QString& name, const QString& appendFormat)
{
	Q_ASSERT(appendFormat.contains(QLatin1String("%1")));

	QFileInfo info{name};

	if (!info.exists())
		return name;

	const QDir dir{info.absoluteDir()};
	const QString fileName{info.fileName()};

	int i{1};

	while (info.exists()) {
		QString file{fileName};
		int index{file.lastIndexOf(QLatin1Char('.'))};
		const QString appendString{appendFormat.arg(i)};

		if (index == -1)
			file.append(appendString);
		else
			file = file.left(index) + appendString + file.mid(index);

		info.setFile(dir, file);
		++i;
	}

	return info.absoluteFilePath();
}

void Application::removeDirectory(const QString& directory)
{
	QDir dir{directory};

	if (dir.exists()) {
		const QFileInfoList list{dir.entryInfoList()};
		QFileInfo info{};

		for (int i{0}; i < list.size(); ++i) {
			info = list[i];

			if (info.isDir() && info.fileName() != QLatin1String(".") && info.fileName() != QLatin1String(".."))
				Application::removeDirectory(info.absoluteFilePath());
			else if (info.isFile())
				QFile(info.absoluteFilePath()).remove();
		}

		dir.rmdir(directory);
	}
}

// Constructor&  destructor
Application::Application(int& argc, char** argv) :
	SingleApplication(argc, argv, true),
	m_plugins(nullptr),
	m_autoFill(nullptr),
	m_cookieJar(nullptr),
	m_history(nullptr),
	m_networkManager(nullptr),
	m_webProfile(nullptr)
{
	// Setting up settings environment
	QCoreApplication::setApplicationName(QLatin1String("Sielo"));
	QCoreApplication::setApplicationVersion(QLatin1String("1.17.14"));

	setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
	setAttribute(Qt::AA_EnableHighDpiScaling);
	/*
		// QSQLITE database plugin is required
		if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE"))) {
			QMessageBox::critical(0,
								  QStringLiteral("Error"),
								  QStringLiteral(
										  "Qt SQLite database plugin is not available. Please install it and restart the application."));
			return;
		}
	*/
	// Loading fonts information
	int id = QFontDatabase::addApplicationFont(":data/fonts/morpheus.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	m_morpheusFont = QFont(family);
	m_normalFont = font();

	// Check command line options with given arguments
	QUrl startUrl{};
	QString startProfile{ };
	QStringList messages;

	bool newInstance{false};

	// Check startup arguments
	if (argc > 1) {
		CommandLineOption command{};

		foreach(const CommandLineOption::ActionPair& pair, command.getActions())
		{
			switch (pair.action) {
			case Application::CL_StartWithProfile:
				startProfile = pair.text;
				break;
			case Application::CL_NewTab:
				messages.append(QLatin1String("ACTION:NewTab"));
				m_postLaunchActions.append(OpenNewTab);
				break;
			case Application::CL_NewWindow:
				messages.append(QLatin1String("ACTION:NewWindow"));
				break;
			case Application::CL_StartPrivateBrowsing:
				m_privateBrowsing = true;
				break;
			case Application::CL_StartNewInstance:
				newInstance = true;
				break;
			case Application::CL_OpenUrlInCurrentTab:
				startUrl = QUrl::fromUserInput(pair.text);
				messages.append("ACTION:OpenUrlInCurrentTab" + pair.text);
				break;
			case Application::CL_OpenUrlInNewWindow:
				startUrl = QUrl::fromUserInput(pair.text);
				messages.append("ACTION:OpenUrlInNewWindow" + pair.text);
				break;
			case Application::CL_OpenUrl:
				startUrl = QUrl::fromUserInput(pair.text);
				messages.append("URL:" + pair.text);
				break;
			case Application::CL_ExitAction:
				m_isClosing = true;
			default:
				break;

			}
		}
	}

	if (messages.isEmpty()) {
		messages.append(QLatin1String(" "));
	}

	// Check is there is already an instance of Sielo running
	if (isSecondary() && !newInstance && !privateBrowsing()) {
		m_isClosing = true;
		foreach(const QString& message, messages)
		{
			sendMessage(message.toUtf8());
		}

		return;
	}

	// Open urls from Sielo dialogs in Sielo itself
	QDesktopServices::setUrlHandler("http", this, "addNewTab");
	QDesktopServices::setUrlHandler("https", this, "addNewTab");
	QDesktopServices::setUrlHandler("ftp", this, "addNewTab");

	ProfileManager::initConfigDir();
	ProfileManager profileManager{};
	profileManager.initCurrentProfile(startProfile);

	Settings::createSettings(DataPaths::currentProfilePath() + "/settings.ini");

#ifndef QT_DEBUG
	// the 3rd parameter is the site id
	m_piwikTracker = new PiwikTracker(this, QUrl("https://sielo.app/analytics"), 1);
	m_piwikTracker->sendVisit("launch");
#endif 

	// Setting up web and network objects
	m_webProfile = privateBrowsing() ? new QWebEngineProfile(this) : QWebEngineProfile::defaultProfile();
	connect(m_webProfile, &QWebEngineProfile::downloadRequested, this, &Application::downloadRequested);

	m_networkManager = new NetworkManager(this);
	m_autoFill = new AutoFill;

	loadSettings();
	translateApplication();

	// Setup web channel with custom script (mainly for autofill)
	QString webChannelScriptSrc = Scripts::webChannelDefautlScript();

	QWebEngineScript script{};

	script.setName(QStringLiteral("_sielo_webchannel"));
	script.setInjectionPoint(QWebEngineScript::DocumentCreation);
	script.setWorldId(QWebEngineScript::MainWorld);
	script.setRunsOnSubFrames(true);
	script.setSourceCode(webChannelScriptSrc.arg(readFile(QStringLiteral(":/qtwebchannel/qwebchannel.js"))));

	m_webProfile->scripts()->insert(script);

	m_plugins = new PluginProxy;
	m_plugins->loadPlugins();

	// Check if we start after a crash
	if (!privateBrowsing()) {
		Settings settings{};

		m_startingAfterCrash = settings.value(QLatin1String("isRunning"), false).toBool();
		settings.setValue(QLatin1String("isRunning"), true);

		if (m_startingAfterCrash)
			startAfterCrash();
	}

	// Create or restore window
	BrowserWindow* window{createWindow(Application::WT_FirstAppWindow, startUrl)};

	connect(this, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged()));

	if (!privateBrowsing()) {
		if (isStartingAfterCrash()) {
			if (afterCrashLaunch() == RestoreSession)
				m_restoreManager = new RestoreManager();
		}
		else if (afterLaunch() == RestoreSession || afterLaunch() == OpenSavedSession) {
			m_restoreManager = new RestoreManager(afterLaunch() == OpenSavedSession);
		}

		if (m_restoreManager && !m_restoreManager->isValid()) 
			destroyRestoreManager();
	}

	if (m_restoreManager)
		restoreSession(window, m_restoreManager->restoreData());

	// Check for update
	Updater* updater{new Updater(window)};
	Q_UNUSED(updater);

	// Wait a little for post launch actions
	QTimer::singleShot(0, this, &Application::postLaunch);
}

Application::~Application()
{
	if (m_windows.count() > 0)
		saveSession();

	QThreadPool::globalInstance()->waitForDone();

	delete m_plugins;
	delete m_cookieJar;
	delete m_downloadManager;
}

void Application::loadSettings()
{
	Settings settings;

	// General Sielo settings
	m_fullyLoadThemes = settings.value("Settings/fullyLoadThemes", true).toBool();
	m_useTopToolBar = settings.value("Settings/useTopToolBar", false).toBool();
	m_hideBookmarksHistoryActions = settings.value("Settings/hideBookmarksHistoryByDefault", false).toBool();
	m_floatingButtonFoloweMouse = settings.value("Settings/floatingButtonFoloweMouse", true).toBool();

	m_networkManager->loadSettings();

	// Check if the user have enable the witcher font
	if (settings.value("Settings/useMorpheusFont", false).toBool()) {
		QWebEngineSettings* webSettings = QWebEngineSettings::defaultSettings();

		setFont(m_morpheusFont);
		webSettings->setFontFamily(QWebEngineSettings::StandardFont, "Z003");
		webSettings->setFontFamily(QWebEngineSettings::CursiveFont, "Z003");
		webSettings->setFontFamily(QWebEngineSettings::FantasyFont, "Z003");
		webSettings->setFontFamily(QWebEngineSettings::FixedFont, "Z003");
		webSettings->setFontFamily(QWebEngineSettings::SansSerifFont, "Z003");
		webSettings->setFontFamily(QWebEngineSettings::SerifFont, "Z003");
	}

	// Load specific settings for all windows
	foreach(BrowserWindow* window, m_windows) window->loadSettings();

	// Load settings for password
	if (m_autoFill)
		m_autoFill->loadSettings();

	loadWebSettings();
	loadApplicationSettings();
	loadThemesSettings();
	loadPluginsSettings();
	loadTranslationSettings();
}

void Application::loadWebSettings()
{
	Settings settings{};

	// load web settings
	QWebEngineSettings* webSettings = m_webProfile->settings();

	settings.beginGroup("Web-Settings");

	webSettings->setAttribute(QWebEngineSettings::PluginsEnabled, settings.value("allowPlugins", true).toBool());
	webSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, settings.value("allowJavaScript", true).toBool());
	webSettings->setAttribute(QWebEngineSettings::LinksIncludedInFocusChain,
							  settings.value("includeLinkInFocusChain", false).toBool());
	webSettings->setAttribute(QWebEngineSettings::XSSAuditingEnabled, settings.value("XSSAuditing", false).toBool());
	webSettings
		->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled,
					   settings.value("animateScrolling", true).toBool());
	webSettings->setAttribute(QWebEngineSettings::SpatialNavigationEnabled,
							  settings.value("spatialNavigation", false).toBool());
	webSettings->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, false);
	webSettings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
	webSettings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

	setWheelScrollLines(settings.value("wheelScrollLines", wheelScrollLines()).toInt());

	QWebEngineProfile* webProfile = QWebEngineProfile::defaultProfile();

	QString defaultUserAgent = webProfile->httpUserAgent();
	defaultUserAgent.replace(QRegularExpression(QStringLiteral("QtWebEngine/[^\\s]+")),
							 QStringLiteral("Sielo/%1").arg(QCoreApplication::applicationVersion()));
	webProfile->setHttpUserAgent(defaultUserAgent);


	const bool allowCache{settings.value("allowLocalCache", true).toBool()};
	webProfile->setHttpCacheType(allowCache ? QWebEngineProfile::DiskHttpCache : QWebEngineProfile::MemoryHttpCache);
	webProfile->setCachePath(settings.value("cachePath", webProfile->cachePath()).toString());

	webProfile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
	webProfile->setPersistentStoragePath(DataPaths::currentProfilePath());

	settings.endGroup();

	// Force local storage to be disabled if it's a provate session
	if (privateBrowsing()) {
		webSettings->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
		history()->setSaving(false);
	}
}

void Application::loadApplicationSettings()
{
	Settings settings{};
	QSettings oldSettings{"Feldrise", "Sielo"};

	// Check the current version number of Sielo, and make setting update if needed
	//TODO: improve this with a switch
	int oldSettingsVersionNumber = oldSettings.value("versionNumber", 14).toInt();
	if (oldSettingsVersionNumber < 15) {
		if (oldSettingsVersionNumber < 14) {
			oldSettings.setValue("installed", false);
			if (oldSettingsVersionNumber < 12) {
				if (oldSettingsVersionNumber < 11) {
					// Update with new bookmarks
					QString directory{DataPaths::currentProfilePath()};
					QFile::remove(directory
								  + QLatin1String("/bookmarks.xbel"));
					QFile::copy(QLatin1String(":data/bookmarks.xbel"), directory
								+ QLatin1String("/bookmarks.xbel"));
					QFile::setPermissions(directory
										  + QLatin1String("/bookmarks.xbel"),
										  QFileDevice::ReadUser | QFileDevice::WriteUser);
				}

				// Update home page to use last version of doosearch
				oldSettings.setValue("Web-Settings/homePage", "https://doosearch.sielo.app/");
				oldSettings.setValue("Web-Settings/urlOnNewTab", "https://doosearch.sielo.app/");

				foreach(BrowserWindow* window, m_windows)
				{
					window->loadSettings();
					for (int i{0}; i < window->tabsSpaceSplitter()->count(); ++i) {
						window->tabWidget(i)->setHomeUrl("https://doosearch.sielo.app");
					}
				}
			}
		}

		oldSettings.setValue("versionNumber", 15);
		updateToProfiles();
	}

	if (settings.value("versionNumber", 0).toInt() < 16) {
		QString directory{DataPaths::currentProfilePath()};

		if (!QDir(directory + "/maquette-grid").exists())
			QDir(directory).mkdir("maquette-grid");

		QFile::copy(QLatin1String(":data/default-maquettegrid.dat"), directory + QLatin1String("/maquette-grid/default.dat"));
		QFile::setPermissions(directory + QLatin1String("/bookmarks.xbel"), QFileDevice::ReadUser | QFileDevice::WriteUser);

		settings.setValue("versionNumber", 16);
	}
}

void Application::loadThemesSettings()
{
	Settings settings{};

	// Load theme info
	QFileInfo themeInfo{
		DataPaths::currentProfilePath() + "/themes" + QLatin1Char('/')
		+ settings.value("Themes/currentTheme", "sielo-default").toString()
		+ QLatin1String("/main.sss")
	};

	// Check if the theme existe
	if (themeInfo.exists()) {
		// Check default theme version and update it if needed
		if (settings.value("Themes/defaultThemeVersion", 1).toInt() < 53) {
			if (settings.value("Themes/defaultThemeVersion", 1).toInt() < 11) {
				QString defaultThemePath{DataPaths::currentProfilePath() + "/themes"};

				QDir(defaultThemePath + "/bluegrey-flat").removeRecursively();
				QDir(defaultThemePath + "/cyan-flat").removeRecursively();
				QDir(defaultThemePath + "/green-flat").removeRecursively();
				QDir(defaultThemePath + "/indigo-flat").removeRecursively();
				QDir(defaultThemePath + "/orange-flat").removeRecursively();
				QDir(defaultThemePath + "/purple-flat").removeRecursively();
				QDir(defaultThemePath + "/red-flat").removeRecursively();
				QDir(defaultThemePath + "/sielo-default").removeRecursively();
				QDir(defaultThemePath + "/teal-flat").removeRecursively();
				QDir(defaultThemePath + "/white-flat").removeRecursively();
				QDir(defaultThemePath + "/yellow-flat").removeRecursively();

				loadThemeFromResources("sielo-default", false);
				settings.setValue("Themes/currentTheme", QLatin1String("sielo-default"));
			}

			loadThemeFromResources("firefox-like-light", false);
			loadThemeFromResources("firefox-like-dark", false);
			loadThemeFromResources("sielo-flat", false);
			loadThemeFromResources("round-theme", false);
			loadThemeFromResources("ColorZilla", false);
			loadThemeFromResources("sielo-default", false);
			settings.setValue("Themes/defaultThemeVersion", 52);
		}

		loadTheme(settings.value("Themes/currentTheme", QLatin1String("sielo-default")).toString(),
				  settings.value("Themes/lightness", QLatin1String("dark")).toString());

	}
	else {
		loadThemeFromResources("firefox-like-light", false);
		loadThemeFromResources("firefox-like-dark", false);
		loadThemeFromResources("sielo-flat", false);
		loadThemeFromResources("round-theme", false);
		loadThemeFromResources("ColorZilla", false);
		loadThemeFromResources();

		settings.setValue("Themes/defaultThemeVersion", 53);
	}
}

void Application::loadPluginsSettings()
{
	Settings settings{};

	int pluginsVersion{settings.value("Plugin-Settings/pluginsVersion", 0).toInt()};
	QString pluginsPath{DataPaths::currentProfilePath() + "/plugins"};
	QString dataPluginsPath{":/plugins/data/plugins/"};

	if (pluginsVersion < 6) {
#if defined(Q_OS_WIN)
		copyPath(QDir(dataPluginsPath + "/windows").absolutePath(), pluginsPath);
#elif defined(Q_OS_MACOS) 
		copyPath(QDir(dataPluginsPath + "/macos").absolutePath(), pluginsPath);
#else 
		copyPath(QDir(dataPluginsPath + "/linux").absolutePath(), pluginsPath);
#endif

		settings.setValue("Plugin-Settings/pluginsVersion", 6);
	}
}

void Application::loadTranslationSettings()
{
	Settings settings{};
	settings.beginGroup("Language");

	if (settings.value("version", 0).toInt() < 18) {
		QDir(DataPaths::currentProfilePath() + "/locale").removeRecursively();
		copyPath(QDir(":data/locale").absolutePath(), DataPaths::currentProfilePath() + "/locale");
		settings.setValue("version", 18);
	}
}

void Application::updateToProfiles()
{
	Settings settings{};
	QSettings oldSettings{"Feldrise", "Sielo"};
	QStringList keys = oldSettings.allKeys();

	for (const QString& key : keys)
		settings.setValue(key, oldSettings.value(key));

	QString appData{QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)};
	QString oldDataPath{appData.insert(appData.count() - 6, "/Feldrise")};

	std::string stdOld = oldDataPath.toStdString();
	
	copyPath(oldDataPath +"/themes", DataPaths::currentProfilePath() + "/themes", true);
	copyPath(oldDataPath + "/maquette-grid", DataPaths::currentProfilePath() + "/maquette-grid", true);
	
	QFile(DataPaths::currentProfilePath() + "/bookmarks.json").remove();
	QFile(DataPaths::currentProfilePath() + "/fbutton.dat").remove();
	QFile(oldDataPath + "/bookmarks.json").copy(DataPaths::currentProfilePath() + "/bookmarks.json");
	QFile(oldDataPath + "/fbutton.dat").copy(DataPaths::currentProfilePath() + "/fbutton.dat");

	loadThemeFromResources("firefox-like-light", false);
	loadThemeFromResources("firefox-like-dark", false);
	loadThemeFromResources("sielo-flat", false);
	loadThemeFromResources("round-theme", false);
	loadThemeFromResources("ColorZilla", false);
	loadThemeFromResources("sielo-default", false);
	settings.setValue("Themes/defaultThemeVersion", 44);
}

void Application::translateApplication()
{
	Settings settings{};
	QString file{settings.value("Language/language", QLocale::system().name()).toString()};

	// It can only be "C" locale, for which we will use default English language
	if (file.size() < 2)
		file.clear();

	if (!file.isEmpty() && !file.endsWith(QLatin1String(".qm")))
		file.append(QLatin1String(".qm"));

	// Either we load default language (with empty file), or we attempt to load xx.qm (xx_yy.qm)
	Q_ASSERT(file.isEmpty() || file.size() >= 5);

	QString translationPath{DataPaths::currentProfilePath() + "/locale"};

	if (!file.isEmpty()) {
		if (!QFile(QString("%1/%2").arg(translationPath, file)).exists()) {
			QDir dir{translationPath};
			QString lang{file.left(2) + QLatin1String("*.qm")};

			const QStringList translations = dir.entryList(QStringList(lang));

			// If no translation can be found, we will use the default English
			file = translations.isEmpty() ? QString() : translations[0];
		}
	}

	// Application translations
	QTranslator* app{new QTranslator(this)};
	app->load(file, translationPath);

	// Qt translations
	QTranslator* sys{new QTranslator(this)};
	sys->load("qt_" + file, QLibraryInfo::location(QLibraryInfo::TranslationsPath));

	if (sys->isEmpty())
		sys->load("qt_" + file, translationPath);

	m_languageFile = file;

	installTranslator(app);
	installTranslator(sys);
}

QString Application::currentLanguage() const
{
	QString lang = m_languageFile;

	if (lang.isEmpty()) {
		return "en_US";
	}

	return lang.left(lang.length() - 3);
}

QWebEngineProfile *Application::webProfile()
{
	return m_webProfile;
}

int Application::windowCount() const
{
	return m_windows.count();
}

BrowserWindow *Application::getWindow() const
{
	if (m_lastActiveWindow)
		return m_lastActiveWindow.data();

	return m_windows.isEmpty() ? nullptr : m_windows[0];
}

BrowserWindow *Application::createWindow(Application::WindowType type, const QUrl& startUrl)
{
	if (windowCount() == 0)
		type = Application::WT_FirstAppWindow;

	BrowserWindow* window{new BrowserWindow(type, startUrl)};
	QObject::connect(window, &BrowserWindow::destroyed, this, &Application::windowDestroyed);

	m_windows.prepend(window);
	return window;
}

BrowserWindow *Application::createWindow(MaquetteGridItem* item)
{
	Q_ASSERT(windowCount() != 0);

	BrowserWindow* maquetteGridWindow = new BrowserWindow(Application::WT_OtherRestoredWindow);
	RestoreData data = item->data();
	openSession(maquetteGridWindow, data);

	QObject::connect(maquetteGridWindow, &BrowserWindow::destroyed, this, &Application::windowDestroyed);

	m_windows.prepend(maquetteGridWindow);
	return maquetteGridWindow;
}

Application::AfterLaunch Application::afterLaunch() const
{
	Settings settings{};

	return static_cast<AfterLaunch>(settings.value(QStringLiteral("Settings/afterLaunch"), OpenHomePage).toInt());
}

void Application::openSession(BrowserWindow* window, RestoreData& restoreData)
{
	setOverrideCursor(Qt::BusyCursor);

	if (!window)
		window = createWindow(WT_OtherRestoredWindow);

	window->restoreWindowState(restoreData.windows.takeAt(0));

	foreach(const BrowserWindow::SavedWindow &data, restoreData.windows)
	{
		BrowserWindow* window = createWindow(WT_OtherRestoredWindow);
		window->restoreWindowState(data);
	}

	restoreOverrideCursor();
}

bool Application::restoreSession(BrowserWindow* window, RestoreData restoreData)
{
	if (m_privateBrowsing || !restoreData.isValid())
		return false;

	m_isRestoring = true;

	openSession(window, restoreData);

	m_restoreManager->clearRestoreData();
	destroyRestoreManager();

	m_isRestoring = false;

	return true;
}

void Application::destroyRestoreManager()
{
	delete m_restoreManager;
	m_restoreManager = nullptr;
}

void Application::addSidebar(const QString& id, SideBarInterface* sideBarInterface)
{
	m_sidebars[id] = sideBarInterface;
}

void Application::removeSidebar(SideBarInterface* sideBarInterface) {
	const QString id{m_sidebars.key(sideBarInterface)};

	if (id.isEmpty())
		return;

	m_sidebars.remove(id);

	foreach(BrowserWindow* window, Application::instance()->windows())
	{
		foreach(TabWidget* tabWidget, window->tabsSpaceSplitter()->tabWidgets())
			tabWidget->sideBarManager()->sideBarRemoved(id);
	}
}


void Application::saveSettings()
{
	// Save settings of AdBlock
	ADB::Manager::instance()->save();

	// If we are in private browsing, we don't want to save settings obviously
	if (privateBrowsing())
		return;

	QSettings settings{DataPaths::currentProfilePath() + "/settings.ini", QSettings::IniFormat};

	settings.setValue("isRunning", false);

	settings.beginGroup("Web-Settings");

	// Check if we want to remove history and|or cookies. Remove them if needed
	bool deleteHistory
	{
		settings.value("deleteHistoryOnClose", false).toBool() ||
		!settings.value("allowHistory", true).toBool()
	};

	settings.endGroup();

	bool deleteCookies{settings.value("Cookie-Settings/deleteCookiesOnClose", false).toBool()};

	if (deleteHistory)
		m_history->clearHistory();
	if (deleteCookies)
		m_cookieJar->deleteAllCookies();

#ifndef QT_DEBUG
	m_piwikTracker->sendEvent("exit", "exit", "exit", "exit application");
#endif
}

void Application::saveSession(bool saveForHome)
{
	if (m_privateBrowsing || m_isRestoring || m_windows.count() == 0 || m_restoreManager)
		return;

	QByteArray data{};
	QDataStream stream{&data, QIODevice::WriteOnly};

	RestoreData restoreData{};
	restoreData.windows.reserve(m_windows.count());

	for (BrowserWindow* window : m_windows)
		restoreData.windows.append(BrowserWindow::SavedWindow(window));

	if (m_restoreManager && m_restoreManager->isValid()) {
		QDataStream creashStream(&restoreData.crashedSession, QIODevice::WriteOnly);
		creashStream << m_restoreManager->restoreData();
	}

	stream << 1;
	stream << restoreData;

	// Save data to a file
	QFile file{};
	if (saveForHome)
		file.setFileName(DataPaths::currentProfilePath() + QLatin1String("/home-session.dat"));
	else
		file.setFileName(DataPaths::currentProfilePath() + QLatin1String("/session.dat"));

	file.open(QIODevice::WriteOnly);
	file.write(data);
	file.close();
}

void Application::reloadUserStyleSheet()
{
	Settings setting{};
	QString userCSSFile{setting.value("Settings/userStyleSheet", QString()).toString()};

	setUserStyleSheet(userCSSFile);
}

void Application::quitApplication()
{
	m_isClosing = true;

	m_plugins->shutdown();

	quit();
}

void Application::postLaunch()
{
	// Check if we want to open a new tab
	if (m_postLaunchActions.contains(OpenNewTab))
		getWindow()->tabWidget()->addView(QUrl(), Application::NTT_SelectedNewEmptyTab);

	connect(this, &Application::receivedMessage, this, &Application::messageReceived);
	connect(this, &Application::aboutToQuit, this, &Application::saveSettings);
}

void Application::windowDestroyed(QObject* window)
{
	Q_ASSERT(static_cast<BrowserWindow*>(window));
	Q_ASSERT(m_windows.contains(static_cast<BrowserWindow*>(window)));

	m_windows.removeOne(static_cast<BrowserWindow*>(window));
}

void Application::onFocusChanged()
{
	BrowserWindow* activeBrowserWindow = qobject_cast<BrowserWindow*>(activeWindow());

	if (activeBrowserWindow) {
		m_lastActiveWindow = activeBrowserWindow;

		emit activeWindowChanged(m_lastActiveWindow);
	}
}

void Application::downloadRequested(QWebEngineDownloadItem* download)
{
	downloadManager()->downlaod(download);
	downloadManager()->show();
	downloadManager()->raise();
	download->accept();
}

void Application::messageReceived(quint32, QByteArray messageBytes)
{
	QWidget* actualWindow = getWindow();
	QUrl actualUrl{};
	QString message{QString::fromUtf8(messageBytes)};

	// Check if the message start by a url. If so, open it in new tab
	if (message.startsWith(QLatin1String("URL:"))) {
		const QUrl url{QUrl::fromUserInput(message.mid(4))};
		addNewTab(url);
		actualWindow = getWindow();
	}
	else if (message.startsWith(QLatin1String("ACTION:"))) {
		const QString text{message.mid(7)};

		if (text == QLatin1String("NewTab"))
			addNewTab();
		else if (text == QLatin1String("NewWindow"))
			actualWindow = createWindow(Application::WT_NewWindow);
		else if (text.startsWith(QLatin1String("OpenUrlInCurrentTab")))
			actualUrl = QUrl::fromUserInput(text.mid(19));
		else if (text.startsWith(QLatin1String("OpenUrlInNewWindow"))) {
			createWindow(Application::WT_NewWindow, QUrl::fromUserInput(text.mid(18)));
			return;
		}
	}
	else
		actualWindow = createWindow(Application::WT_NewWindow);

	if (!actualWindow) {
		if (!isClosing()) {
			createWindow(Application::WT_NewWindow, actualUrl);
		}

		return;
	}

	actualWindow->setWindowState(actualWindow->windowState() & ~Qt::WindowMinimized);
	actualWindow->raise();
	actualWindow->activateWindow();
	actualWindow->setFocus();

	BrowserWindow* window = qobject_cast<BrowserWindow*>(actualWindow);

	if (window && !actualUrl.isEmpty())
		window->loadUrl(actualUrl);
}

void Application::setUserStyleSheet(const QString& filePath)
{
	QString userCSS{};
	QFile file{filePath};
	QByteArray array{};

	// Check if we can open the file
	if (!filePath.isEmpty() && file.open(QFile::ReadOnly)) {
		array = file.readAll();
		file.close();
	}

	userCSS += QString::fromUtf8(array).remove(QLatin1Char('\n'));

	// Check if we have an old script
	const QString name{QStringLiteral("_sielo_userstylesheet")};
	QWebEngineScript oldScript = m_webProfile->scripts()->findScript(name);

	if (!oldScript.isNull())
		m_webProfile->scripts()->remove(oldScript);

	if (userCSS.isEmpty())
		return;

	// Apply custom css
	QWebEngineScript script{};
	script.setName(name);
	script.setInjectionPoint(QWebEngineScript::DocumentReady);
	script.setWorldId(QWebEngineScript::ApplicationWorld);
	script.setRunsOnSubFrames(true);
	script.setSourceCode(WebPage::setCSS(userCSS));

	m_webProfile->scripts()->insert(script);
}

CookieJar *Application::cookieJar()
{
	if (!m_cookieJar)
		m_cookieJar = new CookieJar();

	return m_cookieJar;
}

History *Application::history()
{
	if (!m_history)
		m_history = new History(this);

	return m_history;
}

Bookmarks *Application::bookmarks()
{
	if (!m_bookmarks)
		m_bookmarks = new Bookmarks(this);

	return m_bookmarks;
}

MaquetteGrid *Application::maquetteGrid()
{
	if (!m_maquetteGrid)
		m_maquetteGrid = new MaquetteGrid(this);

	return m_maquetteGrid;
}

DownloadManager *Application::downloadManager()
{
	if (!m_downloadManager)
		m_downloadManager = new DownloadManager();

	return m_downloadManager;
}

HTML5PermissionsManager *Application::permissionsManager()
{
	if (!m_permissionsManager)
		m_permissionsManager = new HTML5PermissionsManager(this);

	return m_permissionsManager;
}

void Application::startAfterCrash()
{
	QMessageBox requestAction{};
	requestAction.setWindowTitle(QApplication::tr("Start after crash"));
	requestAction.setText(QApplication::tr("You are starting Sielo after a crash. What would you like to do?"));

	QAbstractButton* startBlankSession = requestAction.addButton(QApplication::tr("Start New Session"),
																 QMessageBox::NoRole);
	QAbstractButton* restoreSession = requestAction.addButton(QApplication::tr("Restore Session"),
															  QMessageBox::YesRole);

	requestAction.exec();

	// 32bit version is not longer supported since Sielo Maynapeh
	// Since some people seem to be able to run only the 32 bit version...
	/*if (!is32bit()) {
		QMessageBox::information(nullptr, QApplication::tr("Info"), QApplication::tr(
			"Please, if Sielo continues crashing, consider trying this 32bit version."));
	}*/

	if (requestAction.clickedButton() == restoreSession) {
		m_afterCrashLaunch = AfterLaunch::RestoreSession;
	}
	else {
		m_afterCrashLaunch = AfterLaunch::OpenHomePage;
	}
}

void Application::processCommand(const QString& command, const QStringList args)
{
	if (m_plugins->processCommand(command, args))
		return;

	if (command == "site") {
		LoadRequest siteRequest{};
		siteRequest.setUrl(QUrl("http://www.feldrise.com/Sielo/"));

		getWindow()->tabWidget()->webTab()->webView()
			->loadInNewTab(siteRequest, Application::NTT_CleanSelectedTabAtEnd);
	}

	if (command == "github") {
		LoadRequest githubRequest{};
		githubRequest.setUrl(QUrl("https://github.com/Feldrise/SieloNavigateur"));

		getWindow()->tabWidget()->webTab()->webView()
			->loadInNewTab(githubRequest, Application::NTT_CleanSelectedTabAtEnd);
	}

	if (command == "witcher") {
		if (args.count() == 1) {
			Settings settings{};
			QWebEngineSettings* webSettings = QWebEngineSettings::defaultSettings();

			if (args[0] == "enable") {
				setFont(m_morpheusFont);
				webSettings->setFontFamily(QWebEngineSettings::StandardFont, "Z003");
				webSettings->setFontFamily(QWebEngineSettings::CursiveFont, "Z003");
				webSettings->setFontFamily(QWebEngineSettings::FantasyFont, "Z003");
				webSettings->setFontFamily(QWebEngineSettings::FixedFont, "Z003");
				webSettings->setFontFamily(QWebEngineSettings::SansSerifFont, "Z003");
				webSettings->setFontFamily(QWebEngineSettings::SerifFont, "Z003");

				// Reload font
						foreach (BrowserWindow* window, m_windows) {
						for (int i{0}; i < window->tabsSpaceSplitter()->count(); ++i) {
							for (int j{0}; j < window->tabWidget(i)->count(); ++j) {
								window->tabWidget(i)->webTab(j)->addressBar()
										->setFont(m_morpheusFont);
							}
							window->tabWidget(i)->tabBar()->qtabBar()->setFont(m_morpheusFont);
						}
					}
				settings.setValue("Settings/useMorpheusFont", true);
			}
			else if (args[0] == "disable") {
				setFont(m_normalFont);
				webSettings->setFontFamily(QWebEngineSettings::StandardFont, "DejaVu Serif");
				webSettings->setFontFamily(QWebEngineSettings::CursiveFont, "DejaVu Sans");
				webSettings->setFontFamily(QWebEngineSettings::FantasyFont, "DejaVu Sans");
				webSettings->setFontFamily(QWebEngineSettings::FixedFont, "DejaVu Sans Mono");
				webSettings->setFontFamily(QWebEngineSettings::SansSerifFont, "DejaVu Sans");
				webSettings->setFontFamily(QWebEngineSettings::SerifFont, "DejaVu Serif");

				// Reload font
						foreach (BrowserWindow* window, m_windows) {
						for (int i{0}; i < window->tabsSpaceSplitter()->count(); ++i) {
							for (int j{0}; j < window->tabWidget(i)->count(); ++j) {
								window->tabWidget(i)->webTab(j)->addressBar()
										->setFont(m_normalFont);
							}
							window->tabWidget(i)->tabBar()->qtabBar()->setFont(m_normalFont);
						}
					}
				settings.setValue("Settings/useMorpheusFont", false);
			}
			else
				QMessageBox::critical(getWindow(),
									  QApplication::tr("Failed"),
									  QApplication::tr("The argument is unknow"));
		}
		else
			QMessageBox::critical(getWindow(),
								  QApplication::tr("Failed"),
								  QApplication::tr("This command need one argument!"));
	}
	if (command == "easteregg") {
		QStringList eastereggs{};
		eastereggs << "http://feldrise.com"
			<< "http://www.perdu.com/"
			<< "http://po.ta.to/"
			<< "https://procatinator.com/"
			<< "http://tholman.com/texter/"
			<< "http://hristu.net/"
			<< "https://omfgdogs.com/"
			<< "http://fallingfalling.com/"
			<< "http://zombo.com/"
			<< "http://thenicestplaceontheinter.net/"
			<< "http://pointerpointer.com/"
			<< "http://ducksarethebest.com/"
			<< "http://www.koalastothemax.com/"
			<< "http://ninjaflex.com/"
			<< "http://www.taghua.com/"
			<< "https://www.staggeringbeauty.com/";

		int sitesCount{eastereggs.count()};
		int easteregg{qrand() % ((sitesCount)-0) + 0};

		LoadRequest eastereggRequest{};
		eastereggRequest.setUrl(eastereggs[easteregg]);

		getWindow()->tabWidget()->webTab()->webView()
			->loadInNewTab(eastereggRequest, Application::NTT_CleanSelectedTabAtEnd);
	}
}

void Application::addNewTab(const QUrl& url)
{
	BrowserWindow* window = getWindow();

	if (window) {
		window->tabWidget()
			->addView(url,
					  url.isEmpty() ? Application::NTT_SelectedNewEmptyTab : Application::NTT_SelectedTabAtEnd);
	}
}

void Application::startPrivateBrowsing(const QUrl& startUrl)
{
	QUrl url{startUrl};

	if (QAction* action = qobject_cast<QAction*>(sender()))
		url = action->data().toUrl();

	QStringList args{};
	args.append(QStringLiteral("--private-browsing"));

	if (!url.isEmpty())
		args << url.toEncoded();

	if (!QProcess::startDetached(applicationFilePath(), args))
		QMessageBox::warning(nullptr,
							 "Failed",
							 "Cannot start new browser process for private browsing! " + applicationFilePath());
}

void Application::loadTheme(const QString& name, const QString& lightness)
{
	QString activeThemePath{DataPaths::currentProfilePath() + "/themes" + QLatin1Char('/') + name};
	QString sss{readFile(activeThemePath + QLatin1String("/main.sss"))};

	// If the theme use user color API
	if (QDir(activeThemePath + "/dark").exists() && QDir(activeThemePath + "/light").exists()) {
		QIcon::setThemeSearchPaths(QStringList() << activeThemePath);
		QIcon::setThemeName(lightness);
	}
	else {
		QIcon::setThemeSearchPaths(QStringList() << DataPaths::currentProfilePath() + "/themes");
		QIcon::setThemeName(name);
	}

	// Load specific theme file for the current OS
	if (m_fullyLoadThemes) {
#if defined(Q_OS_MAC)
		sss.append(readFile(activeThemePath + QLatin1String("/mac.sss")));
#elif defined(Q_OS_LINUX)
		sss.append(readFile(activeThemePath + QLatin1String("/linux.sss")));
#elif defined(Q_OS_WIN)
		sss.append(readFile(activeThemePath + QLatin1String("/windows.sss")));
#endif

		QString relativePath{QDir::current().relativeFilePath(activeThemePath)};

		sss = parseSSS(sss, relativePath, lightness);
		//		sss.replace(RegExp(QStringLiteral("scolor\\s*\\(\\s*main\\s*(\\s*,\\s*)\b([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\b\\s*(,\\s*normal))\\s*\\)")), "testeee");

		setStyleSheet(sss);
	}
	else {
		setStyleSheet("");
	}

	if (m_plugins)
		m_plugins->emitThemeChanged(lightness == "dark");
}

QString Application::parseSSS(QString& sss, const QString& relativePath, const QString& lightness)
{
	// Replace url with absolute path
	sss.replace(RegExp(QStringLiteral("url\\s*\\(\\s*([^\\*:\\);]+)\\s*\\)")),
				QString("url(%1/\\1)").arg(relativePath));

	// Replace some Sielo API properties to Qt properties
	sss.replace("sproperty", "qproperty");
	sss.replace("slineargradient", "qlineargradient");

	// Replace sbackground with a blured background 
	sss = parseSSSBackground(sss, relativePath);

	// Replace theme colors to user colors
	sss = parseSSSColor(sss, lightness);

	return sss;
}

QString Application::parseSSSBackground(QString& sss, const QString& relativePath)
{
	QString bluredBackgroundPath = getBlurredBackgroundPath("images/background.png", 15);

	if (bluredBackgroundPath.isEmpty())
		sss.replace(RegExp(";\\s*\\background\\s*\\:\\s*\\sbackground\\s*\\;"), "");
	else
		sss.replace("sbackground()", "url(" + bluredBackgroundPath + ")");

	std::string strSss = sss.toStdString();

	return sss;
}

QString Application::parseSSSColor(QString& sss, const QString& lightness)
{
	sss.replace(RegExp(QStringLiteral(
		"scolor\\s*\\(\\s*(main|second|accent|text)\\s*,\\s*(normal|light|dark)\\s*,\\s*([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\\s*\\)"
	)),
		"rgba($color\\1\\2, \\3\\4)");
	sss.replace(
		RegExp(QStringLiteral(
		"scolor\\s*\\(\\s*(main|second|accent|text)\\s*,\\s*(normal|light|dark)\\s*\\)")),
		"rgba($color\\1\\2, 255)");
	sss.replace(RegExp(QStringLiteral("scolor\\s*\\(\\s*(main|second|accent|text)\\s*\\)")),
				"rgba($color\\1normal, 255)");

	sss.replace(QLatin1String("\\4"), "");

	sss.replace("$colormainlight", AppearancePage::colorString("mainlight"));
	sss.replace("$colormaindark", AppearancePage::colorString("maindark"));
	sss.replace("$colormainnormal", AppearancePage::colorString("mainnormal"));
	sss.replace("$colorsecondlight", AppearancePage::colorString("secondlight"));
	sss.replace("$colorseconddark", AppearancePage::colorString("seconddark"));
	sss.replace("$colorsecondnormal", AppearancePage::colorString("secondnormal"));
	sss.replace("$coloraccentlight", AppearancePage::colorString("accentlight"));
	sss.replace("$coloraccentdark", AppearancePage::colorString("accentdark"));
	sss.replace("$coloraccentnormal", AppearancePage::colorString("accentnormal"));
	sss.replace("$colortextlight", AppearancePage::colorString("textlight"));
	sss.replace("$colortextdark", AppearancePage::colorString("textdark"));
	sss.replace("$colortextnormal", AppearancePage::colorString("textnormal"));

	sss.replace("$ulightness", lightness);

	return sss;
}

QString Application::getBlurredBackgroundPath(const QString& defaultBackground, int radius)
{
	Settings settings{};

	QString backgroundPath = settings.value(QLatin1String("Settings/backgroundPath"), defaultBackground).toString();

	if (!QFile::exists(backgroundPath))
		return QString();

	QImage backgroundImage{backgroundPath};
	QPixmap output = QPixmap::fromImage(blurImage(backgroundImage, backgroundImage.rect(), 10));

	QFile file{DataPaths::currentProfilePath() + "/themes" + QLatin1String("/bluredBackground.png")};
	file.open(QIODevice::WriteOnly);
	output.save(&file, "PNG");

	return file.fileName();
}

QImage Application::blurImage(const QImage& image, const QRect& rect, int radius, bool alphaOnly)
{
	int tab[] = {14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2};
	int alpha = (radius < 1) ? 16 : (radius > 17) ? 1 : tab[radius - 1];

	QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	int r1 = rect.top();
	int r2 = rect.bottom();
	int c1 = rect.left();
	int c2 = rect.right();

	int bpl = result.bytesPerLine();
	int rgba[4];
	unsigned char* p;

	int i1 = 0;
	int i2 = 3;

	if (alphaOnly)
		i1 = i2 = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);

	for (int col = c1; col <= c2; col++) {
		p = result.scanLine(r1) + col * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p += bpl;
		for (int j = r1; j < r2; j++, p += bpl)
			for (int i = i1; i <= i2; i++)
				p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
	}

	for (int row = r1; row <= r2; row++) {
		p = result.scanLine(row) + c1 * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p += 4;
		for (int j = c1; j < c2; j++, p += 4)
			for (int i = i1; i <= i2; i++)
				p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
	}

	for (int col = c1; col <= c2; col++) {
		p = result.scanLine(r2) + col * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p -= bpl;
		for (int j = r1; j < r2; j++, p -= bpl)
			for (int i = i1; i <= i2; i++)
				p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
	}

	for (int row = r1; row <= r2; row++) {
		p = result.scanLine(row) + c2 * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p -= 4;
		for (int j = c1; j < c2; j++, p -= 4)
			for (int i = i1; i <= i2; i++)
				p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
	}

	return result;
}

void Application::loadThemeFromResources(QString name, bool loadAtEnd)
{
	QString defaultThemePath{DataPaths::currentProfilePath() + "/themes"};
	QString defaultThemeDataPath{":/" + name + "/data/themes/" + name};

	defaultThemePath += QLatin1Char('/') + name;

	QDir(defaultThemePath).removeRecursively();
	QDir().mkpath(defaultThemePath + QLatin1String("/images"));

	copyPath(QDir(defaultThemeDataPath).absolutePath(), defaultThemePath);

	if (loadAtEnd)
		loadTheme(name);
}

bool Application::copyPath(const QString& fromDir, const QString& toDir, bool coverFileIfExist)
{
	QDir sourceDir(fromDir);
	QDir targetDir(toDir);
	if (!targetDir.exists()) {
		/* if directory don't exists, build it */
		if (!targetDir.mkdir(targetDir.absolutePath()))
			return false;
	}

	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList)
	{
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isDir()) {
			/* if it is directory, copy recursively*/
			if (!copyPath(fileInfo.filePath(),
				targetDir.filePath(fileInfo.fileName()),
				coverFileIfExist))
				return false;
		}
		else {
			/* if coverFileIfExist == true, remove old file first */
			if (coverFileIfExist && targetDir.exists(fileInfo.fileName())) {
				targetDir.remove(fileInfo.fileName());
			}

			// files copy
			if (!QFile::copy(fileInfo.filePath(),
				targetDir.filePath(fileInfo.fileName())) ||
				!QFile::setPermissions(targetDir.filePath(fileInfo.fileName()),
				QFileDevice::ReadUser | QFileDevice::WriteUser)) {
				return false;
			}
		}
	}
	return true;
}

QString Application::readFile(const QString& filename)
{
	QFile file{filename};

	if (!filename.isEmpty() && file.open(QFile::ReadOnly)) {
		const QByteArray array{file.readAll()};

		file.close();

		return QString::fromUtf8(array);
	}

	return QString();
}
}
