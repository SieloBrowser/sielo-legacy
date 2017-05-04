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

#pragma once
#ifndef SIELOBROWSER_RESTOREMANAGER_HPP
#define SIELOBROWSER_RESTOREMANAGER_HPP

#include <QByteArray>

#include <QVector>

#include "Web/Tab/WebTab.hpp"

namespace Sn {
class WebPage;
class RecoveryJsObject;

class RestoreManager {
public:
	struct WindowData {
		int currentTab{};
		QByteArray windowState{};
		QVector<WebTab::SavedTab> tabsState;
	};

	RestoreManager();
	virtual ~RestoreManager();

	bool isValid() const;
	QVector<RestoreManager::WindowData> restoreData() const;

	QObject* recoveryObject(WebPage* page);

private:
	void createFromFile(const QString& file);

	RecoveryJsObject* m_recoveryObject{nullptr};
	QVector<RestoreManager::WindowData> m_data;
};

typedef QVector<RestoreManager::WindowData> RestoreData;

}
Q_DECLARE_TYPEINFO(Sn::RestoreManager::WindowData, Q_MOVABLE_TYPE);

#endif //SIELOBROWSER_RESTOREMANAGER_HPP
