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
#ifndef Q_OS_WIN32
        QMessageBox::warning(nullptr, "Mise à joure", "Sielo Navigateur n'est pas à joure, nous vous \n"
                                                      "recommandont de passer à la version " + m_version);
#endif
    }
    else {
        QMessageBox::information(nullptr, "DEBUG", "Sielo est à joure !");
        m_reply = manager.get(QNetworkRequest(QUrl("http://feldrise.com/Sielo/showTxt.txt")));

        QEventLoop loop2;
        connect(m_reply, &QNetworkReply::finished, &loop2, &QEventLoop::quit);
        loop2.exec();

        QString showTxt{ m_reply->readAll() };
        if(showTxt == "true\n") {
            QMessageBox::information(nullptr, "DEBUG", "Need to show txt !");
            TextToShow *textToShow{ new TextToShow() };
            textToShow->show();
        }
        else {
            QMessageBox::information(nullptr, "DEBUG", "Don't need to show txt !");
        }
    }
}

SStarter::~SStarter()
{

}

TextToShow::TextToShow(QWidget *parent) :
    QDialog(parent)
{
    setModal(true);

    QNetworkAccessManager manager{};
    m_reply = manager.get(QNetworkRequest(QUrl("http://feldrise.com/Sielo/textToShow.html")));

    QEventLoop loop;
    connect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    m_textToShow->setText(m_reply->readAll());

    m_boxLayout->addWidget(m_textToShow);
    m_layout->addWidget(m_box);
    m_layout->addWidget(m_boxBtn);

    connect(m_boxBtn, &QDialogButtonBox::accepted, this, &TextToShow::accept);
    connect(m_boxBtn, &QDialogButtonBox::rejected, this, &TextToShow::close);
}

TextToShow::~TextToShow()
{

}
