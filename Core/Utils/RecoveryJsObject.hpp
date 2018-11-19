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
#ifndef SIELOBROWSER_RECOVERYJSOBJECT_HPP
#define SIELOBROWSER_RECOVERYJSOBJECT_HPP

#include "SharedDefines.hpp"

#include <QObject>

#include <QJsonArray>

namespace Sn {
class WebPage;
class BrowserWindow;
class RestoreManager;

class SIELO_SHAREDLIB RecoveryJsObject: public QObject {
Q_OBJECT
	Q_PROPERTY(QJsonArray restoreData
				   READ
				   restoreData
				   CONSTANT)

public:
	RecoveryJsObject(RestoreManager* manager);

	void setPage(WebPage* page);

	QJsonArray restoreData() const;

public slots:
	void startNewSession();
	void restoreSession(const QStringList& excludeWindows, const QStringList& excluedTabs);

private:
	BrowserWindow* getBrowserWindow() const;

	RestoreManager* m_manager{nullptr};
	WebPage* m_page{nullptr};
};
}

#endif //SIELOBROWSER_RECOVERYJSOBJECT_HPP
