#pragma once

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class SStarter : public QObject
{
public:
    SStarter(QObject *parent = nullptr);
    ~SStarter();

private:
    QNetworkReply *m_reply{};
    QString m_currentVersion{ "0.1b\n" };
    QString m_version{};
};
