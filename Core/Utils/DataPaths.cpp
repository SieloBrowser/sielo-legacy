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

#include "DataPaths.hpp"

#include <QDir> 
#include <QStandardPaths>

#include <QApplication>

#include "Application.hpp"

namespace Sn
{
Q_GLOBAL_STATIC(DataPaths, sn_data_paths)

DataPaths::DataPaths()
{
// AppData
#if defined(Q_OS_MACOS)
	m_paths[AppData].append(QApplication::applicationDirPath() + QLatin1String("/../Resources"));
#else
	m_paths[AppData].append(QApplication::applicationDirPath());
#endif

	m_paths[Translations].append(m_paths[AppData][0] + QLatin1String("/locale"));
	m_paths[Themes].append(m_paths[AppData][0] + QLatin1String("/themes"));
	m_paths[Plugins].append(m_paths[AppData][0] + QLatin1String("/plugins"));


#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
	// Use %LOCALAPPDATA%/sielo as Config path on Windows
	m_paths[Config].append(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
#elif defined(Q_OS_MACOS)
	m_paths[Config].append(QDir::homePath() + QLatin1String("/Library/Application Support/Sielo"));
#else // Unix
	m_paths[Config].append(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QLatin1String("/sielo"));
#endif


	// Profiles
	m_paths[Profiles].append(m_paths[Config].at(0) + QLatin1String("/profiles"));

	// Temp
#ifdef Q_OS_UNIX
	const QByteArray& user{qgetenv("USER")};
	const QString& tempPath{QString("%1/sielo-%2").arg(QDir::tempPath(), user.constData())};
	m_paths[Temp].append(tempPath);
#else
	m_paths[Temp].append(m_paths[Config].at(0) + QLatin1String("/tmp"));
#endif

	// Cache
#ifdef Q_OS_UNIX
	const QString &cachePath{QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)};
	if (!cachePath.isEmpty())
		m_paths[Cache].append(cachePath + QLatin1String("/sielo"));
#endif

	// Make sure the Config and Temp paths exists
	QDir dir{};
	dir.mkpath(m_paths[Config].at(0));
	dir.mkpath(m_paths[Temp].at(0));

	// We also allow to load data from Config path
	m_paths[Translations].append(m_paths[Config][0] + QLatin1String("/locale"));
	m_paths[Themes].append(m_paths[Config][0] + QLatin1String("/themes"));
	m_paths[Plugins].append(m_paths[Config][0] + QLatin1String("/plugins"));

}

void DataPaths::setCurrentProfilePath(const QString& profilePath)
{
	sn_data_paths()->initCurrentProfile(profilePath);
}

void DataPaths::setPortableVersion()
{
	DataPaths* dataPaths = sn_data_paths();

	dataPaths->m_paths[Config] = dataPaths->m_paths[AppData];

	dataPaths->m_paths[Profiles] = dataPaths->m_paths[Config];
	dataPaths->m_paths[Profiles].first().append(QLatin1String("/profiles"));

	dataPaths->m_paths[Temp] = dataPaths->m_paths[Config];
	dataPaths->m_paths[Temp].first().append(QLatin1String("/tmp"));

	// Make sure the Config and Temp paths exists
	QDir dir{};
	dir.mkpath(dataPaths->m_paths[Config][0]);
	dir.mkpath(dataPaths->m_paths[Temp][0]);
}

QString DataPaths::path(Path type)
{
	Q_ASSERT(!sn_data_paths()->m_paths[type].isEmpty());

	return sn_data_paths()->m_paths[type][0];
}

QStringList DataPaths::allPaths(Path type)
{
	Q_ASSERT(!sn_data_paths()->m_paths[type].isEmpty());

	return sn_data_paths()->m_paths[type];
}

QString DataPaths::currentProfilePath()
{
	return path(CurrentProfile);
}

void DataPaths::clearTempData()
{
	Application::removeDirectory(path(Temp));
}

void DataPaths::initCurrentProfile(const QString& profilePath)
{
	m_paths[CurrentProfile].append(profilePath);

	if (m_paths[Cache].isEmpty())
		m_paths[Cache].append(m_paths[CurrentProfile][0] + QLatin1String("/cache"));

	if (m_paths[Sessions].isEmpty())
		m_paths[Sessions].append(m_paths[CurrentProfile][0] + QLatin1String("/sessions"));

	QDir dir;
	dir.mkpath(m_paths[Cache][0]);
	dir.mkpath(m_paths[Sessions][0]);
}
}
