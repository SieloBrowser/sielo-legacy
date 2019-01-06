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

#include "Widgets/Preferences/Appearance.hpp"

#include <QProcess>
#include <QTimer>

#include <QMessageBox>
#include <QColorDialog>
#include <QFileDialog>

#include <QDir>
#include <QVariant>

#include "Utils/RegExp.hpp"
#include "Utils/DataPaths.hpp"
#include "Utils/Settings.hpp"

#include "Widgets/Preferences/PreferencesDialog.hpp"

#include "Application.hpp"

namespace Sn
{
QString AppearancePage::colorString(QString id)
{
	QColor returnColor{};
	Settings settings{};

	settings.beginGroup("Themes");

	if (id.contains("main"))
		returnColor = settings.value(QLatin1String("mainColor"), QColor(30, 30, 30)).value<QColor>();
	else if (id.contains("second"))
		returnColor = settings.value(QLatin1String("secondColor"), QColor(45, 45, 45)).value<QColor>();
	else if (id.contains("accent"))
		returnColor = settings.value(QLatin1String("accentColor"), QColor(29, 94, 173)).value<QColor>();
	else if (id.contains("text"))
		returnColor = settings.value(QLatin1String("textColor"), QColor(240, 240, 240)).value<QColor>();

	if (id.contains("light"))
		returnColor = returnColor.lighter();
	else if (id.contains("dark"))
		returnColor = returnColor.darker();

	return QString::number(returnColor.red()) + ", " + QString::number(returnColor.green()) + ", " +
		QString::number(returnColor.blue());
}

AppearancePage::AppearancePage(QWidget* parent, PreferencesDialog* preferencesDialog) :
	QWidget(parent),
	m_preferencesDialog(preferencesDialog)
{
	setupUI();
	loadSettings();

	connect(m_themeList, &QListWidget::currentItemChanged, this, &AppearancePage::currentChanged);
	connect(m_addThemeButton, &QPushButton::clicked, this, &AppearancePage::addTheme);
	connect(m_viewGalleryButton, &QPushButton::clicked, this, &AppearancePage::openGallery);
	connect(m_mainColorButton, &QPushButton::clicked, this, &AppearancePage::getColor);
	connect(m_secondColorButton, &QPushButton::clicked, this, &AppearancePage::getColor);
	connect(m_accentColorButton, &QPushButton::clicked, this, &AppearancePage::getColor);
	connect(m_textColorButton, &QPushButton::clicked, this, &AppearancePage::getColor);
	connect(m_showFloatingButton, &QCheckBox::toggled, this, &AppearancePage::showFloatingButtonChanged);
	connect(m_tabsSpacesPadding, &QSlider::valueChanged, this, &AppearancePage::tabsSpacesPaddingValueChanged);
	connect(m_blurFilterRadius, &QSlider::valueChanged, this, &AppearancePage::blurFilterRadiusValueChanged);
	connect(m_backgroundLocationButton, &QPushButton::clicked, this, &AppearancePage::backgroundLocationClicked);

	currentChanged();
}

AppearancePage::~AppearancePage()
{
	delete m_nameLayout;
}

void AppearancePage::save()
{
	Settings settings;

	settings.beginGroup("Settings");

	settings.setValue(QLatin1String("fullyLoadThemes"), m_fullyLoadThemes->isChecked());

	settings.setValue(QLatin1String("backdropBlur"), m_blurFilterRadius->value());

	if (!m_fullyLoadThemes->isChecked() && Application::instance()->fullyLoadThemes()) {
		QMessageBox::warning(this, tr("Warning"),
		                     tr(
			                     "Be aware, if you only load theme's icons, floating button will disappear. You should disable floating button when you don't load full theme."));
	}

	settings.setValue(QLatin1String("showFloatingButton"), m_showFloatingButton->isChecked());
	settings.setValue(QLatin1String("floatingButtonFoloweMouse"),
					  m_showFloatingButton->isChecked() ? m_floatingButtonFoloweMouse->isChecked() : false);
	settings.setValue(QLatin1String("hideToolbarControls"),
					  m_showFloatingButton->isChecked() ? m_hideControles->isChecked() : false);
	settings
		.setValue(QLatin1String("hideBookmarksHistoryByDefault"),
		          m_hideBookmarksHistoryActionsByDefault->isChecked());
	settings.setValue(QLatin1String("bottomToolBar"), m_bottomNavigationBar->isChecked());

	settings.setValue(QLatin1String("tabsSpacesPadding"), m_tabsSpacesPadding->value());
	settings.setValue(QLatin1String("repeatBackground"), m_repeatBackground->isChecked());
	settings.setValue(QLatin1String("backgroundPath"), m_backgroundLocationEdit->text());

	settings.endGroup();

	QListWidgetItem* currentItem = m_themeList->currentItem();

	if (m_activeTheme == currentItem->data(Qt::UserRole).toString())
		if (!m_colorsChanged)
			return;

	m_activeTheme = currentItem->data(Qt::UserRole).toString();

	settings.beginGroup("Themes");

	settings.setValue("currentTheme", m_activeTheme);
	settings.setValue("mainColor", m_mainColorButton->property("color").value<QColor>());
	settings.setValue("secondColor", m_secondColorButton->property("color").value<QColor>());
	settings.setValue("accentColor", m_accentColorButton->property("color").value<QColor>());
	settings.setValue("textColor", m_textColorButton->property("color").value<QColor>());

	if (m_mainColorButton->property("color").value<QColor>().lightness() > 150)
		settings.setValue("lightness", "light");
	else
		settings.setValue("lightness", "dark");

	settings.endGroup();

	Application::instance()->loadTheme(m_activeTheme);
	m_preferencesDialog->reloadTheme();
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

void AppearancePage::tabsSpacesPaddingValueChanged(int value)
{
	m_tabsSpacesPaddingLabel->setText(tr("Tabs spaces padding (%1px)").arg(value));
}

void AppearancePage::blurFilterRadiusValueChanged(int value)
{
	m_blurFilterRadiusLabel->setText(tr("Background blur filter radius (%1px)").arg(value));
}


void AppearancePage::backgroundLocationClicked()
{
	QString backgroundLocation{
		QFileDialog::getOpenFileName(this, tr("Choose background"),
		                             QFileInfo(m_backgroundLocationEdit->text()).absolutePath(),
		                             "Images (*.png *.jpg *.jpeg)")
	};

	if (backgroundLocation.isEmpty())
		return;

	m_backgroundLocationEdit->setText(backgroundLocation);
}

void AppearancePage::openGallery()
{
	QMessageBox::critical(this, tr("Unavailable"), tr("The gallery is currently unavailable."));
}

void AppearancePage::addTheme()
{
#if defined(Q_OS_WIN)
	QString compilerName = "sielo-compiler.exe";
#else
	QString compilerName = "sielo-compiler";
#endif
	if (!QFile(QDir(QCoreApplication::applicationDirPath()).absolutePath() + QLatin1Char('/') + compilerName)
		.exists()) {
		QMessageBox::critical(this,
		                      tr("Error"),
		                      tr(
			                      "Can't decompile theme... Be sure compiler is with Sielo main exe, else move it manually or update/reinstall the browser."));
		return;
	}

	QString themeFile{QFileDialog::getOpenFileName(this, tr("Open a theme"), QString(), "Themes (*.snthm)")};

	if (themeFile.isEmpty())
		return;

	QFileInfo themeInfo{
		DataPaths::currentProfilePath() + "/themes/" + QLatin1Char('/')
		+ QFileInfo(themeFile).baseName()
		+ QLatin1String("/main.sss")
	};

	if (themeInfo.exists()) {
		QMessageBox::warning(this,
		                     tr("Theme exist"),
		                     tr("The theme already exist and is going to be update with the new version."));
		QDir(DataPaths::currentProfilePath() + "/themes/" + QLatin1Char('/') + QFileInfo(themeFile).baseName())
			.removeRecursively();
	}

	QStringList decompileArgs{};
	decompileArgs << "decompile" << themeFile
		<< DataPaths::currentProfilePath() + "/themes/" + QLatin1Char('/')
		+ QFileInfo(themeFile).baseName() << "Theme successfully decompiled";

	QProcess::execute(QDir(QCoreApplication::applicationDirPath()).absolutePath() + QLatin1Char('/') + compilerName,
	                  decompileArgs);

	QTimer::singleShot(500, this, &AppearancePage::loadSettings);
}

void AppearancePage::getColor()
{
	QPushButton* button{qobject_cast<QPushButton*>(sender())};

	QColor choosenColor{
		QColorDialog::getColor(button->property("color").value<QColor>(), m_themeColorsBox, tr("Select a color"))
	};

	if (choosenColor.isValid() && choosenColor != button->property("color").value<QColor>()) {
		button->setProperty("color", choosenColor);
		button->setStyleSheet(
			"border: 1px solid " + QLatin1String((choosenColor.lightness() > 150) ? "#000000;" : "#ffffff;") +
			" background: " + choosenColor.name() + "; color: " +
			QLatin1String((choosenColor.lightness() > 150) ? " #000000;" : "#ffffff;"));
		m_colorsChanged = true;
	}
}

void AppearancePage::showFloatingButtonChanged(bool enabled)
{
	Q_UNUSED(enabled);

	m_floatingButtonFoloweMouse->setEnabled(m_showFloatingButton->isChecked());
	m_hideControles->setEnabled(m_showFloatingButton->isChecked());
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
	Settings settings{};

	settings.beginGroup("Settings");

	m_fullyLoadThemes
		->setChecked(settings.value(QLatin1String("fullyLoadThemes"), Application::instance()->fullyLoadThemes())
		                     .toBool());
	m_showFloatingButton
		->setChecked(
			settings.value(QLatin1String("showFloatingButton"), Application::instance()->showFloatingButton()).toBool());
	m_hideControles->setChecked(settings.value(QLatin1String("hideToolbarControls"), Application::instance()->hideToolbarControls()).toBool());
	m_hideControles->setEnabled(m_showFloatingButton->isChecked());
	m_floatingButtonFoloweMouse->setChecked(settings.value(QLatin1String("floatingButtonFolowMouse"),
	                                                       Application::instance()->floatingButtonFoloweMouse())
	                                                .toBool());
	m_floatingButtonFoloweMouse->setEnabled(m_showFloatingButton->isChecked());
	m_hideBookmarksHistoryActionsByDefault->setChecked(settings.value(QLatin1String("hideBookmarksHistoryByDefault"),
													   Application::instance()
													   ->hideBookmarksHistoryActions()).toBool());
	m_bottomNavigationBar->setChecked(settings.value("bottomToolBar", false).toBool());

	m_tabsSpacesPadding->setValue(settings.value(QLatin1String("tabsSpacesPadding"), 7).toInt());
	m_tabsSpacesPaddingLabel->setText(tr("Tabs spaces padding (%1px)").arg(m_tabsSpacesPadding->value()));
	m_repeatBackground->setChecked(settings.value(QLatin1String("repeatBackground"), false).toBool());
	m_backgroundLocationEdit->setText(settings.value(QLatin1String("backgroundPath"), "").toString());
	m_blurFilterRadius->setValue(settings.value(QLatin1String("backdropBlur"), 100).toInt());
	m_blurFilterRadiusLabel->setText(tr("Background blur filter radius (%1px)").arg(m_blurFilterRadius->value()));

	settings.endGroup();
	settings.beginGroup("Themes");

	m_activeTheme = settings.value("currentTheme", "sielo-default").toString();

	m_themeList->clear();

	QDir dir{DataPaths::currentProfilePath() + "/themes"};
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

	QColor mainColor{settings.value(QLatin1String("mainColor"), QColor{60, 60, 60}).value<QColor>()};
	QColor secondColor{settings.value(QLatin1String("secondColor"), QColor{30, 30, 30}).value<QColor>()};
	QColor accentColor{settings.value(QLatin1String("accentColor"), QColor{29, 94, 173}).value<QColor>()};
	QColor textColor{settings.value(QLatin1String("textColor"), QColor{240, 240, 240}).value<QColor>()};

	m_mainColorButton->setProperty("color", mainColor);
	m_mainColorButton->setStyleSheet(
		"border: 1px solid " + QLatin1String((mainColor.lightness() > 150) ? "#000000;" : "#ffffff;") +
		" background: " + mainColor.name() + "; color: " +
		QLatin1String((mainColor.lightness() > 150) ? " #000000;" : "#ffffff;"));
	m_secondColorButton->setProperty("color", secondColor);
	m_secondColorButton->setStyleSheet(
		"border: 1px solid " + QLatin1String((secondColor.lightness() > 150) ? "#000000;" : "#ffffff;") +
		" background: " + secondColor.name() + "; color: " +
		QLatin1String((secondColor.lightness() > 150) ? " #000000;" : "#ffffff;"));
	m_accentColorButton->setProperty("color", accentColor);
	m_accentColorButton->setStyleSheet(
		"border: 1px solid " + QLatin1String((accentColor.lightness() > 150) ? "#000000;" : "#ffffff;") +
		" background: " + accentColor.name() + "; color: " +
		QLatin1String((accentColor.lightness() > 150) ? " #000000;" : "#ffffff;"));
	m_textColorButton->setProperty("color", textColor);
	m_textColorButton->setStyleSheet(
		"border: 1px solid " + QLatin1String((textColor.lightness() > 150) ? "#000000;" : "#ffffff;") +
		" background: " + textColor.name() + "; color: " +
		QLatin1String((textColor.lightness() > 150) ? " #000000;" : "#ffffff;"));

	settings.endGroup();
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
	m_themeColorsBox = new QGroupBox(tr("Theme colors (might not work on every themes)"), this);

	m_tabs = new QTabWidget(this);
	m_themePage = new QWidget(m_tabs);
	m_advancedPage = new QWidget(m_tabs);

	m_layout = new QVBoxLayout(this);
	m_themePageLayout = new QVBoxLayout(m_themePage);
	m_advancedPageLayout = new QVBoxLayout(m_advancedPage);

	m_themeLayout = new QVBoxLayout(m_themeBox);
	m_themeColorsLayout = new QGridLayout(m_themeColorsBox);
	m_areaLayout = new QFormLayout(m_areaWidget);
	m_nameLayout = new QHBoxLayout();
	m_themeActionLayout = new QHBoxLayout();
	m_backgroundLayout = new QHBoxLayout();
	m_blurFilterLayout = new QVBoxLayout();

	m_themeColorsLayout->setContentsMargins(4, 4, 4, 4);
	m_areaLayout->setContentsMargins(4, 4, 4, 4);
	m_areaLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	m_themeList = new QListWidget(this);
	m_themeList->setIconSize(QSize(48, 48));

	m_nameLabel = new QLabel(tr("<b>Name: </b>"), m_areaWidget);
	m_nameLabel->setAlignment(static_cast<Qt::Alignment>(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop));

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
	m_authorLabel->setAlignment(static_cast<Qt::Alignment>(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop));

	m_author = new QLabel(m_areaWidget);
	m_author->setWordWrap(true);
	//m_author->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_descLabel = new QLabel(tr("<b>Description: </b>"), m_areaWidget);
	m_descLabel->setAlignment(static_cast<Qt::Alignment>(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop));

	m_desc = new QLabel(m_areaWidget);
	m_desc->setWordWrap(true);
	//m_desc->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_addThemeButton = new QPushButton(tr("Add a Theme"), this);
	m_viewGalleryButton = new QPushButton(tr("Open Gallery"), this);

	m_mainColorButton = new QPushButton(tr("Main Color"), m_themeColorsBox);
	m_secondColorButton = new QPushButton(tr("Second Color"), m_themeColorsBox);
	m_accentColorButton = new QPushButton(tr("Accent Color"), m_themeColorsBox);
	m_textColorButton = new QPushButton(tr("Text Color"), m_themeColorsBox);

	m_fullyLoadThemes = new QCheckBox(tr("Fully load theme (otherwise it will only load theme's icons)"));
	
	m_showFloatingButton = new QCheckBox(tr("Show Floating Button"), this);
	m_floatingButtonFoloweMouse = new QCheckBox(tr("Floating button automatically move to focused tabs space"));
	m_hideControles = new QCheckBox(tr("Hide Toolbar controls (back, forward, etc.)"), this);
	m_hideBookmarksHistoryActionsByDefault =
		new QCheckBox(tr("Hide bookmarks and history action in the navigation tool bar by default"));
#ifdef Q_OS_WIN
	m_bottomNavigationBar = new QCheckBox(tr("Show title bar at the bottom"), this);
#else 
	m_bottomNavigationBar = new QCheckBox(tr("Show navigation bar at the bottom"), this);
#endif
	
	m_tabsSpacesPaddingLabel = new QLabel(tr("Tabs spaces padding (in px)"), this);
	m_tabsSpacesPadding = new QSlider(Qt::Horizontal, this);
	m_tabsSpacesPadding->setMinimum(0);
	m_tabsSpacesPadding->setMaximum(60);

	m_repeatBackground = new QCheckBox(tr("Repeate the background if the picture is too small"), this);

	m_backgroundLabel = new QLabel(tr("Background: "), this);
	m_backgroundLocationEdit = new QLineEdit(this);
	m_backgroundLocationButton = new QPushButton(tr("..."), this);
	m_spacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_blurFilterRadius = new QSlider(Qt::Horizontal, this);
	m_blurFilterRadius->setMinimum(0);
	m_blurFilterRadius->setMaximum(999);
	m_blurFilterRadiusLabel = new QLabel(tr("Background blur filter radius (in px)"), this);

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

	m_themeColorsLayout->addWidget(m_mainColorButton, 0, 0, 1, 1);
	m_themeColorsLayout->addWidget(m_secondColorButton, 0, 1, 1, 1);
	m_themeColorsLayout->addWidget(m_accentColorButton, 1, 0, 1, 1);
	m_themeColorsLayout->addWidget(m_textColorButton, 1, 1, 1, 1);

	m_themeLayout->addWidget(m_themeList);
	m_themeLayout->addWidget(m_areaWidget);
	m_themeLayout->addLayout(m_themeActionLayout);
	m_themeLayout->addWidget(m_themeColorsBox);

	m_backgroundLayout->addWidget(m_backgroundLabel);
	m_backgroundLayout->addWidget(m_backgroundLocationEdit);
	m_backgroundLayout->addWidget(m_backgroundLocationButton);

	m_blurFilterLayout->addWidget(m_blurFilterRadiusLabel);
	m_blurFilterLayout->addWidget(m_blurFilterRadius);

	m_themePageLayout->addWidget(m_themeBox);
	m_themePageLayout->addWidget(m_fullyLoadThemes);

	m_advancedPageLayout->addWidget(m_showFloatingButton);
	m_advancedPageLayout->addWidget(m_floatingButtonFoloweMouse);
	m_advancedPageLayout->addWidget(m_hideControles);
	m_advancedPageLayout->addWidget(m_hideBookmarksHistoryActionsByDefault);
	m_advancedPageLayout->addWidget(m_bottomNavigationBar);
	m_advancedPageLayout->addWidget(m_tabsSpacesPaddingLabel);
	m_advancedPageLayout->addWidget(m_tabsSpacesPadding);
	m_advancedPageLayout->addWidget(m_repeatBackground);
	m_advancedPageLayout->addLayout(m_backgroundLayout);
	m_advancedPageLayout->addLayout(m_blurFilterLayout);
	m_advancedPageLayout->addItem(m_spacer);

#ifdef Q_OS_WIN
	if (QCoreApplication::testAttribute(Qt::AA_UseSoftwareOpenGL)) {
		m_oldChipsetInfoLabel = new QLabel(tr("You are using an old chipset that doesn't have a correct OpenGL port. To improve user experience, the hardware acceleration is disabled."), this);
		m_advancedPageLayout->addWidget(m_oldChipsetInfoLabel);
	}
#endif

	m_tabs->addTab(m_themePage, tr("Themes"));
	m_tabs->addTab(m_advancedPage, tr("Advanced"));

	m_layout->addWidget(m_tabs);
}
}
