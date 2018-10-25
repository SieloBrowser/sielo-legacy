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

#include "ProfileManager.hpp"

#include <QDir>
#include <QFile>
#include <QSqlDatabase>

#include <QMessageBox>

#include <QSettings>

#include "Database/SqlDatabase.hpp"

#include "Utils/DataPaths.hpp"
#include "Utils/Updater.hpp"

#include "Application.hpp"

namespace Sn
{

int ProfileManager::createProfile(const QString& profileName)
{
	QDir dir{DataPaths::path(DataPaths::Profiles)};

	if (QDir(dir.absolutePath() + QLatin1Char('/') + profileName).exists())
		return -1; // Profile already exists

	if (!dir.mkdir(profileName))
		return -2; // Cannot create directory

	dir.cd(profileName);
	QFile(QLatin1String(":data/browsedata.db")).copy(dir.filePath(QLatin1String("browsedata.db")));
	QFile(dir.filePath(QLatin1String("browsedata.db"))).setPermissions(QFile::ReadUser | QFile::WriteUser);

	QFile(QLatin1String(":data/bookmarks.json")).copy(dir.filePath(QLatin1String("bookmarks.json")));
	QFile(dir.filePath(QLatin1String("bookmarks.json"))).setPermissions(QFile::ReadUser | QFile::WriteUser);

	QFile versionFile(dir.filePath(QLatin1String("version")));
	versionFile.open(QFile::WriteOnly);
	versionFile.write(Application::currentVersion.toUtf8());
	versionFile.close();

	return 0; // Success
}

bool ProfileManager::removeProfile(const QString& profileName)
{
	QDir dir{DataPaths::path(DataPaths::Profiles) + QLatin1Char('/') + profileName};

	if (!dir.exists())
		return false;

	Application::removeDirectory(dir.absolutePath());

	return true;
}

QString ProfileManager::currentProfile()
{
	QString path{DataPaths::currentProfilePath()};

	return path.mid(path.lastIndexOf(QLatin1Char('/')) + 1);
}

QString ProfileManager::startingProfile()
{
	QSettings settings{DataPaths::path(DataPaths::Profiles) + QLatin1String("/profiles.ini"), QSettings::IniFormat};

	return settings.value(QLatin1String("Profiles/startProfile"), QLatin1String("default")).toString();
}

void ProfileManager::setStartingProfile(const QString& profileName)
{
	QSettings settings{DataPaths::path(DataPaths::Profiles) + QLatin1String("/profiles.ini"), QSettings::IniFormat};

	settings.setValue(QLatin1String("Profiles/startProfile"), profileName);
}

QStringList ProfileManager::availableProfiles()
{
	QDir dir{DataPaths::path(DataPaths::Profiles)};

	return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

ProfileManager::ProfileManager()
{
// Empty
}
void ProfileManager::initConfigDir()
{
	QDir dir{DataPaths::path(DataPaths::Config)};

	if (dir.exists() && QFile(dir.filePath(QLatin1String("profiles/profiles.ini"))).exists())
		return;

	//TODO: Update from previous Sielo version

	if (!dir.exists())
		dir.mkpath(dir.absolutePath());

	dir.mkdir(QLatin1String("profiles"));
	dir.cd(QLatin1String("profiles"));

	// $Config/profiles
	QFile(dir.filePath(QLatin1String("profiles.ini"))).remove();
	QFile(QLatin1String(":data/profiles.ini")).copy(dir.filePath(QLatin1String("profiles.ini")));
	QFile(dir.filePath(QLatin1String("profiles.ini"))).setPermissions(QFile::ReadUser | QFile::WriteUser);

	dir.mkdir(QLatin1String("default"));
	dir.cd(QLatin1String("default"));

	// $Config/profiles/default
	QFile(dir.filePath(QLatin1String("browsedata.db"))).remove();
	QFile(QLatin1String(":data/browsedata.db")).copy(dir.filePath(QLatin1String("browsedata.db")));
	QFile(dir.filePath(QLatin1String("browsedata.db"))).setPermissions(QFile::ReadUser | QFile::WriteUser);

	QFile(QLatin1String(":data/bookmarks.json")).copy(dir.filePath(QLatin1String("bookmarks.json")));
	QFile(dir.filePath(QLatin1String("bookmarks.json"))).setPermissions(QFile::ReadUser | QFile::WriteUser);

	QFile versionFile(dir.filePath(QLatin1String("version")));
	versionFile.open(QFile::WriteOnly);
	versionFile.write(Application::currentVersion.toUtf8());
	versionFile.close();
}

void ProfileManager::initCurrentProfile(const QString& profileName)
{
	QString profilePath{DataPaths::path(DataPaths::Profiles) + QLatin1Char('/')};

	if (profileName.isEmpty())
		profilePath.append(startingProfile());
	else
		profilePath.append(profileName);

	DataPaths::setCurrentProfilePath(profilePath);

	updateCurrentProfile();
	connectDatabase();
}

void ProfileManager::updateCurrentProfile() const
{
	QDir profileDir{DataPaths::currentProfilePath()};

	if (!profileDir.exists()) {
		QDir newDir{profileDir.path().remove(profileDir.dirName())};
		newDir.mkdir(profileDir.dirName());
	}

	QFile versionFile{profileDir.filePath(QLatin1String("version"))};

	if (versionFile.exists()) {
		versionFile.open(QFile::ReadOnly);
		QString profileVersion{QString::fromUtf8(versionFile.readAll())};
		versionFile.close();

		updateProfile(Application::currentVersion, profileVersion.trimmed());
	}
	else
		copyDataToProfile();

	versionFile.open(QFile::WriteOnly);
	versionFile.write(Application::currentVersion.toUtf8());
	versionFile.close();
}

void ProfileManager::updateProfile(const QString& current, const QString& profile) const
{
	if (current == profile)
		return;

	Updater::Version prof{profile};

	if (prof > Updater::Version(Application::currentVersion)) {
		copyDataToProfile();
		return;
	}

	// Nothing yet
}

void ProfileManager::copyDataToProfile() const
{
	QDir profileDir{DataPaths::currentProfilePath()};
	QFile browseData{profileDir.filePath(QLatin1String("browsedata.db"))};

	if (browseData.exists()) {
		const QString browseDataBackup{Application::ensureUniqueFilename(profileDir.filePath(QLatin1String("browsedata-backup.db")))};

		browseData.copy(browseDataBackup);
		browseData.remove();

		QFile settings{profileDir.filePath("settings.ini")};
		if (settings.exists()) {
			const QString settingsBackup{Application::ensureUniqueFilename(profileDir.filePath("settings-backup.ini"))};

			settings.copy(settingsBackup);
			settings.remove();
		}

		QFile sessionFile{profileDir.filePath("session.dat")};
		if (sessionFile.exists()) {
			QString oldVersion{QString::fromUtf8(Application::readAllFileByteContents(profileDir.filePath("version"))).trimmed()};

			if (oldVersion.isEmpty())
				oldVersion = "unknown-version";

			const QString sessionBackup{Application::ensureUniqueFilename(profileDir.filePath(QStringLiteral("sessions/backup-%1.dat").arg(oldVersion)))};

			sessionFile.copy(sessionBackup);
			sessionFile.remove();
		}

		QMessageBox::warning(nullptr, QObject::tr("Sielo: Incompatible profile version"), QObject::tr("Incompatible profile version has been detected. To avoid losing your profile data, they were "
							 "backed up in following file:<br/><br/><b>%1<br/></b>").arg(browseDataBackup));
	}

	QFile(QLatin1String(":data/browsedata.db")).copy(profileDir.filePath(QLatin1String("browsedata.db")));
	QFile(profileDir.filePath(QLatin1String("browsedata.db"))).setPermissions(QFile::ReadUser | QFile::WriteUser);
}

void ProfileManager::connectDatabase()
{
	const QString dbFile{DataPaths::currentProfilePath() + QLatin1String("/browsedata.db")};

	if (m_databaseConnected)
		QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());

	QSqlDatabase db{QSqlDatabase::addDatabase(QLatin1String("QSQLITE"))};
	db.setDatabaseName(dbFile);

	if (!QFile::exists(dbFile)) {
		qWarning("Cannot find SQLite database file! Copying and using the defaults!");

		QFile(":data/browsedata.db").copy(dbFile);
		QFile(dbFile).setPermissions(QFile::ReadUser | QFile::WriteUser);
		db.setDatabaseName(dbFile);
	}

	if (Application::instance()->privateBrowsing())
		db.setConnectOptions("QSQLITE_OPEN_READONLY");

	if (!db.open())
		qWarning("Cannot open SQLite database! Continuing without database....");

	SqlDatabase::instance()->setDatabase(db);

	m_databaseConnected = true;
}
}
