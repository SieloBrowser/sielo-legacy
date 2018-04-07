#pragma once

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QString>

class Manager : public QObject
{
public:
    Manager(Manager *parent = nullptr);
    ~Manager();

    bool compressFolder(QString srcFolder, QString fileDestination);
    bool decompress(QString src, QString dst);

private:
    QFile m_file{};
    QDataStream m_dataStream{};

    bool compress(QString srcFolder, QString prefex);
};
