#include "includes/SStarter.hpp"

#include <QNetworkRequest>
#include <QEventLoop>
#include <QMessageBox>

SStarter::SStarter(QObject *parent) :
    QObject(parent)
{
    QNetworkAccessManager manager{};
    m_reply = manager.get(QNetworkRequest(QUrl("http://feldrise.com/Sielo/version.txt")));

    QEventLoop loop;
    connect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    m_version = m_reply->readAll();
    if(m_version != m_currentVersion) {
        QMessageBox::information(nullptr, "DEBUG", "Sielo n'est pas à joure ! (" + m_version + "->" + m_currentVersion + ")");
    }
    else {
        QMessageBox::information(nullptr, "DEBUG", "Sielo est à joure !");
    }
}

SStarter::~SStarter()
{

}
