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

#include "Utils/Updater.hpp"

#include <QTimer>

#include <QMessageBox>

#include <QFile>
#include <QStandardPaths>
#include <QDesktopServices>

#include <QRegExp>
#include <QRegularExpression>

#include <QMessageBox>

#include <QNetworkRequest>
#include <QNetworkReply>

#include "Network/NetworkManager.hpp"

#include "Widgets/UpdaterDialog.hpp"

#include "BrowserWindow.hpp"

#include "Application.hpp"

namespace Sn
{

Updater::Version::Version(const QString& string)
{
	// Heavier than Regex but it work :P
	QString cleanVersion = "";
	const QString requiredValues = "0123456789.";
	for (const auto &e : string) {
		for (const auto &f : requiredValues) {
			if (e == f) cleanVersion += e;
		}
	}

	//QString cleanVersion{QString(string).remove(QRegExp("/[^\d.]/g"))};

	isValid = false;

	QStringList v = cleanVersion.split(QLatin1Char('.'));

	if (v.count() != 3)
		return;

	bool ok{false};

	majorVersion = v[0].toInt(&ok);
	if (!ok)
		return;

	minorVersion = v[1].toInt(&ok);
	if (!ok)
		return;

	revisionNumber = v[2].toInt(&ok);
	if (!ok)
		return;

	isValid = majorVersion >= 0 && minorVersion >= 0 && revisionNumber >= 0;
}

bool Updater::Version::operator <(const Updater::Version &other) const
{
	if (this->majorVersion != other.majorVersion) 
		return this->majorVersion < other.majorVersion;
	if (this->minorVersion != other.minorVersion) 
		return this->minorVersion < other.minorVersion;
	if (this->revisionNumber != other.revisionNumber) 
		return this->revisionNumber < other.revisionNumber;

	return false;
}

bool Updater::Version::operator >(const Updater::Version &other) const
{
	if (*this == other) 
		return false;
	
	return !operator<(other);
}

bool Updater::Version::operator ==(const Updater::Version &other) const
{
	return (this->majorVersion == other.majorVersion &&
			this->minorVersion == other.minorVersion &&
			this->revisionNumber == other.revisionNumber);
}

bool Updater::Version::operator >=(const Updater::Version &other) const
{
	if (*this == other) 
		return true;
	
	return *this > other;
}

bool Updater::Version::operator <=(const Updater::Version &other) const
{
	if (*this == other) 
		return true;
	
	return *this < other;
}

QString Updater::Version::versionString() const
{
	return QString("%1.%2.%3").arg(majorVersion, minorVersion, revisionNumber);
}


Updater::Updater(BrowserWindow* window, QObject* parent) :
	QObject(parent),
	m_window(window)
{
	QTimer::singleShot(10 * 1000, this, &Updater::start);
}

void Updater::downloadUpdateInfoCompleted()
{
	if (!m_versionReply)
		return;

	QByteArray updateInfo = m_versionReply->readAll();
	QTextStream in{&updateInfo};
	QString newVersion{};

	if (m_versionReply->error() != QNetworkReply::NoError)
		return;

	while (!in.atEnd()) {
		QString line{in.readLine()};
		QStringList versionInfo{line.split(QLatin1Char(','))};

		if (newVersion.isEmpty())
			newVersion = versionInfo[0];

		if (versionInfo[0] == Application::currentVersion)
			break;

		for (int i{1}; i < versionInfo.count(); ++i) {
			if (versionInfo[i] == "fullUpdate")
				m_fullUpdate = true;
			if (versionInfo[i] == "themeUpdate")
				m_themeUpdate = true;
		}
	}
	
	m_versionReply->deleteLater();

	Version current{Application::currentVersion};
	Version newVers{newVersion};

	if (!newVers.isValid)
		return;

	if (newVers > current) {
#if defined(Q_OS_WIN)
		if (!Application::instance()->isPortable()) {
			QString updaterName{};
			QString arch{};

			if (Application::instance()->is32bit())
				arch = "_x86";

			if (m_fullUpdate)
				updaterName = "sielo_full_update_setup" + arch + ".exe";
			else if (m_themeUpdate)
				updaterName = "sielo_theme_update_setup" + arch + ".exe";
			else
				updaterName = "sielo_update_setup" + arch + ".exe";

			QUrl updaterUrl{QUrl("http://www.feldrise.com/Sielo/" + updaterName)};
			startDownloadNewVersion(updaterUrl);
		}
		else {
#endif
			if (!Application::currentVersion.contains("closed-beta"))
				QMessageBox::information(m_window,
										 tr("Update"),
										 tr("A new version of Sielo is available (%1)! We advise you to download it.")
										 .arg(newVersion));
#if defined(Q_OS_WIN)
		}
#endif
	}
}

void Updater::downloadCompleted()
{
	QString updaterLocation{QStandardPaths::writableLocation(QStandardPaths::TempLocation)};
	QFile updater{updaterLocation + QLatin1String("/SieloUpdater.exe")};

	if (!updater.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(m_window, tr("Update fail"), tr("Impossible to update Sielo... Please retry later"));
		return;
	}

	updater.write(m_updaterReply->readAll());
	updater.close();

	QDesktopServices::openUrl(QUrl(updaterLocation + QLatin1String("/SieloUpdater.exe")));

	m_updaterReply->deleteLater();
	m_window->close();
}

void Updater::start()
{
	QUrl newVersionUrl{QUrl("http://www.feldrise.com/Sielo/versions.txt")};

	startDownloadingUpdateInfo(newVersionUrl);
}

void Updater::startDownloadNewVersion(const QUrl& url)
{
	m_updaterReply = Application::instance()->networkManager()->get(QNetworkRequest(url));
	UpdaterDialog* dialog{new UpdaterDialog(m_window)};

	connect(m_updaterReply, &QNetworkReply::finished, dialog, &UpdaterDialog::downloadCompleted);
	connect(m_updaterReply, &QNetworkReply::downloadProgress, dialog, &UpdaterDialog::downloadProgress);
	connect(m_updaterReply, &QNetworkReply::finished, this, &Updater::downloadCompleted);
}

void Updater::startDownloadingUpdateInfo(const QUrl& url)
{
	m_versionReply = Application::instance()->networkManager()->get(QNetworkRequest(url));

	connect(m_versionReply, &QNetworkReply::finished, this, &Updater::downloadUpdateInfoCompleted);
}

}
