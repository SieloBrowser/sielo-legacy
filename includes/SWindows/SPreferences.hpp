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

class SThemeManager : public QObject
{
public:
    SThemeManager(QObject *parent = nullptr);
    ~SThemeManager();

    bool compressTheme(QString srcFolder, QString fileDestionation);
    bool decompressTheme(QString srcTheme);

private:
    QFile m_file{};
    QDataStream m_dataStream{};

    bool compress(QString srcFolder, QString prefex);
};

class GeneralPageWidget : public QWidget
{
public:
    GeneralPageWidget(QWidget *parent = nullptr);
    ~GeneralPageWidget();

    void save();

private:
    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QGroupBox *m_closeBox{ new QGroupBox(this) };
    QGroupBox *m_homePagebox{ new QGroupBox(this) };
    QVBoxLayout *m_closeLayout{ new QVBoxLayout(m_closeBox) };
    QVBoxLayout *m_homePageLayout{ new QVBoxLayout(m_homePagebox) };

    QRadioButton *m_homePageRButton{ new QRadioButton("Ouvrir sur la page d'accueil", m_closeBox) };
    QRadioButton *m_saveTabRButton{ new QRadioButton("Ouvrir les onglets de la dernière session", m_closeBox) };
    QLineEdit *m_homePageArea{ new QLineEdit(m_homePagebox) };
};

class BrowsPageWidget : public QWidget
{
public:
    BrowsPageWidget(SPreferencesWindow *parent = nullptr);
    ~BrowsPageWidget();

    void save();

public slots:
	void deleteAllCookies();

private:
	SPreferencesWindow *m_parent{ nullptr };
    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QGroupBox *m_webBox{ new QGroupBox(this) };
    QGroupBox *m_cookiesBox{ new QGroupBox(this) };
    QVBoxLayout *m_webLayout{ new QVBoxLayout(m_webBox) };
    QVBoxLayout *m_cookiesLayout{ new QVBoxLayout(m_cookiesBox) };

    QCheckBox *m_pluginCheckBox{ new QCheckBox("Activer les plugins ( ils doivent être installés )", m_webBox) };
    QCheckBox *m_javascripCheckBox{ new QCheckBox("Activer Javascript ( fortement recommandé ) ", m_webBox) };
    QCheckBox *m_cookiesCheckBox{ new QCheckBox("Activer les cookies ( fortement recommandé )", m_cookiesBox) };
    QPushButton *m_deleteAllCookies{ new QPushButton("Supprimer tous les cookies", m_cookiesBox) };
};

class ThemePageWidget : public QWidget
{
public:
    ThemePageWidget(QWidget *parent = nullptr);
    ~ThemePageWidget();

    void save();
private:
    void choosePath();
    void addTheme();

    QIcon m_folderIcon{ style()->standardIcon(QStyle::SP_DirClosedIcon) };
    QIcon m_okIcon{ SMainWindow::dataPath + "Images/ok.png" };

    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QGroupBox *m_chooseThemeBox{ new QGroupBox(this) };
    QGroupBox *m_addThemeBox{ new QGroupBox(this) };
    QVBoxLayout *m_chooseThemeLayout{ new QVBoxLayout(m_chooseThemeBox) };
    QVBoxLayout *m_addThemeLayout{ new QVBoxLayout(m_addThemeBox) };

    QComboBox *m_themeComboBox{ new QComboBox(m_chooseThemeBox) };
    QLineEdit *m_themePath{ new QLineEdit(m_addThemeBox) };
    QAction *m_choosePathAction{ m_themePath->addAction(m_folderIcon, QLineEdit::LeadingPosition) };
    QAction *m_addThemeAction{ m_themePath->addAction(m_okIcon, QLineEdit::TrailingPosition) };

};

class SPreferencesWindow : public QDialog
{
public:
    SPreferencesWindow(SMainWindow *parent = nullptr);
    ~SPreferencesWindow();

	SMainWindow *getParent() { return m_parent; }
public slots:
    void accept();

private:
    SMainWindow *m_parent{ nullptr };
    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QTabWidget *m_tab{ new QTabWidget(this) };

    QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this) };

    GeneralPageWidget *m_generalPageWidget{ new GeneralPageWidget(this) };
    BrowsPageWidget *m_browsPageWidget{ new BrowsPageWidget(this) };
    ThemePageWidget *m_themePageWidget{ new ThemePageWidget(this) };
};
