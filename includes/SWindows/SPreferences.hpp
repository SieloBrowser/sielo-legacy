#pragma once

#include <QStandardPaths>
#include <QWidget>
#include <QDialog>
#include <QFile>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>

#include "includes/SMainWindow.hpp"

class SMainWindow;
class SPreferencesWindow;

// This is the class for compress and decompress Sielo themes
class SDataManager : public QObject
{
public:
	// Constructor and destructor
	SDataManager(QObject *parent = nullptr);
	~SDataManager();

	static bool compressData(QString srcFolder, QString fileDestionation);
	static bool decompressData(QString srcData, QString destinationFolder);

	static SDataManager *mgr;
private:
	QFile m_file{};
	QDataStream m_dataStream{};

	bool compress(QString srcFolder, QString prefex);
};

// This is the page to choose basic options like home page
class GeneralPageWidget : public QWidget
{
public:
	// Constructor and destructor
	GeneralPageWidget(QWidget *parent = nullptr);
	~GeneralPageWidget();

	void save(); //< Save the settings from this page

private:
	// Layouts and boxes of the page
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QGroupBox *m_closeBox{ new QGroupBox(this) };
	QGroupBox *m_homePagebox{ new QGroupBox(this) };
	QVBoxLayout *m_closeLayout{ new QVBoxLayout(m_closeBox) };
	QVBoxLayout *m_homePageLayout{ new QVBoxLayout(m_homePagebox) };

	// Widgets of the page
	QRadioButton *m_homePageRButton{ new QRadioButton(QObject::tr("Ouvrir sur la page d'accueil"), m_closeBox) };
	QRadioButton *m_saveTabRButton{ new QRadioButton(QObject::tr("Ouvrir les onglets de la dernière session"), m_closeBox) };
	QLineEdit *m_homePageArea{ new QLineEdit(m_homePagebox) };
};

// This is the page to choose brows options like activation of plugins 
class BrowsPageWidget : public QWidget
{
public:
	// Constructor and destructor
	BrowsPageWidget(SPreferencesWindow *parent = nullptr);
	~BrowsPageWidget();

	void save(); //< Save the settings from this page

public slots:
	void deleteAllCookies();

private:
	SPreferencesWindow *m_parent{ nullptr };
	
	// Layouts and boxes for this page
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QGroupBox *m_webBox{ new QGroupBox(this) };
	QGroupBox *m_cookiesBox{ new QGroupBox(this) };
	QVBoxLayout *m_webLayout{ new QVBoxLayout(m_webBox) };
	QVBoxLayout *m_cookiesLayout{ new QVBoxLayout(m_cookiesBox) };

	// Widgets for this page
	QCheckBox *m_pluginCheckBox{ new QCheckBox(QObject::tr("Activer les plugins (ils doivent être installés)"), m_webBox) };
	QCheckBox *m_javascripCheckBox{ new QCheckBox(QObject::tr("Activer Javascript (fortement recommandé)"), m_webBox) };
	QCheckBox *m_cookiesCheckBox{ new QCheckBox(QObject::tr("Activer les cookies (fortement recommandé)"), m_cookiesBox) };
	QPushButton *m_deleteAllCookies{ new QPushButton(QObject::tr("Supprimer tous les cookies"), m_cookiesBox) };
};

// This is the page for add and change themes
class ThemePageWidget : public QWidget
{
public:
	// Constructor and destructor
	ThemePageWidget(QWidget *parent = nullptr);
	~ThemePageWidget();

	void save(); //< Save the settings for this page
private:
	void choosePath(); //< Choose a path to add in the QLineEdit for the theme to add
	void addTheme(); //< Add a theme to Sielo

	QIcon m_folderIcon{ style()->standardIcon(QStyle::SP_DirClosedIcon) };
	QIcon m_okIcon{ SMainWindow::dataPath + "Images/ok.png" };

	// Layouts and boxes of the page
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QGroupBox *m_chooseThemeBox{ new QGroupBox(this) };
	QGroupBox *m_addThemeBox{ new QGroupBox(this) };
	QVBoxLayout *m_chooseThemeLayout{ new QVBoxLayout(m_chooseThemeBox) };
	QVBoxLayout *m_addThemeLayout{ new QVBoxLayout(m_addThemeBox) };

	// Widgets of the page
	QComboBox *m_themeComboBox{ new QComboBox(m_chooseThemeBox) };
	QLineEdit *m_themePath{ new QLineEdit(m_addThemeBox) };
	QAction *m_choosePathAction{ m_themePath->addAction(m_folderIcon, QLineEdit::LeadingPosition) };
	QAction *m_addThemeAction{ m_themePath->addAction(m_okIcon, QLineEdit::TrailingPosition) };

};

// This is the dialog with all pages for preferences
class SPreferencesWindow : public QDialog
{
public:
	// Constructor and destructor
	SPreferencesWindow(SMainWindow *parent = nullptr);
	~SPreferencesWindow();

	SMainWindow *getParent() { return m_parent; }
public slots:
	void accept(); //< Save all settings

private:
	SMainWindow *m_parent{ nullptr };

	// The layout and tab of the window
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QTabWidget *m_tab{ new QTabWidget(this) };

	// The widget of window
	QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this) };

	// All the pages of the window
	GeneralPageWidget *m_generalPageWidget{ new GeneralPageWidget(this) };
	BrowsPageWidget *m_browsPageWidget{ new BrowsPageWidget(this) };
	ThemePageWidget *m_themePageWidget{ new ThemePageWidget(this) };
};
