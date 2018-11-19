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
#ifndef SIELOBROWSER_SETTINGS_HPP
#define SIELOBROWSER_SETTINGS_HPP

#include "SharedDefines.hpp"

#include <QSettings>

namespace Sn
{
class SIELO_SHAREDLIB Settings {
public:
	Settings();
	~Settings();

	static void createSettings(const QString& fileName);
	static void syncSettings();

	bool contains(const QString& key);
	void remove(const QString& key);

	void setValue(const QString& key, const QVariant& defaultValue = QVariant());
	QVariant value(const QString& key, const QVariant& defaultValue = QVariant());

	void beginGroup(const QString& prefix);
	void endGroup();

	void sync();

private:
	static QSettings* s_settings;

	QString m_openedGroup{};
};
}

#endif //SIELOBROWSER_SETTINGS_HPP