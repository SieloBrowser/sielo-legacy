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

// This is the page to choose basic options like home page
class GeneralPageWidget : public QWidget
{
public:
	// Constructor and destructor
	GeneralPageWidget(QWidget *parent = nullptr);
	~GeneralPageWidget();

	void save(); //< Save the settings from this page

	QCheckBox *getShowMenuBar() { return m_showMenuBar; }

private:
	// Layouts and boxes of the page
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QGroupBox *m_closeBox{ new QGroupBox(this) };
	QGroupBox *m_personalisationBox{ new QGroupBox(this) };
	QGroupBox *m_homePagebox{ new QGroupBox(this) };
	QVBoxLayout *m_closeLayout{ new QVBoxLayout(m_closeBox) };
	QGridLayout *m_personalisationLayout{ new QGridLayout(m_personalisationBox) };
	QVBoxLayout *m_homePageLayout{ new QVBoxLayout(m_homePagebox) };

	// Widgets of the page
	QRadioButton *m_homePageRButton{ new QRadioButton(QObject::tr("Ouvrir sur la page d'accueil"), m_closeBox) };
	QRadioButton *m_saveTabRButton{ new QRadioButton(QObject::tr("Ouvrir les onglets de la dernière session"), m_closeBox) };
	QCheckBox *m_showMenuBar{ new QCheckBox(QObject::tr("Afficher la barre de menu"), m_personalisationBox) };
	QLabel *m_loadingBarStyleLabel{ new QLabel(QObject::tr("Style de la barre de chargement"), m_personalisationBox) };
	QComboBox *m_loadingBarStyleComboBox{ new QComboBox(m_personalisationBox) };
	QRadioButton *m_prefereOpenInNewTab{ new QRadioButton(QObject::tr("Ouvrir les fenêtres de Sielo dans un nouvelle onglet"), m_personalisationBox) };
	QRadioButton *m_prefereOpenInNewWin{ new QRadioButton(QObject::tr("Ouvrir les fenêtres de Sielo dans une nouvelle fenêtre"), m_personalisationBox) };
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
};
