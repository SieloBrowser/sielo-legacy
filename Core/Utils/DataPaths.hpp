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
#ifndef SIELOBROWSER_DATAPATHS_HPP
#define SIELOBROWSER_DATAPATHS_HPP

#include <QStringList>
#include <array>

namespace Sn
{
class DataPaths {
public:
	enum Path {
		AppData = 0,             // /usr/share/sielo or . or ../Resources
		Translations = 1,        // $AppData/locale
		Themes = 2,              // $AppData/themes
		Plugins = 3,             // $AppData/plugins
		Config = 4,              // $XDG_CONFIG_HOME/sielo or %LOCALAPPDATA%/sielo or $AppData/data (portable)
		Profiles = 5,            // $Config/profiles
		CurrentProfile = 6,      // $Profiles/current_profile
		Temp = 7,                // $Config/tmp
		Cache = 8,               // $XDG_CACHE_HOME/sielo or $CurrentProfile/cache
		Sessions = 9,            // $CurrentProfile/sessions
		LastPath = 10
	};

	DataPaths();

	static void setCurrentProfilePath(const QString& profilePath);
	static void setPortableVersion();

	static QString path(Path type);
	static QStringList allPaths(Path type);
	static QString currentProfilePath();

	static void clearTempData();

private:
	void initCurrentProfile(const QString& profilePath);

	QStringList m_paths[LastPath];
};
}

#endif //SIELOBROWSER_DATAPATHS_HPP