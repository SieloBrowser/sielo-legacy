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

#include "Settings.hpp"

namespace Sn {
QSettings* Settings::s_settings = nullptr;

Settings::Settings()
{
	if (s_settings && !s_settings->group().isEmpty()) {
		m_openedGroup = s_settings->group();
		s_settings->endGroup();
	}
}

Settings::~Settings()
{
	if (!s_settings->group().isEmpty())
		s_settings->endGroup();

	if (!m_openedGroup.isEmpty())
		s_settings->beginGroup(m_openedGroup);
}

void Settings::createSettings(const QString& fileName)
{
	s_settings = new QSettings(fileName, QSettings::IniFormat);
}

void Settings::syncSettings()
{
	if (!s_settings)
		return;

	s_settings->sync();
}

bool Settings::contains(const QString& key)
{
	return s_settings->contains(key);
}

void Settings::remove(const QString& key)
{
	s_settings->remove(key);
}

void Settings::setValue(const QString& key, const QVariant& defaultValue)
{
	s_settings->setValue(key, defaultValue);
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue)
{
	return s_settings->value(key, defaultValue);
}

void Settings::beginGroup(const QString& prefix)
{
	s_settings->beginGroup(prefix);
}

void Settings::endGroup()
{
	s_settings->endGroup();
}

void Settings::sync()
{
	s_settings->sync();
}
}
