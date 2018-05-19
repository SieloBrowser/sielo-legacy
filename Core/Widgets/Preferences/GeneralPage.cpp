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

#include "Widgets/Preferences/GeneralPage.hpp"

#include <QSettings>

#include <QMessageBox>

#include <QFileInfo>
#include <QDir>

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

	connect(m_radioNSOpenSaved, &QRadioButton::toggled, this, &GeneralPage::startupActionChanged);
	connect(m_btnSaveCurrentSession, &QPushButton::clicked, this, &GeneralPage::saveCurrentSession);

};

GeneralPage::~GeneralPage()
{
	// Empty
}

void GeneralPage::loadSettings()
{
	QSettings settings{};

	// Set new tab and new window option
	settings.beginGroup("Web-Settings");

	QUrl homePageUrl{settings.value(QLatin1String("homePage"), QUrl("https://doosearch.sielo.app/")).toUrl()};

	if (homePageUrl == QUrl("about:blank")) {
		m_radioHPBlank->setChecked(true);
		m_homePageUrl->setText("https://doosearch.sielo.app/");
		m_homePageUrl->setEnabled(false);
	}
	else {
		m_radioHPCustomUrl->setChecked(true);
		m_homePageUrl->setText(homePageUrl.toString());
	}

	QUrl newTabUrl{settings.value(QLatin1String("urlOnNewTab"), QUrl("https://doosearch.sielo.app/")).toUrl()};

	if (newTabUrl == QUrl("about:blank")) {
		m_radioNTOpenBlankPage->setChecked(true);
		m_newTabUrl->setText("https://www.google.com");
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

	m_dontLoadTabsUntilSelect->setChecked(settings.value(QLatin1String("LoadTabsOnActivation"), true).toBool());

	settings.endGroup();

	// Set what to do on new session
	settings.beginGroup("Settings");

	Application::AfterLaunch afterLaunch = static_cast<Application::AfterLaunch>(settings.value(
			QLatin1String("afterLaunch"), Application::OpenHomePage).toInt());
	if (afterLaunch == Application::OpenBlankPage)
		m_radioNSOpenBlankPage->setChecked(true);
	else if (afterLaunch == Application::OpenHomePage)
		m_radioNSOpenHomePage->setChecked(true);
	else if (afterLaunch == Application::RestoreSession)
		m_radioNSRestoreSession->setChecked(true);
	else if (afterLaunch == Application::OpenSavedSession)
		m_radioNSOpenSaved->setChecked(true);

	m_btnSaveCurrentSession->setEnabled(afterLaunch == Application::OpenSavedSession);

	settings.endGroup();

	// Set language
	settings.beginGroup("Language");

	QString activeLanguage = Application::instance()->currentLanguage();

	if (!activeLanguage.isEmpty() && activeLanguage != QLatin1String("en_US"))
		m_languages->addItem(createLanguageItem(activeLanguage), activeLanguage);

	m_languages->addItem("English (en_US)");

	const QString translationPath = Application::instance()->paths()[Application::P_Translations];
	QDir lanDir{translationPath};
	QStringList list = lanDir.entryList(QStringList("*.qm"));

			foreach (const QString& name, list) {
			if (name.startsWith(QLatin1String("qt_")))
				continue;

			QString loc{name};
			loc.remove(QLatin1String(".qm"));

			if (loc == activeLanguage)
				continue;

			m_languages->addItem(createLanguageItem(loc), loc);
		}

}

void GeneralPage::save()
{

	QSettings settings{};

	// Save what to do on new session
	settings.beginGroup("Settings");

	if (m_radioNSOpenBlankPage->isChecked())
		settings.setValue(QLatin1String("afterLaunch"), Application::OpenBlankPage);
	else if (m_radioNSOpenHomePage->isChecked())
		settings.setValue(QLatin1String("afterLaunch"), Application::OpenHomePage);
	else if (m_radioNSRestoreSession->isChecked())
		settings.setValue(QLatin1String("afterLaunch"), Application::RestoreSession);
	else
		settings.setValue(QLatin1String("afterLaunch"), Application::OpenSavedSession);

	settings.endGroup();

	// Save what to do on new tab and new window
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

	// Save if we want to load tabs at startup
	settings.setValue(QLatin1String("LoadTabsOnActivation"), m_dontLoadTabsUntilSelect->isChecked());

	settings.endGroup();

	// Save language
	settings.beginGroup("Language");

	settings.setValue("language", m_languages->itemData(m_languages->currentIndex()).toString());

	settings.endGroup();
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

void GeneralPage::startupActionChanged(bool isOpenSaved)
{
	m_btnSaveCurrentSession->setEnabled(isOpenSaved);

	if (!QFileInfo(Application::instance()->paths()[Application::P_Data] + QLatin1String("/home-session.dat")).exists()
		&& isOpenSaved) {
		saveCurrentSession();
	}
}

void GeneralPage::saveCurrentSession()
{
	Application::instance()->saveSession(true);

	QMessageBox::information(this, tr("Saved"), tr("Your session will be restored at startup"));
}

void GeneralPage::setupUI()
{
	m_layout = new QVBoxLayout(this);
	m_layoutGroupHomePage = new QVBoxLayout();
	m_layoutGroupNewTab = new QVBoxLayout();
	m_layoutGroupNewSession = new QVBoxLayout();
	m_layoutGroupLanguage = new QVBoxLayout();

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

	m_groupNewSession = new QGroupBox(tr("New Session"), this);

	m_radioNSOpenBlankPage = new QRadioButton(tr("Open blank page"), m_groupNewSession);
	m_radioNSOpenHomePage = new QRadioButton(tr("Open home page"), m_groupNewSession);
	m_radioNSRestoreSession = new QRadioButton(tr("Restore session"), m_groupNewSession);
	m_radioNSOpenSaved = new QRadioButton(tr("Open saved session"), m_groupNewSession);

	m_radioNSOpenHomePage->setChecked(true);

	m_btnSaveCurrentSession = new QPushButton(tr("Save current session"), m_groupNewSession);

	m_groupLanguage = new QGroupBox(tr("Language"), this);

	m_descLanguage = new QLabel(tr("Available translations:"), m_groupLanguage);
	m_languages = new QComboBox(m_groupLanguage);
	m_descLanguageNeedRestart = new QLabel(tr("In order to change language, you must restart your browser."),
										   m_groupLanguage);

	m_dontLoadTabsUntilSelect = new QCheckBox(tr("Don't load tabs until selected"), this);

	m_groupHomePage->setLayout(m_layoutGroupHomePage);

	m_layoutGroupHomePage->addWidget(m_radioHPBlank);
	m_layoutGroupHomePage->addWidget(m_radioHPCustomUrl);
	m_layoutGroupHomePage->addWidget(m_homePageUrl);

	m_groupNewTab->setLayout(m_layoutGroupNewTab);

	m_layoutGroupNewTab->addWidget(m_radioNTOpenBlankPage);
	m_layoutGroupNewTab->addWidget(m_radioNTOpenHomePage);
	m_layoutGroupNewTab->addWidget(m_radioNTOpenCutomUrl);
	m_layoutGroupNewTab->addWidget(m_newTabUrl);

	m_groupNewSession->setLayout(m_layoutGroupNewSession);

	m_layoutGroupNewSession->addWidget(m_radioNSOpenBlankPage);
	m_layoutGroupNewSession->addWidget(m_radioNSOpenHomePage);
	m_layoutGroupNewSession->addWidget(m_radioNSRestoreSession);
	m_layoutGroupNewSession->addWidget(m_radioNSOpenSaved);
	m_layoutGroupNewSession->addWidget(m_btnSaveCurrentSession);

	m_groupLanguage->setLayout(m_layoutGroupLanguage);

	m_layoutGroupLanguage->addWidget(m_descLanguage);
	m_layoutGroupLanguage->addWidget(m_languages);
	m_layoutGroupLanguage->addWidget(m_descLanguageNeedRestart);

	m_layout->addWidget(m_groupHomePage);
	m_layout->addWidget(m_groupNewTab);
	m_layout->addWidget(m_groupNewSession);
	m_layout->addWidget(m_groupLanguage);
	m_layout->addWidget(m_dontLoadTabsUntilSelect);
}

QString GeneralPage::createLanguageItem(const QString& lang)
{
	QLocale locale{lang};

	if (locale.language() == QLocale::C)
		return lang;

	QString country{QLocale::countryToString(locale.country())};
	QString language{QLocale::languageToString(locale.language())};

	return QString("%1, %2 (%3)").arg(language, country, lang);
}

}