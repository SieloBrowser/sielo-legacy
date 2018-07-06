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

#include <QDesktopServices>
#include <QFontDatabase>

#include <QMessageBox>

#include <QSettings>

#include <QWebEngineSettings>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <AdBlock/Manager.hpp>

#include <ndb/option.hpp>

#include "BrowserWindow.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Password/AutoFill/AutoFill.hpp"

#include "Cookies/CookieJar.hpp"

#include "History/History.hpp"
#include "Bookmarks/Bookmarks.hpp"
#include "MaquetteGrid/MaquetteGrid.hpp"
#include "MaquetteGrid/MaquetteGridManager.hpp"
#include "Download/DownloadManager.hpp"

#include "Utils/RegExp.hpp"
#include "Database/SqlDatabase.hpp"
#include "Utils/CommandLineOption.hpp"
#include "Utils/Updater.hpp"

#include "Web/WebPage.hpp"
#include "Web/Scripts.hpp"
#include "Web/HTML5Permissions/HTML5PermissionsManager.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Network/NetworkManager.hpp"

#include "Widgets/TitleBar.hpp"
#include "Widgets/NavigationControlDialog.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/AddressBar/AddressBar.hpp"
#include "Widgets/Preferences/Appearance.hpp"

namespace Sn
{
QString Application::currentVersion = QString("1.17.00 closed-beta");

// Static member
QList<QString> Application::paths()
{
	/* 
	 * On Windows : P_Data = C:\Users\%userame%\AppData\Roaming\Feldrise\Sielo
	 * On Linux : P_Data = /home/%username%/.local/share/feldrise/Sielo
	 * On macOS : P_Data = Unknown
	 */
	QList<QString> paths{};

	paths.append(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
	paths.append(paths[Application::P_Data] + QLatin1String("/plugins"));
	paths.append(paths[Application::P_Data] + QLatin1String("/themes"));
	paths.append(paths[Application::P_Data] + QLatin1String("/locale"));
	paths.append(paths[Application::P_Data] + QLatin1String("/maquette-grid"));

	return paths;
}

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
	QCoreApplication::setOrganizationName(QLatin1String("Feldrise"));
	QCoreApplication::setApplicationName(QLatin1String("Sielo"));
	QCoreApplication::setApplicationVersion(QLatin1String("1.17.00"));

	setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
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
	/*int id = QFontDatabase::addApplicationFont(":data/fonts/morpheus.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	m_morpheusFont = QFont(family);
	m_normalFont = font();*/

	// the 3rd parameter is the site id
	m_piwikTracker = new PiwikTracker(this, QUrl("https://sielo.app/analytics"), 1);
	m_piwikTracker->sendVisit("launch");

	// Check command line options with given arguments
	QUrl startUrl{};
	QStringList messages;

	bool newInstance{false};

	// Check startup arguments
	if (argc > 1) {
		CommandLineOption command{};

		foreach (const CommandLineOption::ActionPair& pair, command.getActions()) {
			switch (pair.action) {
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
		foreach (const QString& message, messages) {
			sendMessage(message.toUtf8());
		}

		return;
	}

	// Open urls from Sielo dialogs in Sielo itself
	QDesktopServices::setUrlHandler("http", this, "addNewTab");
	QDesktopServices::setUrlHandler("https", this, "addNewTab");
	QDesktopServices::setUrlHandler("ftp", this, "addNewTab");

	connectDatabase(); // connect ndb

	m_plugins = new PluginProxy;

	// Setting up web and network objects
	m_webProfile = privateBrowsing() ? new QWebEngineProfile(this) : QWebEngineProfile::defaultProfile();
	connect(m_webProfile, &QWebEngineProfile::downloadRequested, this, &Application::downloadRequested);

	loadSettings();
	translateApplication();

	m_networkManager = new NetworkManager(this);
	m_autoFill = new AutoFill;

	// Setup web channel with custom script (mainly for autofill)
	QString webChannelScriptSrc = Scripts::webChannelDefautlScript();

	QWebEngineScript script{};

	script.setName(QStringLiteral("_sielo_webchannel"));
	script.setInjectionPoint(QWebEngineScript::DocumentCreation);
	script.setWorldId(QWebEngineScript::MainWorld);
	script.setRunsOnSubFrames(true);
	script.setSourceCode(webChannelScriptSrc.arg(readFile(QStringLiteral(":/qtwebchannel/qwebchannel.js"))));

	m_webProfile->scripts()->insert(script);

	// Check if we start after a crash
	if (!privateBrowsing()) {
		QSettings settings{};

		m_startingAfterCrash = settings.value(QLatin1String("isRunning"), false).toBool();
		settings.setValue(QLatin1String("isRunning"), true);

		if (m_startingAfterCrash)
			startAfterCrash();
	}

	// Create or restore window
	BrowserWindow* window{createWindow(Application::WT_FirstAppWindow, startUrl)};

	if ((isStartingAfterCrash() && afterCrashLaunch() == RestoreSession) ||
		(afterLaunch() == RestoreSession || afterLaunch() == OpenSavedSession)) {
		if (!(isStartingAfterCrash() && afterCrashLaunch() == Application::AfterLaunch::OpenHomePage)) {
			m_restoreManager = new RestoreManager();
			if (!m_restoreManager->isValid())
				destroyRestoreManager();
			else
				QFile::remove(paths()[Application::P_Data] + QLatin1String("/pinnedtabs.dat"));
		}
	}

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

	delete m_plugins;
	delete m_cookieJar;
	delete m_downloadManager;
}

void Application::loadSettings()
{
	QSettings settings;

	// General Sielo settings
	m_fullyLoadThemes = settings.value("Settings/fullyLoadThemes", true).toBool();
	m_useTopToolBar = settings.value("Settings/useTopToolBar", false).toBool();
	m_hideBookmarksHistoryActions = settings.value("Settings/hideBookmarksHistoryByDefault", false).toBool();
	m_floatingButtonFoloweMouse = settings.value("Settings/floatingButtonFoloweMouse", true).toBool();

	m_networkManager->loadSettings();

	// Load specific settings for all windows
	foreach (BrowserWindow* window, m_windows) window->loadSettings();

	// Load settings for password
	if (m_autoFill)
		m_autoFill->loadSettings();

	loadWebSettings();
	loadApplicationSettings();
	loadThemesSettings();
	loadTranslationSettings();
}

void Application::loadWebSettings()
{
	QSettings settings{};

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

	settings.endGroup();

	// Force local storage to be disabled if it's a provate session
	if (privateBrowsing()) {
		webSettings->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
		history()->setSaving(false);
	}
}

void Application::loadApplicationSettings()
{
	QSettings settings{};

	// Check the current version number of Sielo, and make setting update if needed
	//TODO: improve this with a switch
	if (settings.value("versionNumber", 0).toInt() < 14) {
		settings.setValue("installed", false);
		if (settings.value("versionNumber", 0).toInt() < 12) {
			if (settings.value("versionNumber", 0).toInt() < 11) {
				// Update with new bookmarks
				QString directory{ Application::instance()->paths()[Application::P_Data] };
				QFile::remove(directory
					+ QLatin1String("/bookmarks.xbel"));
				QFile::copy(QLatin1String(":data/bookmarks.xbel"), directory
					+ QLatin1String("/bookmarks.xbel"));
				QFile::setPermissions(directory
					+ QLatin1String("/bookmarks.xbel"),
					QFileDevice::ReadUser | QFileDevice::WriteUser);
			}

			// Update home page to use last version of doosearch
			settings.setValue("Web-Settings/homePage", "https://doosearch.sielo.app/");
			settings.setValue("Web-Settings/urlOnNewTab", "https://doosearch.sielo.app/");

			foreach(BrowserWindow* window, m_windows) {
				window->loadSettings();
				for (int i{ 0 }; i < window->tabWidgetsCount(); ++i) {
					window->tabWidget(i)->setHomeUrl("https://doosearch.sielo.app");
				}
			}
		}
		settings.setValue("versionNumber", 14);
	}
}

void Application::loadThemesSettings()
{
	QSettings settings{};

	// Load theme info
	QFileInfo themeInfo{
		paths()[Application::P_Themes] + QLatin1Char('/')
		+ settings.value("Themes/currentTheme", "sielo-default").toString()
		+ QLatin1String("/main.sss")
	};

	// Check if the theme existe
	if (themeInfo.exists()) {
		// Check default theme version and update it if needed
		if (settings.value("Themes/defaultThemeVersion", 1).toInt() < 43) {
			if (settings.value("Themes/defaultThemeVersion", 1).toInt() < 11) {
				QString defaultThemePath{paths()[Application::P_Themes]};

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
			settings.setValue("Themes/defaultThemeVersion", 43);
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
		settings.setValue("Themes/defaultThemeVersion", 43);
	}
}

void Application::loadTranslationSettings()
{
	QSettings settings{};
	settings.beginGroup("Language");

	if (settings.value("version", 0).toInt() < 14) {
		QDir(paths()[P_Translations]).removeRecursively();
		copyPath(QDir(":data/locale").absolutePath(), paths()[P_Translations]);
		settings.setValue("version", 14);
	}
}

void Application::translateApplication()
{
	QSettings settings{};
	QString file{settings.value("Language/language", QLocale::system().name()).toString()};

	// It can only be "C" locale, for which we will use default English language
	if (file.size() < 2)
		file.clear();

	if (!file.isEmpty() && !file.endsWith(QLatin1String(".qm")))
		file.append(QLatin1String(".qm"));

	// Either we load default language (with empty file), or we attempt to load xx.qm (xx_yy.qm)
	Q_ASSERT(file.isEmpty() || file.size() >= 5);

	QString translationPath{paths()[P_Translations]};

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
	BrowserWindow* maquetteGridWindow = new BrowserWindow(item);

	QObject::connect(maquetteGridWindow, &BrowserWindow::destroyed, this, &Application::windowDestroyed);

	m_windows.prepend(maquetteGridWindow);
	return maquetteGridWindow;
}

Application::AfterLaunch Application::afterLaunch() const
{
	QSettings settings{};

	return static_cast<AfterLaunch>(settings.value(QStringLiteral("Settings/afterLaunch"), OpenHomePage).toInt());
}

bool Application::restoreSession(BrowserWindow* window, RestoreData restoreData)
{
	if (m_privateBrowsing || restoreData.isEmpty())
		return false;

	// Make the user know that Sielo is working
	m_isRestoring = true;
	setOverrideCursor(Qt::BusyCursor);

	window->setUpdatesEnabled(false);
	window->tabWidget()->closeRecoveryTab();

	if (window->tabWidget()->normalTabsCount() > 1) {
		BrowserWindow* newWindow{createWindow(Application::WT_OtherRestoredWindow)};
		newWindow->setUpdatesEnabled(false);
		newWindow->restoreWindowState(restoreData[0]);
		newWindow->setUpdatesEnabled(true);

		restoreData.remove(0);
	}
	else {
		int tabCount{window->tabWidget()->pinnedTabsCount()};
		RestoreManager::WindowData data = restoreData[0];

		data.currentTabs[0] += tabCount;
		restoreData.remove(0);

		window->restoreWindowState(data);
	}

	window->setUpdatesEnabled(true);

	processEvents();

	foreach (const RestoreManager::WindowData& data, restoreData) {
		BrowserWindow* window{createWindow(Application::WT_OtherRestoredWindow)};

		window->setUpdatesEnabled(false);
		window->restoreWindowState(data);
		window->setUpdatesEnabled(true);

		processEvents();
	}

	destroyRestoreManager();
	restoreOverrideCursor();

	m_isRestoring = false;

	return true;
}

void Application::destroyRestoreManager()
{
	delete m_restoreManager;
	m_restoreManager = nullptr;
}

void Application::saveSettings()
{
	// Save settings of AdBlock
	ADB::Manager::instance()->save();

	// If we are in private browsing, we don't want to save settings obviously
	if (privateBrowsing())
		return;

	QSettings settings{};

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

	m_piwikTracker->sendEvent("exit", "exit", "exit", "exit application");
}

void Application::saveSession(bool saveForHome)
{
	if (m_privateBrowsing || m_isRestoring || m_windows.count() == 0 || m_restoreManager)
		return;

	QByteArray data{};
	QDataStream stream{&data, QIODevice::WriteOnly};

	// Write the current session in version 2
	stream << 0x0003;
	stream << m_windows.count();

	// Save tabs of all windows
	foreach (BrowserWindow* window, m_windows) {
		window->titleBar()->saveToolBarsPositions();
		stream << window->saveTabs();

		// Save state of window (is it's in full screen)
		if (window->isFullScreen())
			stream << QByteArray();
		else
			stream << window->saveState();

		stream << window->saveGeometry();
	}

	// Save data to a file
	QFile file{};
	if (saveForHome)
		file.setFileName(paths()[Application::P_Data] + QLatin1String("/home-session.dat"));
	else
		file.setFileName(paths()[Application::P_Data] + QLatin1String("/session.dat"));

	file.open(QIODevice::WriteOnly);
	file.write(data);
	file.close();
}

void Application::reloadUserStyleSheet()
{
	QSettings setting{};
	QString userCSSFile{setting.value("Settings/userStyleSheet", QString()).toString()};

	setUserStyleSheet(userCSSFile);
}

void Application::quitApplication()
{
	m_isClosing = true;

	quit();
}

void Application::postLaunch()
{
	// Check if we want to open a new tab
	if (m_postLaunchActions.contains(OpenNewTab))
		getWindow()->tabWidget()->addView(QUrl(), Application::NTT_SelectedNewEmptyTab);

	QSettings settings{};

	// Show the "getting started" page if it's the first time Sielo is launch
	if (!settings.value("installed", false).toBool()) {
		m_piwikTracker->sendEvent("installation", "installation", "installation", "new installation");

		getWindow()->tabWidget()
		           ->addView(QUrl("https://sielo.app/thanks.php"),
		                     Application::NTT_CleanSelectedTabAtEnd);

		NavigationControlDialog* navigationControlDialog{ new NavigationControlDialog(getWindow()) };
		navigationControlDialog->exec();

		settings.setValue("installed", true);
	}

	connect(this, &Application::receivedMessage, this, &Application::messageReceived);
	connect(this, &Application::aboutToQuit, this, &Application::saveSettings);
}

void Application::windowDestroyed(QObject* window)
{
	Q_ASSERT(static_cast<BrowserWindow*>(window));
	Q_ASSERT(m_windows.contains(static_cast<BrowserWindow*>(window)));

	m_windows.removeOne(static_cast<BrowserWindow*>(window));
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

	// Since some people seem to be able to run only the 32 bit version...
	if (!is32bit()) {
		QMessageBox::information(nullptr, QApplication::tr("Info"), QApplication::tr(
			                         "Please, if Sielo continues crashing, consider trying this 32bit version."));
	}

	if (requestAction.clickedButton() == restoreSession) {
		m_afterCrashLaunch = AfterLaunch::RestoreSession;
	}
	else {
		m_afterCrashLaunch = AfterLaunch::OpenHomePage;
	}
}

void Application::connectDatabase()
{
	if (!QDir(paths()[Application::P_Data] + QLatin1String("/database")).exists())
		QDir().mkpath(paths()[Application::P_Data] + QLatin1String("/database"));

	// ndb params
	ndb::connection_param params{};

	// We want to throw an error if we attemp to write in database in private mode
	if (m_privateBrowsing) {
		params.flag = ndb::connection_flag::read_only;
	}

	// Databases are stored in a specific folder in %appdata%
	params.path = QString(paths()[Application::P_Data] + QLatin1String("/database")).toStdString();

	// Connecte databases
	ndb::connect<dbs::password>(params);
	ndb::connect<dbs::navigation>(params);
	ndb::connect<dbs::image>(params);

	// TODO: remove this at the end of ndb integration
	/*const QString dbFile = paths()[Application::P_Data] + QLatin1String("/browsedata.db");

	if (m_databaseConnected)
		QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());

	QSqlDatabase db{QSqlDatabase::addDatabase(QLatin1String("QSQLITE"))};

	db.setDatabaseName(dbFile);

	if (!QFile::exists(dbFile)) {
		qWarning() << "Cannot find SQLite database file! Copying and using the defaults!";

		QFile(":data/browsedata.db").copy(dbFile);
		QFile(dbFile).setPermissions(QFile::ReadUser | QFile::WriteUser);

		db.setDatabaseName(dbFile);
	}

	if (m_privateBrowsing)
		db.setConnectOptions("QSQLITE_OPEN_READONLY");

	if (!db.open())
		qWarning() << "Cannot open SQLite database! Continuing without database...";
*/
	m_databaseConnected = true;
}

void Application::processCommand(const QString& command, const QStringList args)
{
	if (m_plugins->processCommand(command, args))
		return;

	if (command == "site") {
		LoadRequest siteRequest{};
		siteRequest.setUrl(QUrl("http://www.feldrise.com/Sielo/"));

		getWindow()->tabWidget()->weTab()->webView()
		           ->loadInNewTab(siteRequest, Application::NTT_CleanSelectedTabAtEnd);
	}

	if (command == "github") {
		LoadRequest githubRequest{};
		githubRequest.setUrl(QUrl("https://github.com/Feldrise/SieloNavigateur"));

		getWindow()->tabWidget()->weTab()->webView()
		           ->loadInNewTab(githubRequest, Application::NTT_CleanSelectedTabAtEnd);
	}

	/*if (command == "witcher") {
		if (args.count() == 1) {
			QSettings settings{};
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
						for (int i{0}; i < window->tabWidgetsCount(); ++i) {
							for (int j{0}; j < window->tabWidget(i)->count(); ++j) {
								window->tabWidget(i)->weTab(j)->addressBar()
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
						for (int i{0}; i < window->tabWidgetsCount(); ++i) {
							for (int j{0}; j < window->tabWidget(i)->count(); ++j) {
								window->tabWidget(i)->weTab(j)->addressBar()
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
	}*/
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
		int easteregg{qrand() % ((sitesCount) - 0) + 0};

		LoadRequest eastereggRequest{};
		eastereggRequest.setUrl(eastereggs[easteregg]);

		getWindow()->tabWidget()->weTab()->webView()
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
	QString activeThemePath{Application::instance()->paths()[Application::P_Themes] + QLatin1Char('/') + name};
	QString sss{readFile(activeThemePath + QLatin1String("/main.sss"))};

	// If the theme use user color API
	if (QDir(activeThemePath + "/dark").exists() && QDir(activeThemePath + "/light").exists()) {
		QIcon::setThemeSearchPaths(QStringList() << activeThemePath);
		QIcon::setThemeName(lightness);
	}
	else {
		QIcon::setThemeSearchPaths(QStringList() << Application::instance()->paths()[Application::P_Themes]);
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
	QSettings settings{};

	QString backgroundPath = settings.value(QLatin1String("Settings/backgroundPath"), defaultBackground).toString();

	if (!QFile::exists(backgroundPath))
		return QString();

	QImage backgroundImage{ backgroundPath };
	QPixmap output = QPixmap::fromImage(blurImage(backgroundImage, backgroundImage.rect(), 10));
	
	QFile file{ paths()[Application::P_Themes] + QLatin1String("/bluredBackground.png") };
	file.open(QIODevice::WriteOnly);
	output.save(&file, "PNG");

	return file.fileName();
}

QImage Application::blurImage(const QImage& image, const QRect& rect, int radius, bool alphaOnly)
{
	int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
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
	QString defaultThemePath{paths()[Application::P_Themes]};
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
	foreach(QFileInfo fileInfo, fileInfoList) {
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
