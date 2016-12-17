#include "includes/Manager.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

Manager::Manager(Manager *parent) :
    QObject(parent)
{
    QStringList args{ QCoreApplication::arguments() };

    if(args[1] == "compress") {
        compressFolder(args[2], args[3]);
    }
    else if(args[1] == "decompress") {
        decompress(args[2], args[3]);
    }

}

Manager::~Manager()
{
    // Empty
}

bool Manager::compressFolder(QString srcFolder, QString fileDestionation)
{
    QDir src{ srcFolder };
    if (!src.exists())
        return false;

    m_file.setFileName(fileDestionation);
    if (!m_file.open(QIODevice::WriteOnly))
        return false;

    m_dataStream.setDevice(&m_file);

    bool success{ compress(srcFolder, "") };
    m_file.close();

    return success;
}

bool Manager::decompress(QString srcTheme, QString dst)
{
    QFile src{ srcTheme };
    QFileInfo themeName{ src };
    QString destinationFolder{ dst };

    if (!src.exists())
        return false;

    QDir dir{};
    if (!dir.mkpath(destinationFolder))
        return false;

    m_file.setFileName(srcTheme);
    if (!m_file.open(QIODevice::ReadOnly))
        return false;

    m_dataStream.setDevice(&m_file);

    while (!m_dataStream.atEnd()) {
        QString fileName{};
        QByteArray data{};

        m_dataStream >> fileName >> data;

        QString subFolder{};
        for (int i{ fileName.length() - 1 }; i > 0; --i) {
            if ((QString(fileName[i]) == QString("\\")) || (QString(fileName[i]) == QString("/"))) {
                subFolder = fileName.left(i);
                dir.mkpath(destinationFolder + "/" + subFolder);
                break;
            }
        }

        QFile outFile(destinationFolder + "/" + fileName);
        if (!outFile.open(QIODevice::WriteOnly)) {
            m_file.close();
            return false;
        }

        outFile.write(qUncompress(data));
        outFile.close();
    }

    m_file.close();

    return true;
}

bool Manager::compress(QString srcFolder, QString prefix)
{
    QDir dir{ srcFolder };
    if (!dir.exists())
        return false;

    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    QFileInfoList folderList{ dir.entryInfoList() };

    for (int i{ 0 }; i < folderList.length(); ++i) {
        QString folderName{ folderList[i].fileName() };
        QString folderPath{ dir.absolutePath() + "/" + folderName };
        QString newPrefex{ prefix + "/" + folderName };

        compress(folderPath, newPrefex);
    }

    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    QFileInfoList filesList{ dir.entryInfoList() };

    for (int i{ 0 }; i < filesList.length(); ++i) {
        QFile file{ dir.absolutePath() + "/" + filesList[i].fileName() };

        if (!file.open(QIODevice::ReadOnly))
            return false;

        m_dataStream << QString(prefix + "/" + filesList[i].fileName());
        m_dataStream << qCompress(file.readAll());

        file.close();
    }

    return true;
}
