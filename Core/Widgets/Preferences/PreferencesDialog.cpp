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

#include "Widgets/Preferences/PreferencesDialog.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "Widgets/Preferences/GeneralPage.hpp"
#include "Widgets/Preferences/Appearance.hpp"
#include "Widgets/Preferences/WebConfigPage.hpp"
#include "Widgets/Preferences/LocalStoragePage.hpp"
#include "Widgets/Preferences/ProxyConfigPage.hpp"
#include "Widgets/Preferences/PasswordPage.hpp"
#include "Widgets/Preferences/PrivacyPage.hpp"
#include "Widgets/Preferences/DownloadPage.hpp"
#include "Widgets/Preferences/AdBlockPage.hpp"
#include "Widgets/Preferences/CurrentTabsSpacePage.hpp"

namespace Sn {

PreferencesDialog::PreferencesDialog(TabWidget* tabWidget, QWidget* parent) :
	QDialog(parent),
	m_tabWidget(tabWidget)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName("preferences_dialog");

	setupUI();

	connect(m_buttonBox, &QDialogButtonBox::clicked, this, &PreferencesDialog::buttonClicked);
}

PreferencesDialog::~PreferencesDialog()
{
	// Empty
}

void PreferencesDialog::saveSettings()
{
	m_generalPage->save();
	m_appearancePage->save();
	m_webConfigPage->save();
	m_localStoragePage->save();
	m_proxyConfigPage->save();
	m_passwordPage->save();
	m_privacyPage->save();
	m_downloadPage->save();
	m_currentTabsSpacePage->save();

	Application::instance()->loadSettings();
}

void PreferencesDialog::buttonClicked(QAbstractButton* button)
{
	switch (m_buttonBox->buttonRole(button)) {
	case QDialogButtonBox::ApplyRole:
		saveSettings();
		break;
	case QDialogButtonBox::AcceptRole:
		saveSettings();
		close();
		break;
	case QDialogButtonBox::RejectRole:
		close();
		break;
	default:
		break;
	}
}

void PreferencesDialog::setupUI()
{
	m_layout = new QVBoxLayout(this);

	m_layoutButton = new QHBoxLayout();

	m_pages = new QTabWidget(this);
	m_pages->setObjectName("preferences_pages");

	m_generalPage = new GeneralPage(m_pages);
	m_downloadPage = new DownloadPage(m_pages);
	m_webConfigPage = new WebConfigPage(m_pages);
	m_localStoragePage = new LocalStoragePage(m_pages);
	m_proxyConfigPage = new ProxyConfigPage(m_pages);
	m_passwordPage = new PasswordPage(m_pages);
	m_privacyPage = new PrivacyPage(m_pages);
	m_appearancePage = new AppearancePage(m_pages);
	m_pageAdBlock = new AdBlockPage(m_pages);
	m_currentTabsSpacePage = new CurrentTabsSpacePage(m_tabWidget, m_pages);

	m_pages->addTab(m_generalPage, tr("General"));
	m_pages->addTab(m_appearancePage, tr("Appearance"));
	m_pages->addTab(m_webConfigPage, tr("Web Configuration"));
	m_pages->addTab(m_localStoragePage, tr("Local Storage"));
	m_pages->addTab(m_proxyConfigPage, tr("Proxy"));
	m_pages->addTab(m_passwordPage, tr("Password Manager"));
	m_pages->addTab(m_privacyPage, tr("Privacy"));
	m_pages->addTab(m_downloadPage, tr("Downloads"));
	m_pages->addTab(m_pageAdBlock, tr("AdBlock"));
	m_pages->addTab(m_currentTabsSpacePage, tr("Current Tabs Space"));

	m_buttonSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);

	m_layoutButton->addItem(m_buttonSpacer);
	m_layoutButton->addWidget(m_buttonBox);

	m_layout->addWidget(m_pages);
	m_layout->addLayout(m_layoutButton);
}

}