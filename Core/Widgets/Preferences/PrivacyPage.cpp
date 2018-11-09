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

#include "PrivacyPage.hpp"

#include "Cookies/CookieManager.hpp"

#include "Utils/Settings.hpp"

#include "Widgets/HTML5Permissions/HTML5PermissionsDialog.hpp"
#include "Widgets/Preferences/JSOptions.hpp"

namespace Sn {

PrivacyPage::PrivacyPage(QWidget* parent) :
	QWidget(parent)
{
	setupUI();
	loadSettings();

	connect(m_cookieManager, &QPushButton::clicked, this, &PrivacyPage::showCookieManager);
	connect(m_JSOptions, &QPushButton::clicked, this, &PrivacyPage::showJSOptions);
	connect(m_HTML5Permissions, &QPushButton::clicked, this, &PrivacyPage::showHTML5Permissions);
}

PrivacyPage::~PrivacyPage()
{
	// Empty
}

void PrivacyPage::loadSettings()
{
	Settings settings{};

	m_doNotTrack->setChecked(settings.value("Web-Settings/DoNotTrack", false).toBool());
}

void PrivacyPage::save()
{
	Settings settings{};

	settings.setValue("Web-Settings/DoNotTrack", m_doNotTrack->isChecked());
}

void PrivacyPage::showCookieManager()
{
	CookieManager* dialog{new CookieManager()};
	dialog->show();
}

void PrivacyPage::showJSOptions()
{
	JSOptions* dialog{new JSOptions(this)};
	dialog->show();
}

void PrivacyPage::showHTML5Permissions()
{
	HTML5PermissionsDialog* dialog{new HTML5PermissionsDialog(this)};
	dialog->show();
}

void PrivacyPage::setupUI()
{
	m_layout = new QGridLayout(this);

	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);

	m_labelCookies = new QLabel(tr("<b>Cookies</b>"), this);
	m_descCookies = new QLabel(tr("Manage Cookies"), this);
	m_cookieManager = new QPushButton(tr("Cookies Manager"), this);
	m_cookieManager->setSizePolicy(sizePolicy);

	m_labelJS = new QLabel(tr("<b>JavaScript</b>"), this);
	m_descJS = new QLabel(tr("Manage JavaScript privacy options"), this);
	m_JSOptions = new QPushButton(tr("JavaScript options"), this);
	m_JSOptions->setSizePolicy(sizePolicy);

	m_labelHTML5Permissions = new QLabel(tr("<b>HTML5 Permissions</b>"), this);
	m_descHTML5Permissions = new QLabel(tr("Manage HTML5 permissions"), this);
	m_HTML5Permissions = new QPushButton(tr("HTML5 Permissions"), this);
	m_HTML5Permissions->setSizePolicy(sizePolicy);

	m_labelOther = new QLabel(tr("<b>Other</b>"), this);
	m_doNotTrack = new QCheckBox(tr("Send Do Not Track header to servers"), this);

	m_cookiesSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_doNotTrackSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
	m_spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_layout->addWidget(m_labelCookies, 0, 0, 1, 3);
	m_layout->addItem(m_cookiesSpacer, 0, 3, 1, 1);
	m_layout->addWidget(m_descCookies, 1, 0, 1, 3);
	m_layout->addWidget(m_cookieManager, 1, 3, 1, 1);
	m_layout->addWidget(m_labelJS, 2, 0, 1, 4);
	m_layout->addWidget(m_descJS, 3, 0, 1, 3);
	m_layout->addWidget(m_JSOptions, 3, 3, 1, 1);
	m_layout->addWidget(m_labelHTML5Permissions, 4, 0, 1, 4);
	m_layout->addWidget(m_descHTML5Permissions, 5, 0, 1, 3);
	m_layout->addWidget(m_HTML5Permissions, 5, 3, 1, 1);
	m_layout->addWidget(m_labelOther, 6, 0, 1, 4);
	m_layout->addItem(m_doNotTrackSpacer, 7, 0, 1, 1);
	m_layout->addWidget(m_doNotTrack, 7, 1, 1, 3);
	m_layout->addItem(m_spacer, 8, 0, 1, 4);
}

}