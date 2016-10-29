#include "includes/SWindows/SPreferences.hpp"
#include "includes/SWidgets/STabWidget.hpp"

#include <QWebEngineSettings>
#include <QDir>

SDataManager *SDataManager::mgr = new SDataManager();
SDataManager::SDataManager(QObject *parent) :
	QObject(parent)
{

}

SDataManager::~SDataManager()
{
	// Empty
}

bool SDataManager::compressData(QString srcFolder, QString fileDestionation)
{
	QDir src{ srcFolder };
	if(!src.exists())
		return false;

	mgr->m_file.setFileName(fileDestionation);
	if(!mgr->m_file.open(QIODevice::WriteOnly))
		return false;

	mgr->m_dataStream.setDevice(&mgr->m_file);

	bool success{ mgr->compress(srcFolder, "") };
	mgr->m_file.close();

	return success;
}

bool SDataManager::decompressData(QString srcData, QString destinationFolder)
{
	QFile src{ srcData };

	if(!src.exists())
		return false;

	QDir dir{};
	if(!dir.mkpath(destinationFolder))
		return false;

	mgr->m_file.setFileName(srcData);
	if(!mgr->m_file.open(QIODevice::ReadOnly))
		return false;

	mgr->m_dataStream.setDevice(&mgr->m_file);

	while(!mgr->m_dataStream.atEnd()) {
		QString fileName{};
		QByteArray data{};

		mgr->m_dataStream >> fileName >> data;

		QString subFolder{};
		for(int i{ fileName.length() - 1 }; i > 0; --i) {
			if((QString(fileName[i]) == QString("\\")) || (QString(fileName[i]) == QString("/"))) {
				subFolder = fileName.left(i);
				dir.mkpath(destinationFolder + "/" + subFolder);
				break;
			}
		}

		QFile outFile(destinationFolder + "/" + fileName);
		if(!outFile.open(QIODevice::WriteOnly)) {
			mgr->m_file.close();
			return false;
		}

		outFile.write(qUncompress(data));
		outFile.close();
	}

	mgr->m_file.close();

	return true;
}

bool SDataManager::compress(QString srcFolder, QString prefix)
{
	QDir dir{ srcFolder };
	if(!dir.exists())
		return false;

	dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
	QFileInfoList folderList{ dir.entryInfoList() };

	for(int i{ 0 }; i < folderList.length(); ++i) {
		QString folderName{ folderList[i].fileName() };
		QString folderPath{ dir.absolutePath() + "/" + folderName };
		QString newPrefex{ prefix + "/" + folderName };

		compress(folderPath, newPrefex);
	}

	dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
	QFileInfoList filesList{ dir.entryInfoList() };

	for(int i{ 0 }; i < filesList.length(); ++i) {
		QFile file{ dir.absolutePath() + "/" + filesList[i].fileName() };

		if(!file.open(QIODevice::ReadOnly))
			return false;

		m_dataStream << QString(prefix + "/" + filesList[i].fileName());
		m_dataStream << qCompress(file.readAll());

		file.close();
	}

	return true;
}

GeneralPageWidget::GeneralPageWidget(QWidget *parent) :
	QWidget(parent)
{
	// Fill the main layout
	m_layout->addWidget(m_closeBox);
	m_layout->addWidget(m_homePagebox);

	// Set widgets attributes
	m_homePageArea->setText(SMainWindow::SSettings->value("preferences/homePage", "http://google.com").toString());

	if(SMainWindow::SSettings->value("preferences/saveTabs", false).toBool())
		m_saveTabRButton->setChecked(true);
	else
		m_homePageRButton->setChecked(true);

	m_closeBox->setTitle(tr("Option d'ouverture des fenêtres"));
	m_homePagebox->setTitle(tr("Page d'accueil"));

	// Fill others layouts
	m_closeLayout->addWidget(m_homePageRButton);
	m_closeLayout->addWidget(m_saveTabRButton);
	m_homePageLayout->addWidget(m_homePageArea);
}

GeneralPageWidget::~GeneralPageWidget()
{

}

void GeneralPageWidget::save()
{
	if(m_saveTabRButton->isChecked())
		SMainWindow::SSettings->setValue("preferences/saveTabs", true);
	else
		SMainWindow::SSettings->setValue("preferences/saveTabs", false);

	SMainWindow::SSettings->setValue("preferences/homePage", m_homePageArea->text());
}

BrowsPageWidget::BrowsPageWidget(SPreferencesWindow *parent) :
	QWidget(parent),
	m_parent(parent)
{
	m_layout->addWidget(m_webBox);
	m_layout->addWidget(m_cookiesBox);

	m_pluginCheckBox->setChecked(SMainWindow::SSettings->value("preferences/enablePlugins", true).toBool());
	m_javascripCheckBox->setChecked(SMainWindow::SSettings->value("preferences/enableJavascript", true).toBool());
	m_cookiesCheckBox->setChecked(SMainWindow::SSettings->value("preferences/enableCookies", true).toBool());

	m_webBox->setTitle(tr("Options de navigation"));
	m_cookiesBox->setTitle(tr("Options pour les cookies"));

	m_webLayout->addWidget(m_pluginCheckBox);
	m_webLayout->addWidget(m_javascripCheckBox);
	m_cookiesLayout->addWidget(m_cookiesCheckBox);
	m_cookiesLayout->addWidget(m_deleteAllCookies);

	connect(m_deleteAllCookies, &QPushButton::clicked, this, &BrowsPageWidget::deleteAllCookies);
}

BrowsPageWidget::~BrowsPageWidget()
{

}

void BrowsPageWidget::save()
{
	SMainWindow::SSettings->setValue("preferences/enablePlugins", m_pluginCheckBox->isChecked());
	SMainWindow::SSettings->setValue("preferences/enableJavascript", m_javascripCheckBox->isChecked());
	SMainWindow::SSettings->setValue("preferences/enableCookies", m_cookiesCheckBox->isChecked());
}

void BrowsPageWidget::deleteAllCookies()
{
	for (int i{ 0 }; i < m_parent->getParent()->getTabs()->count() - 1; ++i) 
		m_parent->getParent()->getTabs()->widget(i)->findChild<SWebView*>()->page()->profile()->cookieStore()->deleteAllCookies();

	QMessageBox::information(this, tr("Cookies"), tr("Tous les cookies ont bien été suprimés"));
}

ThemePageWidget::ThemePageWidget(QWidget *parent) :
	QWidget(parent)
{
	m_layout->addWidget(m_chooseThemeBox);
	m_layout->addWidget(m_addThemeBox);

	for(int i{ 0 }; i < SMainWindow::SSettings->value("preferences/themes/nbre", 1).toInt(); ++i) {
		m_themeComboBox->addItem(SMainWindow::SSettings->value("preferences/themes/" + QString::number(i) + "/name").toString());
		if(SMainWindow::SSettings->value("preferences/themes/" + QString::number(i) + "/current", false).toBool()) {
			m_themeComboBox->setCurrentIndex(i);
		}
	}

	m_themePath->setPlaceholderText(tr("Chemin du thème à ajouter"));

	m_chooseThemeBox->setTitle(tr("Thème de la fenêtre"));
	m_addThemeBox->setTitle(tr("Ajouter un thème"));

	m_chooseThemeLayout->addWidget(m_themeComboBox);
	m_addThemeLayout->addWidget(m_themePath);

	connect(m_choosePathAction, &QAction::triggered, this, &ThemePageWidget::choosePath);
	connect(m_addThemeAction, &QAction::triggered, this, &ThemePageWidget::addTheme);

}

ThemePageWidget::~ThemePageWidget()
{

}

void ThemePageWidget::choosePath()
{
	QString path{ QFileDialog::getOpenFileName(this, "Choisir un thème", m_themePath->text(), "Sielo Navigateur Themes(*.snthm)") };

	if(!path.isEmpty()) {
		m_themePath->setText(path);
	}
}

void ThemePageWidget::addTheme()
{
	QString path{ m_themePath->text() };

	if(!path.endsWith(".snthm")) {
		QMessageBox::critical(this, tr("Erreur"), tr("Le thème n'est pas valide"));
		return;
	}

	QFileInfo themeInfo{ path };
	int index{ SMainWindow::SSettings->value("preferences/themes/nbre", 1).toInt() };

	SDataManager::decompressData(path, SMainWindow::dataPath + "Themes/" + themeInfo.baseName());
	QMessageBox::information(this, tr("Info"), tr("Le thème ") + themeInfo.baseName() + tr(" va être ajouté (patientez quelques instants s'il vous plait)"));

	SMainWindow::SSettings->beginGroup("preferences/themes/");
	SMainWindow::SSettings->setValue("nbre", index);
	SMainWindow::SSettings->setValue(QString::number(index) + "/name/", themeInfo.baseName());
	SMainWindow::SSettings->setValue("nbre", index + 1);
	SMainWindow::SSettings->endGroup();

	m_themeComboBox->addItem(themeInfo.baseName());
	QMessageBox::information(this, tr("Fin"), tr("Le thème à bien été ajouté"));
}

void ThemePageWidget::save()
{
	for(int i{ 0 }; i < m_themeComboBox->count(); ++i) {
		if(i == m_themeComboBox->currentIndex() && !SMainWindow::SSettings->value("preferences/themes/" + QString::number(i) + "/current", false).toBool()) {
			QMessageBox::information(this, tr("Application"), tr("Le thème sera appliqué au prochain redémarrage"));
			SMainWindow::SSettings->setValue("preferences/themes/" + QString::number(i) + "/current", true);
			SMainWindow::SSettings->setValue("preferences/themes/currentTheme", i);
		}
		else if(i != m_themeComboBox->currentIndex()) {
			SMainWindow::SSettings->setValue("preferences/themes/" + QString::number(i) + "/current", false);
		}
	}
}

SPreferencesWindow::SPreferencesWindow(SMainWindow *parent) :
	QDialog(parent),
	m_parent(parent)
{
	// Set window attributes
	resize(657, 270);
	setAttribute(Qt::WA_DeleteOnClose);

	// Fill the layout
	m_layout->addWidget(m_tab);
	m_layout->addWidget(m_boxBtn);

	// Add pages to the tab widget
	m_tab->addTab(m_generalPageWidget, tr("Page d'accueil"));
	m_tab->addTab(m_browsPageWidget, tr("Options de Navigation"));
	m_tab->addTab(m_themePageWidget, tr("Thème"));

	// Connections
	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &SPreferencesWindow::accept);
	connect(m_boxBtn, &QDialogButtonBox::rejected, this, &SPreferencesWindow::close);

}

SPreferencesWindow::~SPreferencesWindow()
{
	// Empty
}

void SPreferencesWindow::accept()
{
	// Save all settings
	m_generalPageWidget->save();
	m_browsPageWidget->save();
	m_themePageWidget->save();

	close();
}
