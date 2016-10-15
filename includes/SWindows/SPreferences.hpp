#pragma once

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>

class SMainWindow;

class HomePageWidget : public QWidget
{
public:
    HomePageWidget(QWidget *parent = nullptr);
    ~HomePageWidget();

    void save();

private:
    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QGroupBox *m_box{ new QGroupBox(this) };
    QVBoxLayout *m_boxLayout{ new QVBoxLayout(m_box) };

    QSpacerItem *m_spacer1{ new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Maximum) };
    QSpacerItem *m_spacer2{ new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Maximum) };
    QLabel *m_label{ new QLabel(QObject::tr("Entrez l'url de la page d'acceuil"), m_box) };
    QLineEdit *m_homePageArea{ new QLineEdit(m_box) };
};

class BrowsPageWidget : public QWidget
{
public:
    BrowsPageWidget(QWidget *parent = nullptr);
    ~BrowsPageWidget();

    void save();
private:
    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QGroupBox *m_webBox{ new QGroupBox(this) };
    QGroupBox *m_cookiesBox{ new QGroupBox(this) };
    QVBoxLayout *m_webLayout{ new QVBoxLayout(m_webBox) };
    QVBoxLayout *m_cookiesLayout{ new QVBoxLayout(m_cookiesBox) };

    QCheckBox *m_pluginCheckBox{ new QCheckBox("Activer les plugins ( ils doivent être installés )", this) };
    QCheckBox *m_javascripCheckBox{ new QCheckBox("Activer Javascript ( fortement recommandé ) ", this) };
    QCheckBox *m_cookiesCheckBox{ new QCheckBox("Activer les cookies ( fortement recommandé )", this) };
    QPushButton *m_deleteAllCookies{ new QPushButton("Supprimer tous les cookies", this) };
};

class SPreferencesWindow : public QDialog
{
public:
    SPreferencesWindow(SMainWindow *parent = nullptr);
    ~SPreferencesWindow();

public slots:
    void accept();

private:
    SMainWindow *m_parent{ nullptr };
    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QTabWidget *m_tab{ new QTabWidget(this) };

    QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this) };

    HomePageWidget *m_homePageWidget{ new HomePageWidget(this) };
    BrowsPageWidget *m_browsPageWidget{ new BrowsPageWidget(this) };
};
