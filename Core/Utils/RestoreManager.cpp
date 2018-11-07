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

#include "Utils/RestoreManager.hpp"


#include "Utils/RecoveryJsObject.hpp"
#include "Utils/DataPaths.hpp"

#include "Web/WebPage.hpp"

#include "Application.hpp"

namespace Sn {
// Static
bool RestoreManager::validateFile(const QString& file)
{
	RestoreData data{};
	createFromFile(file, data);

	return data.isValid();
}

void RestoreManager::createFromFile(const QString& file, RestoreData& data)
{
	if (!QFile::exists(file))
		return;

	QFile recoveryFile{file};

	if (!recoveryFile.open(QIODevice::ReadOnly))
		return;

	QDataStream stream{&recoveryFile};

	int version{0};
	stream >> version;

	if (version > 1)
		return;

	stream >> data;
}

bool RestoreData::isValid() const
{
	for (const BrowserWindow::SavedWindow& window : windows) {
		if (!window.isValid())
			return false;
	}

	return !windows.isEmpty();
}

void RestoreData::clear()
{
	windows.clear();
	crashedSession.clear();
}

QDataStream &operator<<(QDataStream &stream, const RestoreData &data)
{
	stream << 1;
	stream << data.windows.count();

	for (const BrowserWindow::SavedWindow& window : data.windows)
		stream << window;

	stream << data.crashedSession;

	return stream;
}

QDataStream &operator>>(QDataStream &stream, RestoreData &data)
{
	int version{0};
	stream >> version;

	if (version < 1)
		return stream;

	int windowCount{0};
	stream >> windowCount;
	data.windows.reserve(windowCount);

	for (int i{0}; i < windowCount; ++i) {
		BrowserWindow::SavedWindow window{};
		stream >> window;

		data.windows.append(window);
	}

	stream >> data.crashedSession;

	return stream;
}

RestoreManager::RestoreManager(bool openSavedSession) :
	m_recoveryObject(new RecoveryJsObject(this))
{
	if (openSavedSession)
		createFromFile(DataPaths::currentProfilePath() + QLatin1String("/home-session.dat")); 
	else 
		createFromFile(DataPaths::currentProfilePath() + QLatin1String("/session.dat"));

}

RestoreManager::~RestoreManager()
{
	delete m_recoveryObject;
}

bool RestoreManager::isValid() const
{
	return m_data.isValid();
}

RestoreData RestoreManager::restoreData() const
{
	return m_data;
}

void RestoreManager::clearRestoreData()
{
	m_data.clear();

	QDataStream stream(&m_data.crashedSession, QIODevice::ReadOnly);
	stream >> m_data;
}

QObject* RestoreManager::recoveryObject(WebPage* page)
{
	m_recoveryObject->setPage(page);
	return m_recoveryObject;
}

void RestoreManager::createFromFile(const QString& file)
{
	createFromFile(file, m_data);
}
}
