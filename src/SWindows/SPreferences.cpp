#include "includes/SWindows/SPreferences.hpp"
#include "includes/SMainWindow.hpp"

#include <QWebEngineSettings>
#include <QDir>

SThemeManager::SThemeManager(QObject *parent) :
    QObject(parent)
{

}

SThemeManager::~SThemeManager()
{

}

bool SThemeManager::compressTheme(QString srcFolder, QString fileDestionation)
{
    QDir src{ srcFolder };
    if(!src.exists())
        return false;

    m_file.setFileName(fileDestionation);
    if(!m_file.open(QIODevice::WriteOnly))
        return false;

    m_dataStream.setDevice(&m_file);

    bool success{ compress(srcFolder, "") };
    m_file.close();

    QMessageBox::information(nullptr, "DEBUG", "Theme compressé !");
    return success;
}

bool SThemeManager::decompressTheme(QString srcTheme)
{
    QFile src{ srcTheme };
    QFileInfo themeName{ src };
    QMessageBox::information(nullptr, "DEBUG", themeName.baseName());
    QString destinationFolder{ "/home/feldrise/" + themeName.baseName() };

    if(!src.exists()) {
        QMessageBox::information(nullptr, "DEBUG", "Error is here ?");
        return false;
    }

    QDir dir{};
    if(!dir.mkpath(destinationFolder))
        return false;

    m_file.setFileName(srcTheme);
    if(!m_file.open(QIODevice::ReadOnly))
        return false;

    m_dataStream.setDevice(&m_file);

    while(!m_dataStream.atEnd()) {
        QString fileName{};
        QByteArray data{};

        m_dataStream >> fileName >> data;

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
            m_file.close();
            return false;
        }

        outFile.write(qUncompress(data));
        outFile.close();
    }

    m_file.close();

    QMessageBox::information(nullptr, "DEBUG", "Theme décompressé !");
    return true;
}

bool SThemeManager::compress(QString srcFolder, QString prefix)
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
