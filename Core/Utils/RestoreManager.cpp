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

#include "Web/WebPage.hpp"

#include "Application.hpp"

namespace Sn {

RestoreManager::RestoreManager() :
	m_recoveryObject(new RecoveryJsObject(this))
{
	if (Application::instance()->afterLaunch() == Application::RestoreSession)
		createFromFile(Application::instance()->paths()[Application::P_Data] + QLatin1String("/session.dat"));
	else if (Application::instance()->afterLaunch() == Application::OpenSavedSession)
		createFromFile(Application::instance()->paths()[Application::P_Data] + QLatin1String("/home-session.dat"));

}

RestoreManager::~RestoreManager()
{
	delete m_recoveryObject;
}

bool RestoreManager::isValid() const
{
	return !m_data.isEmpty();
}

QVector<RestoreManager::WindowData> RestoreManager::restoreData() const
{
	return m_data;
}

QObject* RestoreManager::recoveryObject(WebPage* page)
{
	m_recoveryObject->setPage(page);
	return m_recoveryObject;
}

void RestoreManager::createFromFile(const QString& file)
{
	if (!QFile::exists(file))
		return;

	QFile recoveryFile{file};

	recoveryFile.open(QIODevice::ReadOnly);

	QDataStream stream{&recoveryFile};

	int version{0};
	stream >> version;

	if (version != 0x0001 && version != 0x0002)
		return;

	int windowCount{};
	stream >> windowCount;

	for (int win{0}; win < windowCount; ++win) {
		QByteArray tabWidgetState{};
		QByteArray windowState{};

		stream >> tabWidgetState;
		stream >> windowState;

		WindowData windowData{};

		windowData.windowState = windowState;

		QDataStream tabWidgetStream{tabWidgetState};

		if (tabWidgetStream.atEnd())
			continue;

		int mainSplitterCount{0};
		tabWidgetStream >> mainSplitterCount;

		windowData.spaceTabsCount.append(mainSplitterCount);

		for (int j{0}; j < mainSplitterCount; ++j) {
			int verticalSplitterCount{0};
			tabWidgetStream >> verticalSplitterCount;

			windowData.spaceTabsCount.append(verticalSplitterCount);
			for (int k{0}; k < verticalSplitterCount; ++k) {
				QVector<WebTab::SavedTab> tabs;
				QByteArray tabState{};
				QUrl homePageUrl{};
				int tabListCount{0};

				tabWidgetStream >> tabState;

				QDataStream tabStream{tabState};


				if (version != 0x0001)
					tabStream >> homePageUrl;

				windowData.homeUrls.append(homePageUrl);
				tabStream >> tabListCount;

				for (int l{0}; l < tabListCount; ++l) {
					WebTab::SavedTab tab{};

					tabStream >> tab;
					tabs.append(tab);
				}

				windowData.tabsState.append(tabs);

				int currentTab{};

				tabStream >> currentTab;

				windowData.currentTabs.append(currentTab);

			}
		}

		m_data.append(windowData);
	}
}

}