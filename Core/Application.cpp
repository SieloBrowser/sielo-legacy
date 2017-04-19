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

#include <QMessageBox>

#include <QSettings>

#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

#include "BrowserWindow.hpp"

#include "Plugins/PluginProxy.hpp"

#include "History/HistoryManager.hpp"
#include "Bookmarks/BookmarkManager.hpp"

#include "Utils/RegExp.hpp"

#include "Web/WebPage.hpp"
#include "Web/HTML5Permissions/HTML5PermissionsManager.hpp"

#include "Network/NetworkManager.hpp"

#include "Widgets/Tab/TabWidget.hpp"

namespace Sn {

// Static member
QList<QString> Application::paths()
{
	QList<QString> paths{};

	paths.append(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
	paths.append(paths[Application::P_Data] + "/plugins");

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
	m_historyManager(nullptr),
	m_networkManager(nullptr),
	m_webProfile(nullptr)
{
	QCoreApplication::setOrganizationName(QLatin1String("Feldrise"));
	QCoreApplication::setApplicationName(QLatin1String("Sielo"));
	QCoreApplication::setApplicationVersion(QLatin1String("1.0.0"));

	m_plugins = new PluginProxy;
	m_webProfile = m_privateBrowsing ? new QWebEngineProfile(this) : QWebEngineProfile::defaultProfile();
	m_networkManager = new NetworkManager(this);

	loadTheme("sielo");

}

Application::~Application()
{
	delete m_plugins;
	delete m_historyManager;
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

void Application::saveSession()
{
	if (m_privateBrowsing || m_windows.count() == 0)
		return;

	QByteArray data{};
	QDataStream stream{&data, QIODevice::WriteOnly};

	stream << QString("0.5");
	stream << m_windows.count();

		foreach (BrowserWindow* window, m_windows) {
			stream << window->tabWidget()->saveState();
			if (window->isFullScreen())
				stream << QByteArray();
			else
				stream << window->saveState();
		}

	QFile file{paths()[Application::P_Data] + QLatin1String("/session.dat")};

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

HTML5PermissionsManager* Application::permissionsManager()
{
	if (!m_permissionsManager)
		m_permissionsManager = new HTML5PermissionsManager(this);

	return m_permissionsManager;
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

void Application::loadTheme(const QString& name)
{
	QString activeThemePath{Application::instance()->paths()[Application::P_Data] + QLatin1String("/themes/") + name};
	QFile theme{activeThemePath + QLatin1String("/main.sss")};
	QByteArray array{};

	if (theme.open(QFile::ReadOnly)) {
		array = theme.readAll();
		theme.close();
	}

	QString sss{QString::fromUtf8(array)};
	QString relativePath{QDir::current().relativeFilePath(activeThemePath)};

	sss.replace(RegExp(QStringLiteral("url\\s*\\(\\s*([^\\*:\\);]+)\\s*\\)")),
				QString("url(%1/\\1)").arg(relativePath));
	sss.replace("sproperty", "qproperty");
	sss.replace("slineargradient", "qlineargradient");

	setStyleSheet(sss);
}

}
