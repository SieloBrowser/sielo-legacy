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
#ifndef SIELOBROWSER_PROFILEMANAGER_HPP
#define SIELOBROWSER_PROFILEMANAGER_HPP

#include "SharedDefines.hpp"

#include <QString>

namespace Sn
{
class SIELO_SHAREDLIB ProfileManager {
public:
	ProfileManager();
	~ProfileManager() = default;

	static void initConfigDir();
	void initCurrentProfile(const QString& profileName);

	static int createProfile(const QString& profileName); // Return 0 on success, -1 profile already exists, -2 cannot create directory
	static bool removeProfile(const QString& profileName);

	static QString currentProfile();
	static QString startingProfile();
	static void setStartingProfile(const QString& profileName);

	static QStringList availableProfiles();

private:
	void updateCurrentProfile() const;
	void updateProfile(const QString& current, const QString& profile) const;
	void copyDataToProfile() const;

	void connectDatabase();

	bool m_databaseConnected{false};
};
}

#endif //SIELOBROWSER_PROFILEMANAGER_HPP