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

#include "Widgets/Preferences/Appearance.hpp"

#include <QSettings>

#include <QProcess>
#include <QTimer>

#include <QMessageBox>
#include <QFileDialog>

#include <QDir>

#include "Application.hpp"

#include "Utils/RegExp.hpp"

namespace Sn {

AppearancePage::AppearancePage(QWidget* parent) :
	QWidget(parent)
{
	setupUI();
	loadSettings();

	connect(m_themeList, &QListWidget::currentItemChanged, this, &AppearancePage::currentChanged);
	connect(m_addThemeButton, &QPushButton::clicked, this, &AppearancePage::addTheme);
	connect(m_viewGalleryButton, &QPushButton::clicked, this, &AppearancePage::openGallery);
	connect(m_useRealToolBar, &QCheckBox::toggled, this, &AppearancePage::useRealToolBarChanged);

	currentChanged();
}

AppearancePage::~AppearancePage()
{
	delete m_nameLayout;
}

void AppearancePage::save()
{
	QSettings settings;

	settings.beginGroup("Settings");

	settings.setValue(QLatin1String("useTopToolBar"), m_useRealToolBar->isChecked());
	settings.setValue(QLatin1String("floatingButtonFoloweMouse"),
					  m_useRealToolBar->isChecked() ? false : m_floatingButtonFoloweMouse->isChecked());
	settings
		.setValue(QLatin1String("hideBookmarksHistoryByDefault"), m_hideBookmarksHistoryActionsByDefault->isChecked());

	if (m_useRealToolBar->isChecked() != Application::instance()->useTopToolBar()) {
		QMessageBox::warning(this, tr("Warning"), tr("Some changes need Sielo restart to have effects"));
	}

	settings.endGroup();

	QListWidgetItem* currentItem = m_themeList->currentItem();

	if (m_activeTheme == currentItem->data(Qt::UserRole).toString())
		return;

	m_activeTheme = currentItem->data(Qt::UserRole).toString();

	settings.setValue("Themes/currentTheme", m_activeTheme);

	Application::instance()->loadTheme(m_activeTheme);

}

void AppearancePage::currentChanged()
{
	QListWidgetItem* currentItem = m_themeList->currentItem();

	if (!currentItem)
		return;

	Theme currentTheme = m_themeHash[currentItem->data(Qt::UserRole).toString()];

	m_name->setText(currentTheme.name);
	m_author->setText(currentTheme.author);
	m_desc->setText(currentTheme.longDesc);
	m_licenseBtn->setHidden(currentTheme.license.isEmpty());
}

void AppearancePage::showLicense()
{
	// Empty
}

void AppearancePage::openGallery()
{
	QMessageBox::critical(this, tr("Unavailable"), tr("The gallery is currently unavailable."));
}

void AppearancePage::addTheme()
{
#if defined(Q_OS_WIN)
	QString compilerName = "sielo-compiler.exe";
#elif defined(Q_OS_LINUX)
	QString compilerName = "sielo-compiler";
#endif
	if (!QFile(QDir(QCoreApplication::applicationDirPath()).absolutePath() + QLatin1Char('/') + compilerName)
		.exists()) {
		QMessageBox::critical(this,
							  tr("Error"),
							  tr("Can't decompile theme... Be sure compiler is with Sielo main exe, else move it manually or update/reinstall the browser."));
		return;
	}

	QString themeFile{QFileDialog::getOpenFileName(this, tr("Open a theme"), QString(), "Themes (*.snthm)")};

	if (themeFile.isEmpty())
		return;

	QFileInfo themeInfo{Application::paths()[Application::P_Themes] + QLatin1Char('/')
						+ QFileInfo(themeFile).baseName()
						+ QLatin1String("/main.sss")};

	if (themeInfo.exists()) {
		QMessageBox::warning(this,
							 tr("Theme exist"),
							 tr("The theme already exist and is going to be update with the new version."));
		QDir(Application::paths()[Application::P_Themes] + QLatin1Char('/') + QFileInfo(themeFile).baseName())
			.removeRecursively();
	}

	QStringList decompileArgs{};
	decompileArgs << "decompile" << themeFile
				  << Application::instance()->paths()[Application::P_Themes] + QLatin1Char('/')
					 + QFileInfo(themeFile).baseName() << "Theme successfully decompiled";

	QProcess::execute(QDir(QCoreApplication::applicationDirPath()).absolutePath() + QLatin1Char('/') + compilerName,
					  decompileArgs);

	QTimer::singleShot(500, this, &AppearancePage::loadSettings);

}

void AppearancePage::useRealToolBarChanged(bool enabled)
{
	m_floatingButtonFoloweMouse->setEnabled(!m_useRealToolBar->isChecked());
	m_hideBookmarksHistoryActionsByDefault->setEnabled(m_useRealToolBar->isChecked());
}

AppearancePage::Theme AppearancePage::parseTheme(const QString& path, const QString& name)
{
	Theme info{};

	if (!QFile(path + "main.sss").exists() || !QFile(path + "theme.info").exists()) {
		info.isValid = false;
		return info;
	}

	if (QFile(path + "theme.png").exists())
		info.icon = QIcon(path + "theme.png");
	else
		info.icon = Application::getAppIcon("webpage");

	if (QFile(path + "theme.license").exists())
		info.license = Application::instance()->readFile(path + "theme.license");

	QString themeInfo{Application::instance()->readFile(path + "theme.info")};

	RegExp regExp{"Name:(.*)\\n"};
	regExp.setMinimal(true);
	regExp.indexIn(themeInfo);

	if (regExp.captureCount() == 1)
		info.name = regExp.capture(1).trimmed();

	if (info.name.isEmpty() || m_themeHash.contains(info.name))
		return info;

	regExp.setPattern("Author:(.*)\\n");
	regExp.indexIn(themeInfo);

	if (regExp.captureCount() == 1)
		info.author = regExp.capture(1).trimmed();

	regExp.setPattern("Short Description:(.*)\\n");
	regExp.indexIn(themeInfo);

	if (regExp.captureCount() == 1)
		info.shortDesc = regExp.capture(1).trimmed();

	regExp.setPattern("Long Description:(.*)\\n");
	regExp.indexIn(themeInfo);

	if (regExp.captureCount() == 1)
		info.longDesc = regExp.capture(1).trimmed();

	info.isValid = true;

	m_themeHash.insert(name, info);

	return info;
}

void AppearancePage::loadSettings()
{
	QSettings settings{};

	settings.beginGroup("Settings");

	m_useRealToolBar
		->setChecked(settings.value(QLatin1String("useTopToolBar"), Application::instance()->useTopToolBar()).toBool());
	m_hideBookmarksHistoryActionsByDefault->setChecked(settings.value(QLatin1String("hideBookmarksHistoryByDefault"),
																	  Application::instance()
																		  ->hideBookmarksHistoryActions()).toBool());
	m_floatingButtonFoloweMouse->setChecked(settings.value(QLatin1String("floatingButtonFolowMouse"),
														   Application::instance()->floatingButtonFoloweMouse())
												.toBool());
	m_floatingButtonFoloweMouse->setEnabled(!m_useRealToolBar->isChecked());
	m_hideBookmarksHistoryActionsByDefault->setEnabled(m_useRealToolBar->isChecked());


	settings.endGroup();
	m_activeTheme = settings.value("Themes/currentTheme", "sielo_default").toString();

	m_themeList->clear();

	QDir dir{Application::instance()->paths()[Application::P_Themes]};
	QStringList list = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

		foreach (const QString& name, list) {
			Theme themeInfo = parseTheme(dir.absoluteFilePath(name) + QLatin1Char('/'), name);

			if (!themeInfo.isValid)
				continue;

			QListWidgetItem* item{new QListWidgetItem(m_themeList)};
			item->setText(themeInfo.name + "\n" + themeInfo.shortDesc);
			item->setIcon(themeInfo.icon);
			item->setData(Qt::UserRole, name);

			if (m_activeTheme == name)
				m_themeList->setCurrentItem(item);

			m_themeList->addItem(item);
		}

}

void AppearancePage::setupUI()
{

	m_areaWidget = new QWidget(this);
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(m_areaWidget->sizePolicy().hasHeightForWidth());
	m_areaWidget->setSizePolicy(sizePolicy);

	m_themeBox = new QGroupBox(tr("Themes"), this);

	m_layout = new QVBoxLayout(this);
	m_themeLayout = new QVBoxLayout(m_themeBox);
	m_areaLayout = new QFormLayout(m_areaWidget);
	m_nameLayout = new QHBoxLayout();
	m_themeActionLayout = new QHBoxLayout();

	m_areaLayout->setContentsMargins(4, 4, 4, 4);
	m_areaLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	m_themeList = new QListWidget(this);
	m_themeList->setIconSize(QSize(48, 48));

	m_nameLabel = new QLabel(tr("<b>Name: </b>"), m_areaWidget);
	//m_nameLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);

	m_name = new QLabel(m_areaWidget);
	//m_name->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_licenseBtn = new QPushButton(tr("License"), m_areaWidget);
	QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(m_licenseBtn->sizePolicy().hasHeightForWidth());
	m_licenseBtn->setSizePolicy(sizePolicy1);
	m_licenseBtn->setFlat(true);
	m_licenseBtn->hide();

	m_authorLabel = new QLabel(tr("<b>Author: </b>"), m_areaWidget);
	//m_authorLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);

	m_author = new QLabel(m_areaWidget);
	m_author->setWordWrap(true);
	//m_author->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_descLabel = new QLabel(tr("<b>Description: </b>"), m_areaWidget);
	//m_descLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);

	m_desc = new QLabel(m_areaWidget);
	m_desc->setWordWrap(true);
	//m_desc->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_addThemeButton = new QPushButton(tr("Add a Theme"), this);
	m_viewGalleryButton = new QPushButton(tr("Open Gallery"), this);

	m_useRealToolBar = new QCheckBox(tr("Use real toolbar instead of floating button"), this);
	m_hideBookmarksHistoryActionsByDefault =
		new QCheckBox(tr("Hide bookmarks and history action in the navigation tool bar by default"));
	m_floatingButtonFoloweMouse = new QCheckBox(tr("Floating button automatically move to focused tabs space"));

	m_nameLayout->addWidget(m_name);
	m_nameLayout->addWidget(m_licenseBtn);

	m_areaLayout->setWidget(0, QFormLayout::LabelRole, m_nameLabel);
	m_areaLayout->setWidget(1, QFormLayout::LabelRole, m_authorLabel);
	m_areaLayout->setWidget(1, QFormLayout::FieldRole, m_author);
	m_areaLayout->setWidget(2, QFormLayout::LabelRole, m_descLabel);
	m_areaLayout->setWidget(2, QFormLayout::FieldRole, m_desc);
	m_areaLayout->setLayout(0, QFormLayout::FieldRole, m_nameLayout);

	m_themeActionLayout->addWidget(m_addThemeButton);
	m_themeActionLayout->addWidget(m_viewGalleryButton);

	m_themeLayout->addWidget(m_themeList);
	m_themeLayout->addWidget(m_areaWidget);
	m_themeLayout->addLayout(m_themeActionLayout);

	m_layout->addWidget(m_themeBox);
	m_layout->addWidget(m_useRealToolBar);
	m_layout->addWidget(m_hideBookmarksHistoryActionsByDefault);
	m_layout->addWidget(m_floatingButtonFoloweMouse);
}
}