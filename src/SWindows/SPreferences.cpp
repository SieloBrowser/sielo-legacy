#include "includes/SWindows/SPreferences.hpp"
#include "includes/SMainWindow.hpp"

#include <QWebEngineSettings>

HomePageWidget::HomePageWidget(QWidget *parent) :
    QWidget(parent)
{
    m_homePageArea->setText(SMainWindow::SSettings->value("preferences/homePage", "http://google.com").toString());

    m_layout->addWidget(m_box);

    m_boxLayout->addItem(m_spacer1);
    m_boxLayout->addWidget(m_label);
    m_boxLayout->addWidget(m_homePageArea);
    m_boxLayout->addItem(m_spacer2);
}

HomePageWidget::~HomePageWidget()
{

}

void HomePageWidget::save()
{
    SMainWindow::SSettings->setValue("preferences/homePage", m_homePageArea->text());
}

BrowsPageWidget::BrowsPageWidget(QWidget *parent) :
    QWidget(parent)
{
    m_layout->addWidget(m_webBox);
    m_layout->addWidget(m_cookiesBox);

    m_pluginCheckBox->setChecked(SMainWindow::SSettings->value("preferences/enablePlugins", true).toBool());
    m_javascripCheckBox->setChecked(SMainWindow::SSettings->value("preferences/enableJavascript", true).toBool());
    m_cookiesCheckBox->setChecked(SMainWindow::SSettings->value("preferences/enableCookies", true).toBool());

    m_webLayout->addWidget(m_pluginCheckBox);
    m_webLayout->addWidget(m_javascripCheckBox);
    m_cookiesLayout->addWidget(m_cookiesCheckBox);
    m_cookiesLayout->addWidget(m_deleteAllCookies);
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

SPreferencesWindow::SPreferencesWindow(SMainWindow *parent) :
    QDialog(parent),
    m_parent(parent)
{
    resize(657, 270);
    m_layout->addWidget(m_tab);
    m_layout->addWidget(m_boxBtn);

    m_tab->addTab(m_homePageWidget, tr("Page d'acceuil"));
    m_tab->addTab(m_browsPageWidget, tr("Navigation"));

    connect(m_boxBtn, &QDialogButtonBox::accepted, this, &SPreferencesWindow::accept);
    connect(m_boxBtn, &QDialogButtonBox::rejected, this, &SPreferencesWindow::close);
}

SPreferencesWindow::~SPreferencesWindow()
{

}

void SPreferencesWindow::accept()
{
    m_homePageWidget->save();
    m_browsPageWidget->save();

    close();
}
