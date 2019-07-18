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
#ifndef SIELOBROWSER_HTML5PERMISSIONSMANAGER_HPP
#define SIELOBROWSER_HTML5PERMISSIONSMANAGER_HPP

#include "SharedDefines.hpp"

#include <QObject>

#include <QHash>

#include <QStringList>
#include <QUrl>

#include <QWebEngine/WebPage.hpp>

namespace Sn {
class WebPage;

class SIELO_SHAREDLIB HTML5PermissionsManager: public QObject {
public:
	HTML5PermissionsManager(QObject* parent);

	void requestPermissions(WebPage* page, const QUrl& origin, const Engine::WebPage::Feature& feature);
	void rememberPermissions(const QUrl& origin, const Engine::WebPage::Feature& feature,
							 const Engine::WebPage::PermissionPolicy& policy);

	void loadSettings();

private:
	void saveSettings();

	QHash<Engine::WebPage::Feature, QStringList> m_granted;
	QHash<Engine::WebPage::Feature, QStringList> m_denied;
};

}

#endif //SIELOBROWSER_HTML5PERMISSIONSMANAGER_HPP
