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

#include <QtWidgets>

#include <QUrl>

#include <QWebEnginePage>
#include <QWebEngineSettings>

#include "Core/Application.hpp"

#include "Core/BrowserWindow.hpp"

#ifdef Q_OS_WIN
#include <d3d9.h>
#endif

int main(int argc, char** argv)
{
	qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9000");
	qputenv("QT_XCB_FORCE_SOFTWARE_OPENGL", "1");
	qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

#ifdef Q_OS_WIN
	// Dirty but quick fix for Old Intel HD Graphic Card
	if (QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS10) {
		IDirect3D9* ctx = Direct3DCreate9(D3D_SDK_VERSION);

		if (ctx != nullptr) {
			D3DADAPTER_IDENTIFIER9 id;
			if (ctx->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &id) == D3D_OK) {
				const QString DeviceDescription( id.Description );
				const QString FindedDeviceName = "Intel(R) HD Graphics";
				QString DeviceVersion = "";

				if (DeviceDescription.length() > FindedDeviceName.length()) {
					bool cool = true;
					int i = 0;
					while (i < FindedDeviceName.length()) {
						if (DeviceDescription[i] != FindedDeviceName[i]) cool = false;
						i++;
					}
					i++;
					for (int j = i; j < DeviceDescription.length(); j++) {
						DeviceVersion += DeviceDescription[j];
					}

					if (DeviceVersion.toInt() > 900 && DeviceVersion.toInt() < 6000) {
						QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
					}
				}
			}
		}
	}
#endif

	QString profilesPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/Sielo/profiles";
	QSettings profilesSettings{profilesPath + QLatin1String("/profiles.ini"), QSettings::IniFormat};

	QString startingProfile = profilesSettings.value(QLatin1String("Profiles/startProfile"), QLatin1String("default")).toString();
	QString currentProfilePath = profilesPath + "/" + startingProfile;
	QSettings settings{currentProfilePath + "/settings.ini", QSettings::IniFormat};

	std::string std1 = profilesPath.toStdString();
	std::string std2 = startingProfile.toStdString();
	std::string std3 = currentProfilePath.toStdString();

	if (settings.value("useSoftwareOpenGL", false).toBool())
		QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

	Sn::Application app(argc, argv);

	if (app.isClosing())
		return 0;

	return app.exec();
}
