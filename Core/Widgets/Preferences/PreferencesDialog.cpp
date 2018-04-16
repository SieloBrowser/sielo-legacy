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

	m_pages->setCurrentIndex(0);

	connect(m_buttonBox, &QDialogButtonBox::clicked, this, &PreferencesDialog::buttonClicked);
	connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(showStackedPage(QListWidgetItem*)));
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

void PreferencesDialog::showStackedPage(QListWidgetItem* item)
{
	if (!item)
		return;

	int index{m_list->currentRow()};
	m_caption->setText("<b>" + item->text() + "</b>");
	m_pages->setCurrentIndex(index);
}

void PreferencesDialog::addPage(QWidget* page, const QString& name)
{
	m_list->addItem(name);
	m_pages->addWidget(page);
}

void PreferencesDialog::setupUI()
{
	resize(860, 560);

	m_layout = new QGridLayout(this);

	m_list = new QListWidget(this);
	m_list->setObjectName(QLatin1String("preferences_list"));
	m_list->setIconSize(QSize(32, 32));
	m_list->setMaximumWidth(220);

	m_caption = new QLabel(this);
	m_caption->setObjectName(QLatin1String("preferences-title"));

	m_scrollArea = new QScrollArea(this);
	m_pages = new QStackedWidget(m_scrollArea);
	m_pages->setFocusPolicy(Qt::NoFocus);
	m_pages->setObjectName(QLatin1String("preferences-pages"));
	m_scrollArea->setObjectName(QLatin1String("preferences-pages"));
	m_scrollArea->setFrameShape(QFrame::NoFrame);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setWidget(m_pages);

	m_topLine = new QFrame(this);
	m_topLine->setObjectName(QLatin1String("preferences-line"));
	m_topLine->setFrameShape(QFrame::HLine);
	m_topLine->setFrameShadow(QFrame::Sunken);

	m_bottomLine = new QFrame(this);
	m_bottomLine->setObjectName(QLatin1String("preferences-line"));
	m_bottomLine->setFrameShape(QFrame::HLine);
	m_bottomLine->setFrameShadow(QFrame::Sunken);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);

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

	addPage(m_generalPage, tr("General"));
	addPage(m_appearancePage, tr("Appearance"));
	addPage(m_webConfigPage, tr("Web Configuration"));
	addPage(m_localStoragePage, tr("Local Storage"));
	addPage(m_proxyConfigPage, tr("Proxy"));
	addPage(m_passwordPage, tr("Password Manager"));
	addPage(m_privacyPage, tr("Privacy"));
	addPage(m_downloadPage, tr("Downloads"));
	addPage(m_pageAdBlock, tr("AdBlock"));
	addPage(m_currentTabsSpacePage, tr("Current Tabs Space"));

	m_layout->addWidget(m_list, 0, 1, 5, 1);
	m_layout->addWidget(m_caption, 0, 0, 1, 1);
	m_layout->addWidget(m_topLine, 1, 0, 1, 1);
	m_layout->addWidget(m_scrollArea, 2, 0, 1, 1);
	m_layout->addWidget(m_bottomLine, 3, 0, 1, 1);
	m_layout->addWidget(m_buttonBox, 4, 0, 1, 1);
}

}