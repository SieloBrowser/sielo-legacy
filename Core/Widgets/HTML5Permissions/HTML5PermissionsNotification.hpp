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
#ifndef SIELOBROWSER_HTML5PERMISSIONSNOTIFICATION_HPP
#define SIELOBROWSER_HTML5PERMISSIONSNOTIFICATION_HPP

#include "SharedDefines.hpp"

#include <QHBoxLayout>

#include <QWidget>

#include <QWebEngine/WebPage.hpp>

#include <QLabel>
#include <QCheckBox>
#include <QPushButton>

#include <QSpacerItem>

namespace Sn {
class WebPage;

class SIELO_SHAREDLIB HTML5PermissionsNotification: public QWidget {
Q_OBJECT

public:
	HTML5PermissionsNotification(const QUrl& origin, Engine::WebPage* page, const Engine::WebPage::Feature& feature);
	~HTML5PermissionsNotification();

private slots:
	void grantPermissions();
	void denyPermissions();

private:
	void setupUi();

	QHBoxLayout* m_layout{nullptr};
	QLabel* m_iconLabel{nullptr};
	QLabel* m_textLabel{nullptr};
	QSpacerItem* m_spacer{nullptr};
	QCheckBox* m_remember{nullptr};
	QPushButton* m_allowButton{nullptr};
	QPushButton* m_denyButton{nullptr};

	QUrl m_origin{};
	Engine::WebPage* m_page{nullptr};
	Engine::WebPage::Feature m_feature;

};
}

#endif //SIELOBROWSER_HTML5PERMISSIONSNOTIFICATION_HPP
