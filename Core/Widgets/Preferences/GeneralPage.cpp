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

#include "Widgets/Preferences/GeneralPage.hpp"

#include <QSettings>

#include <QMessageBox>
#include <QtCore/QFileInfo>

#include "Application.hpp"

#include "BrowserWindow.hpp"

namespace Sn {

GeneralPage::GeneralPage(QWidget* parent) :
	QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setupUI();

	loadSettings();

	connect(m_radioHPBlank, &QRadioButton::toggled, this, &GeneralPage::homePageActionChanged);

	connect(m_radioNTOpenCutomUrl, &QRadioButton::toggled, this, &GeneralPage::newTabActionChanged);

	connect(m_btnSaveCurrentSession, &QPushButton::clicked, this, &GeneralPage::saveCurrentSession);
	connect(m_comboActionOnNewSession, SIGNAL(currentIndexChanged(
												  const QString&)), this, SLOT(startupActionChanged(
																				   const QString&)));

	connect(m_useRealToolBar, &QCheckBox::toggled, this, &GeneralPage::useRealToolBarChanged);
};

GeneralPage::~GeneralPage()
{
	// Empty
}

void GeneralPage::loadSettings()
{
	QSettings settings{};

	settings.beginGroup("Settings");

	m_comboActionOnNewSession
		->setCurrentIndex(settings.value(QLatin1String("afterLaunch"), Application::OpenHomePage).toInt());

	m_btnSaveCurrentSession->setEnabled(settings.value(QLatin1String("afterLaunch"), Application::OpenHomePage).toInt()
										== Application::OpenSavedSession);

	m_useRealToolBar
		->setChecked(settings.value(QLatin1String("useTopToolBar"), Application::instance()->useTopToolBar()).toBool());

	m_floatingButtonFoloweMouse->setChecked(settings.value(QLatin1String("floatingButtonFolowMouse"),
														   Application::instance()->floatingButtonFoloweMouse())
												.toBool());
	m_floatingButtonFoloweMouse->setEnabled(!m_useRealToolBar->isChecked());

	settings.endGroup();

	settings.beginGroup("Web-Settings");

	QUrl homePageUrl{settings.value(QLatin1String("homePage"), QUrl("https://www.ecosia.org")).toUrl()};

	if (homePageUrl == QUrl("about:blank")) {
		m_radioHPBlank->setChecked(true);
		m_homePageUrl->setText("https://www.ecosia.org");
		m_homePageUrl->setEnabled(false);
	}
	else {
		m_radioHPCustomUrl->setChecked(true);
		m_homePageUrl->setText(homePageUrl.toString());
	}

	QUrl newTabUrl{settings.value(QLatin1String("urlOnNewTab"), QUrl("https://www.ecosia.org")).toUrl()};

	if (newTabUrl == QUrl("about:blank")) {
		m_radioNTOpenBlankPage->setChecked(true);
		m_newTabUrl->setText("https://www.ecosia.org");
		m_newTabUrl->setEnabled(false);
	}
	else if (newTabUrl == homePageUrl) {
		m_radioNTOpenHomePage->setChecked(true);
		m_newTabUrl->setText(homePageUrl.toString());
		m_newTabUrl->setEnabled(false);
	}
	else {
		m_radioNTOpenCutomUrl->setChecked(true);
		m_newTabUrl->setText(newTabUrl.toString());
	}

	settings.endGroup();
}

void GeneralPage::save()
{

	QSettings settings{};

	settings.beginGroup("Settings");

	settings.setValue(QLatin1String("afterLaunch"), m_comboActionOnNewSession->currentIndex());
	settings.setValue(QLatin1String("useTopToolBar"), m_useRealToolBar->isChecked());

	settings.setValue(QLatin1String("floatingButtonFoloweMouse"),
					  m_useRealToolBar->isChecked() ? false : m_floatingButtonFoloweMouse->isChecked());

	if (m_useRealToolBar->isChecked() != Application::instance()->useTopToolBar()) {
		QMessageBox::warning(this, tr("Warning"), tr("Some changes need Sielo restart to have effects"));
	}

	settings.endGroup();

	settings.beginGroup("Web-Settings");

	settings.setValue(QLatin1String("homePage"),
					  m_radioHPBlank->isChecked() ? QUrl("about:blank") : QUrl(m_homePageUrl->text()));

	if (m_radioNTOpenBlankPage->isChecked()) {
		settings.setValue(QLatin1String("urlOnNewTab"), QUrl("about:blank"));
	}
	else if (m_radioNTOpenHomePage->isChecked()) {
		settings.setValue(QLatin1String("urlOnNewTab"),
						  m_radioHPBlank->isChecked() ? QUrl("about:blank") : QUrl(m_homePageUrl->text()));
	}
	else {
		settings.setValue(QLatin1String("urlOnNewTab"), QUrl(m_newTabUrl->text()));
	}

	Application::instance()->loadSettings();

}

void GeneralPage::homePageActionChanged(bool enabled)
{
	if (enabled)
		m_homePageUrl->setEnabled(false);
	else
		m_homePageUrl->setEnabled(true);
}

void GeneralPage::newTabActionChanged(bool enabled)
{
	if (enabled)
		m_newTabUrl->setEnabled(true);
	else
		m_newTabUrl->setEnabled(false);
}

void GeneralPage::startupActionChanged(const QString& currentIndex)
{
	m_btnSaveCurrentSession->setEnabled(currentIndex == tr("Open saved session"));

	if (!QFileInfo(Application::instance()->paths()[Application::P_Data] + QLatin1String("/home-session.dat")).exists()
		&& currentIndex == tr("Open saved session")) {
		saveCurrentSession();
	}
}

void GeneralPage::saveCurrentSession()
{
	Application::instance()->saveSession(true);

	QMessageBox::information(this, tr("Saved"), tr("Your session will be restored at startup"));
}

void GeneralPage::useRealToolBarChanged(bool enabled)
{
	m_floatingButtonFoloweMouse->setEnabled(!m_useRealToolBar->isChecked());
}

void GeneralPage::setupUI()
{
	m_layout = new QVBoxLayout(this);
	m_layoutGroup1 = new QVBoxLayout();
	m_layoutGroup2 = new QVBoxLayout();

	m_groupHomePage = new QGroupBox(tr("Home page"), this);

	m_radioHPBlank = new QRadioButton(tr("Open blank page"), m_groupHomePage);
	m_radioHPCustomUrl = new QRadioButton(tr("Open custom url"), m_groupHomePage);

	m_radioHPCustomUrl->setChecked(true);

	m_homePageUrl = new QLineEdit(m_groupHomePage);
	m_homePageUrl->setPlaceholderText(tr("Home page url"));

	m_groupNewTab = new QGroupBox(tr("New tab"), this);

	m_radioNTOpenBlankPage = new QRadioButton(tr("Open blank page"), m_groupNewTab);
	m_radioNTOpenHomePage = new QRadioButton(tr("Open home page"), m_groupNewTab);
	m_radioNTOpenCutomUrl = new QRadioButton(tr("Open custom url"), m_groupNewTab);

	m_radioNTOpenHomePage->setChecked(true);

	m_newTabUrl = new QLineEdit(m_groupNewTab);
	m_newTabUrl->setPlaceholderText(tr("New tab url"));

	m_descActionOnNewSession = new QLabel(tr("Action to do on new session"), this);

	m_btnSaveCurrentSession = new QPushButton(tr("Save current session"), this);

	m_comboActionOnNewSession = new QComboBox(this);
	m_comboActionOnNewSession->clear();
	m_comboActionOnNewSession->addItems(QStringList() << tr("Open blank page")
													  << tr("Open home page")
													  << tr("Restore session")
													  << tr("Open saved session"));

	m_useRealToolBar = new QCheckBox(tr("Use real toolbar instead of floating button"), this);
	m_floatingButtonFoloweMouse = new QCheckBox(tr("Floating button automatically move to focused tabs space"));

	m_groupHomePage->setLayout(m_layoutGroup1);

	m_layoutGroup1->addWidget(m_radioHPBlank);
	m_layoutGroup1->addWidget(m_radioHPCustomUrl);
	m_layoutGroup1->addWidget(m_homePageUrl);

	m_groupNewTab->setLayout(m_layoutGroup2);

	m_layoutGroup2->addWidget(m_radioNTOpenBlankPage);
	m_layoutGroup2->addWidget(m_radioNTOpenHomePage);
	m_layoutGroup2->addWidget(m_radioNTOpenCutomUrl);
	m_layoutGroup2->addWidget(m_newTabUrl);

	m_layout->addWidget(m_groupHomePage);
	m_layout->addWidget(m_groupNewTab);
	m_layout->addWidget(m_descActionOnNewSession);
	m_layout->addWidget(m_btnSaveCurrentSession);
	m_layout->addWidget(m_comboActionOnNewSession);
	m_layout->addWidget(m_useRealToolBar);
	m_layout->addWidget(m_floatingButtonFoloweMouse);
}

}