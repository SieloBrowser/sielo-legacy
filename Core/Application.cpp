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

#include "Application.hpp"

#include <QString>
#include <QStandardPaths>
#include <QDir>

#include <QStyle>

#include <QSqlDatabase>

#include <QDesktopServices>
#include <QFontDatabase>

#include <QMessageBox>

#include <QSettings>

#include <QWebEngineSettings>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

#include "BrowserWindow.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Password/AutoFill/AutoFill.hpp"

#include "History/HistoryManager.hpp"
#include "Bookmarks/BookmarkManager.hpp"
#include "Download/DownloadManager.hpp"

#include "Utils/RegExp.hpp"
#include "Utils/CommandLineOption.hpp"
#include "Utils/Updater.hpp"

#include "Web/WebPage.hpp"
#include "Web/HTML5Permissions/HTML5PermissionsManager.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Network/NetworkManager.hpp"

#include "Widgets/FloatingButton.hpp"
#include "Widgets/AddressBar.cpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/TabBar.hpp"

namespace Sn {

QString Application::currentVersion = QString("1.2.02e");

// Static member
QList<QString> Application::paths()
{
	QList<QString> paths{};

	paths.append(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
	paths.append(paths[Application::P_Data] + QLatin1String("/plugins"));
	paths.append(paths[Application::P_Data] + QLatin1String("/themes"));

	return paths;
}
Application* Application::instance()
{
	return (static_cast<Application*>(QCoreApplication::instance()));
}

// Constructor&  destructor
Application::Application(int& argc, char** argv) :
	QApplication(argc, argv),
	m_plugins(nullptr),
	m_autoFill(nullptr),
	m_historyManager(nullptr),
	m_networkManager(nullptr),
	m_webProfile(nullptr)
{
	QCoreApplication::setOrganizationName(QLatin1String("Feldrise"));
	QCoreApplication::setApplicationName(QLatin1String("Sielo"));
	QCoreApplication::setApplicationVersion(QLatin1String("1.0.0"));

	// QSQLITE database plugin is required
	if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE"))) {
		QMessageBox::critical(0,
							  QStringLiteral("Error"),
							  QStringLiteral(
								  "Qt SQLite database plugin is not available. Please install it and restart the application."));
		return;
	}

	int id = QFontDatabase::addApplicationFont(":data/fonts/morpheus.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	m_morpheusFont = QFont(family);
	m_normalFont = font();


	loadSettings();

	QUrl startUrl{};

	bool newInstance{false};

	if (argc > 1) {
		CommandLineOption command{};

			foreach (const CommandLineOption::ActionPair& pair, command.getActions()) {
				switch (pair.action) {
				case Application::CL_NewTab:
					m_postLaunchActions.append(OpenNewTab);
					break;
				case Application::CL_NewWindow:
					break;
				case Application::CL_StartPrivateBrowsing:
					m_privateBrowsing = true;
					break;
				case Application::CL_StartNewInstance:
					newInstance = true;
					break;
				case Application::CL_OpenUrlInCurrentTab:
					startUrl = QUrl::fromUserInput(pair.text);
					break;
				case Application::CL_OpenUrlInNewWindow:
					startUrl = QUrl::fromUserInput(pair.text);
					break;
				case Application::CL_OpenUrl:
					startUrl = QUrl::fromUserInput(pair.text);
					break;
				default:
					break;

				}
			}
	}


	QDesktopServices::setUrlHandler("http", this, "addNewTab");
	QDesktopServices::setUrlHandler("ftp", this, "addNewTab");

	connectDatabase();

	m_plugins = new PluginProxy;

	m_webProfile = m_privateBrowsing ? new QWebEngineProfile(this) : QWebEngineProfile::defaultProfile();
	m_networkManager = new NetworkManager(this);
	m_autoFill = new AutoFill;

	QString webChannelScriptSrc = QLatin1String("(function() {"
													"%1"
													""
													"function registerExternal(e) {"
													"    window.external = e;"
													"    if (window.external) {"
													"        var event = document.createEvent('Event');"
													"        event.initEvent('_sielo_external_created', true, true);"
													"        document.dispatchEvent(event);"
													"    }"
													"}"
													""
													"if (self !== top) {"
													"    if (top.external)"
													"        registerExternal(top.external);"
													"    else"
													"        top.document.addEventListener('_sielo_external_created', function() {"
													"            registerExternal(top.external);"
													"        });"
													"    return;"
													"}"
													""
													"function registerWebChannel() {"
													"    try {"
													"        new QWebChannel(qt.webChannelTransport, function(channel) {"
													"            registerExternal(channel.objects.sielo_object);"
													"        });"
													"    } catch (e) {"
													"        setTimeout(registerWebChannel, 100);"
													"    }"
													"}"
													"registerWebChannel();"
													""
													"})()");

	QWebEngineScript script{};

	script.setName(QStringLiteral("_sielo_webchannel"));
	script.setInjectionPoint(QWebEngineScript::DocumentCreation);
	script.setWorldId(QWebEngineScript::MainWorld);
	script.setRunsOnSubFrames(true);
	script.setSourceCode(webChannelScriptSrc.arg(readFile(QStringLiteral(":/qtwebchannel/qwebchannel.js"))));

	m_webProfile->scripts()->insert(script);

	BrowserWindow* window{createWindow(Application::WT_FirstAppWindow, startUrl)};

	if (afterLaunch() == RestoreSession || afterLaunch() == OpenSavedSession) {
		m_restoreManager = new RestoreManager();
		if (!m_restoreManager->isValid())
			destroyRestoreManager();
		else
			QFile::remove(paths()[Application::P_Data] + QLatin1String("/pinnedtabs.dat"));
	}

	Updater* updater{new Updater(window)};

	QTimer::singleShot(0, this, &Application::postLaunch);
}

Application::~Application()
{
	if (m_windows.count() > 0)
		saveSession();

	delete m_plugins;
	delete m_historyManager;
	delete m_downloadManager;
}

void Application::loadSettings()
{
	QSettings settings;

	m_useTopToolBar = settings.value("Settings/useTopToolBar", false).toBool();
	m_floatingButtonFoloweMouse = settings.value("Settings/floatingButtonFoloweMouse", true).toBool();

	if (settings.value("Settings/useMorpheusFont", false).toBool())
		setFont(m_morpheusFont);

		foreach (BrowserWindow* window, m_windows) window->loadSettings();

	QFileInfo themeInfo{paths()[Application::P_Themes] + QLatin1Char('/')
						+ settings.value("Themes/currentTheme", "sielo_default").toString()
						+ QLatin1String("/main.sss")};

	QWebEngineSettings* webSettings = QWebEngineSettings::defaultSettings();
	QWebEngineProfile* webProfile = QWebEngineProfile::defaultProfile();

	settings.beginGroup("Web-Settings");

	webSettings->setAttribute(QWebEngineSettings::PluginsEnabled, settings.value("allowPlugins", true).toBool());
	webSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, settings.value("allowJavaScript", true).toBool());
	webSettings->setAttribute(QWebEngineSettings::LinksIncludedInFocusChain,
							  settings.value("includeLinkInFocusChain", false).toBool());
	webSettings->setAttribute(QWebEngineSettings::XSSAuditingEnabled, settings.value("XSSAuditing", false).toBool());
	webSettings
		->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, settings.value("animateScrolling", true).toBool());
	webSettings->setAttribute(QWebEngineSettings::SpatialNavigationEnabled,
							  settings.value("spatialNavigation", false).toBool());
	webSettings->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, false);
	webSettings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
	webSettings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

	setWheelScrollLines(settings.value("wheelScrollLines", wheelScrollLines()).toInt());

	const bool allowCache{settings.value("allowLocalCache", true).toBool()};
	webProfile->setHttpCacheType(allowCache ? QWebEngineProfile::DiskHttpCache : QWebEngineProfile::MemoryHttpCache);
	webProfile->setCachePath(settings.value("cachePath", webProfile->cachePath()).toString());

	settings.endGroup();

	if (m_autoFill)
		m_autoFill->loadSettings();

	if (themeInfo.exists())
		loadTheme(settings.value("Themes/currentTheme", QLatin1String("sielo_default")).toString());
	else
		loadThemeFromResources();
}

int Application::windowCount() const
{
	return m_windows.count();
}

BrowserWindow* Application::getWindow() const
{
	if (m_lastActiveWindow)
		return m_lastActiveWindow.data();

	return m_windows.isEmpty() ? nullptr : m_windows[0];
}

BrowserWindow* Application::createWindow(Application::WindowType type, const QUrl& startUrl)
{
	if (windowCount() == 0)
		type = Application::WT_FirstAppWindow;

	BrowserWindow* window{new BrowserWindow(type, startUrl)};
	QObject::connect(window, &BrowserWindow::destroyed, this, &Application::windowDestroyed);

	m_windows.prepend(window);
	return window;
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
	QSettings settings{};

	settings.beginGroup("Web-Settings");

	bool deleteHistory
		{settings.value("deleteHistoryOnClose", false).toBool() || !settings.value("allowHistory", true).toBool()};

	settings.endGroup();

	if (deleteHistory)
		m_historyManager->clear();
}

void Application::saveSession(bool saveForHome)
{
	if (m_privateBrowsing || m_isRestoring || m_windows.count() == 0 || m_restoreManager)
		return;

	QByteArray data{};
	QDataStream stream{&data, QIODevice::WriteOnly};

	stream << 0x0002;
	stream << m_windows.count();

		foreach (BrowserWindow* window, m_windows) {
			stream << window->saveTabs();

			if (window->isFullScreen())
				stream << QByteArray();
			else
				stream << window->saveState();
		}

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

void Application::postLaunch()
{
	if (m_postLaunchActions.contains(OpenNewTab))
		getWindow()->tabWidget()->addView(QUrl(), Application::NTT_SelectedNewEmptyTab);

	connect(this, &Application::aboutToQuit, this, &Application::saveSettings);
}

void Application::windowDestroyed(QObject* window)
{
	Q_ASSERT(static_cast<BrowserWindow*>(window));
	Q_ASSERT(m_windows.contains(static_cast<BrowserWindow*>(window)));

	m_windows.removeOne(static_cast<BrowserWindow*>(window));
}

void Application::setUserStyleSheet(const QString& filePath)
{
	QString userCSS{};
	QFile file{filePath};
	QByteArray array{};

	if (!filePath.isEmpty() && file.open(QFile::ReadOnly)) {
		array = file.readAll();
		file.close();
	}

	userCSS += QString::fromUtf8(array).remove(QLatin1Char('\n'));

	const QString name{QStringLiteral("_sielo_userstylesheet")};
	QWebEngineScript oldScript = m_webProfile->scripts()->findScript(name);

	if (!oldScript.isNull())
		m_webProfile->scripts()->remove(oldScript);

	if (userCSS.isEmpty())
		return;

	QWebEngineScript script{};
	script.setName(name);
	script.setInjectionPoint(QWebEngineScript::DocumentReady);
	script.setWorldId(QWebEngineScript::ApplicationWorld);
	script.setRunsOnSubFrames(true);
	script.setSourceCode(WebPage::setCSS(userCSS));

	m_webProfile->scripts()->insert(script);
}

HistoryManager* Application::historyManager()
{
	if (!m_historyManager)
		m_historyManager = new HistoryManager();

	return m_historyManager;
}

BookmarksManager* Application::bookmarksManager()
{
	if (!m_bookmarksManager)
		m_bookmarksManager = new BookmarksManager();

	return m_bookmarksManager;
}

DownloadManager* Application::downloadManager()
{
	if (!m_downloadManager)
		m_downloadManager = new DownloadManager();

	return m_downloadManager;
}

HTML5PermissionsManager* Application::permissionsManager()
{
	if (!m_permissionsManager)
		m_permissionsManager = new HTML5PermissionsManager(this);

	return m_permissionsManager;
}

void Application::connectDatabase()
{
	const QString dbFile = paths()[Application::P_Data] + QLatin1String("/browsedata.db");

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

	m_databaseConnected = true;
}

QString Application::ensureUniqueFilename(const QString& name, const QString& appendFormat)
{
	Q_ASSERT(appendFormat.contains(QLatin1String("%1")));

	QFileInfo info{name};

	if (!info.exists())
		return name;

	const QDir directory{info.absoluteFilePath()};
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

		info.setFile(directory, file);
		++i;
	}

	return info.absoluteFilePath();

}

void Application::processCommand(const QString& command, const QStringList args)
{
	if (m_plugins->processCommand(command, args))
		return;

	if (command == "witcher") {
		if (args.count() == 1) {
			QSettings settings{};
			if (args[0] == "enable") {
				setFont(m_morpheusFont);
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
		int easteregg{qrand() % ((sitesCount) - 0) + 0};

		LoadRequest eastereggRequest{};
		eastereggRequest.setUrl(eastereggs[easteregg]);

		getWindow()->tabWidget()->weTab()->webView()
			->loadInNewTab(eastereggRequest, Application::NTT_CleanSelectedTabAtEnd);
	}
}

void Application::loadTheme(const QString& name)
{
	QString activeThemePath{Application::instance()->paths()[Application::P_Themes] + QLatin1Char('/') + name};
	QString sss{readFile(activeThemePath + QLatin1String("/main.sss"))};

#if defined(Q_OS_MAC)
	sss.append(readFile(activeThemePath + QLatin1String("/mac.sss")));
#elif defined(Q_OS_LINUX)
	sss.append(readFile(activeThemePath + QLatin1String("/linux.sss")));
#elif defined(Q_OS_WIN)
	sss.append(readFile(activeThemePath + QLatin1String("/windows.sss")));
#endif

	QString relativePath{QDir::current().relativeFilePath(activeThemePath)};

	sss.replace(RegExp(QStringLiteral("url\\s*\\(\\s*([^\\*:\\);]+)\\s*\\)")),
				QString("url(%1/\\1)").arg(relativePath));
	sss.replace("sproperty", "qproperty");
	sss.replace("slineargradient", "qlineargradient");

	setStyleSheet(sss);
}

void Application::loadThemeFromResources()
{
	QString defaultThemePath{paths()[Application::P_Themes]};
	QString defaultThemeDataPath{":data/themes/sielo_colorful_flat"};

	defaultThemePath += QLatin1String("/sielo_default");

	if (!QDir().exists(defaultThemePath) || !QDir().exists(defaultThemePath + QLatin1String("/images")))
		QDir().mkpath(defaultThemePath + QLatin1String("/images"));

	QFile::copy(defaultThemeDataPath + "/main.sss", defaultThemePath + QLatin1String("/main.sss"));
	QFile::copy(defaultThemeDataPath + "/windows.sss", defaultThemePath + QLatin1String("/windows.sss"));
	QFile::copy(defaultThemeDataPath + "/linux.sss", defaultThemePath + QLatin1String("/linux.sss"));
	QFile::copy(defaultThemeDataPath + "/theme.info", defaultThemePath + QLatin1String("/theme.info"));
	QFile::copy(defaultThemeDataPath + "/theme.png", defaultThemePath + QLatin1String("/theme.png"));


	QFile::copy(defaultThemeDataPath + "/images/icon.png",
				defaultThemePath + QLatin1String("/images/icon.png"));
	QFile::copy(defaultThemeDataPath + "/images/add-bookmark.png",
				defaultThemePath + QLatin1String("/images/add-bookmark.png"));
	QFile::copy(defaultThemeDataPath + "/images/view-bookmarks.png",
				defaultThemePath + QLatin1String("/images/view-bookmarks.png"));
	QFile::copy(defaultThemeDataPath + "/images/history.png",
				defaultThemePath + QLatin1String("/images/history.png"));
	QFile::copy(defaultThemeDataPath + "/images/back.png",
				defaultThemePath + QLatin1String("/images/back.png"));
	QFile::copy(defaultThemeDataPath + "/images/next.png",
				defaultThemePath + QLatin1String("/images/next.png"));
	QFile::copy(defaultThemeDataPath + "/images/new-tab.png",
				defaultThemePath + QLatin1String("/images/new-tab.png"));
	QFile::copy(defaultThemeDataPath + "/images/new-window.png",
				defaultThemePath + QLatin1String("/images/new-window.png"));
	QFile::copy(defaultThemeDataPath + "/images/home.png",
				defaultThemePath + QLatin1String("/images/home.png"));
	QFile::copy(defaultThemeDataPath + "/images/go.png", defaultThemePath + QLatin1String("/images/go.png"));
	QFile::copy(defaultThemeDataPath + "/images/refresh.png",
				defaultThemePath + QLatin1String("/images/refresh.png"));
	QFile::copy(defaultThemeDataPath + "/images/stop.png",
				defaultThemePath + QLatin1String("/images/stop.png"));
	QFile::copy(defaultThemeDataPath + "/images/addtab.png",
				defaultThemePath + QLatin1String("/images/addtab.png"));
	QFile::copy(defaultThemeDataPath + "/images/menu.png",
				defaultThemePath + QLatin1String("/images/menu.png"));
	QFile::copy(defaultThemeDataPath + "/images/preferences.png",
				defaultThemePath + QLatin1String("/images/preferences.png"));
	QFile::copy(defaultThemeDataPath + "/images/tab-left-arrow.png",
				defaultThemePath + QLatin1String("/images/tab-left-arrow.png"));
	QFile::copy(defaultThemeDataPath + "/images/tab-right-arrow.png",
				defaultThemePath + QLatin1String("/images/tab-right-arrow.png"));


	loadTheme("sielo_default");
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
