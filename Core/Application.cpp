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

#include "History/HistoryManager.hpp"

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
	m_webProfile(nullptr)
{
	QCoreApplication::setOrganizationName(QLatin1String("Feldrise"));
	QCoreApplication::setApplicationName(QLatin1String("Sielo"));
	QCoreApplication::setApplicationVersion(QLatin1String("1.0.0"));
}

Application::~Application()
{
	delete m_historyManager;
}

HistoryManager* Application::historyManager()
{
	if (!m_historyManager)
		m_historyManager = new HistoryManager();

	return m_historyManager;
}
}
