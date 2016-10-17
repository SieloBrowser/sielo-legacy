#include "includes/SStarter.hpp"
#include "includes/SMainWindow.hpp"

#include <QWebEngineView>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QFile>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QMessageBox>

SStarter::SStarter(QObject *parent) :
    QObject(parent)
{
    QNetworkAccessManager manager{};
    m_reply = manager.get(QNetworkRequest(QUrl("http://feldrise.com/Sielo/version.txt")));

	QEventLoop loop{};
    connect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    m_version = m_reply->readAll();
    if(m_version != m_currentVersion) {
#ifndef Q_OS_WIN32
        QMessageBox::warning(nullptr, "Mise à joure", "Sielo Navigateur n'est pas à joure, nous vous \n"
                                                      "recommandont de passer à la version " + m_version);
#else
		if (SMainWindow::SSettings->value("Maj/remind", true).toBool()) {
			MaJDialog *majDialog{ new MaJDialog(nullptr) };
			majDialog->show();
		}
		else {
			SMainWindow* fen{ new SMainWindow() };

			QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
			QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, SMainWindow::SSettings->value("preferences/enablePlugins", true).toBool());
			QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, SMainWindow::SSettings->value("preferences/enableJavascript", true).toBool());

			fen->show();
		}
#endif

    }
    else {
		SMainWindow* fen{ new SMainWindow() };

		QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
		QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, SMainWindow::SSettings->value("preferences/enablePlugins", true).toBool());
		QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, SMainWindow::SSettings->value("preferences/enableJavascript", true).toBool());

		fen->show();

        m_reply = manager.get(QNetworkRequest(QUrl("http://feldrise.com/Sielo/showTxt.txt")));

		QEventLoop loop2{};
        connect(m_reply, &QNetworkReply::finished, &loop2, &QEventLoop::quit);
        loop2.exec();

        QString showTxt{ m_reply->readAll() };
        if(showTxt == "true\n") {
            TextToShow *textToShow{ new TextToShow(fen) };
            textToShow->show();
        }
        else {
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

	QEventLoop loop{{}};
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

MaJDialog::MaJDialog(QWidget * parent)
{
	setModal(true);
	m_layout->addWidget(m_box);
	m_layout->addLayout(m_btnLayout);

	m_box->setTitle(tr("Une nouvelle mise à joure est diponible !"));
	m_boxBtn->addButton(m_installButton, QDialogButtonBox::AcceptRole);
	m_boxBtn->addButton(QDialogButtonBox::Close);;
	m_remindMaj->setChecked(true);

	m_reply = m_netManager.get(QNetworkRequest(QUrl("http://feldrise.com/Sielo/updateTxt.html")));
	QEventLoop loop{};
	connect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();
	m_text->setText(m_reply->readAll());

	m_boxLayout->addWidget(m_text);
	m_boxLayout->addWidget(m_progress);
	m_btnLayout->addWidget(m_remindMaj);
	m_btnLayout->addWidget(m_boxBtn);

	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &MaJDialog::startUpdate);
	connect(m_boxBtn, &QDialogButtonBox::rejected, this, &MaJDialog::close);
}

MaJDialog::~MaJDialog()
{
}

void MaJDialog::startUpdate()
{
	m_boxBtn->setEnabled(false);
	m_installingUpdate = true;

	m_reply = m_netManager.get(QNetworkRequest(QUrl("http://feldrise.com/Sielo/SN_update_setup.exe")));
	connect(m_reply, &QNetworkReply::downloadProgress, this, &MaJDialog::downloadProgress);
	connect(m_reply, &QNetworkReply::finished, this, &MaJDialog::save);
}

void MaJDialog::downloadProgress(quint64 bytesReceived, quint16 bytesTotal)
{
	if (bytesTotal != -1) {
		m_progress->setRange(0, bytesTotal);
		m_progress->setValue(bytesReceived);
	}
}

void MaJDialog::save()
{
	m_updateSuccess = true;
	m_installingUpdate = false;

	QMessageBox::information(this, "DEBUG", QStandardPaths::writableLocation(QStandardPaths::TempLocation));
	m_reply->deleteLater();
	QFile updater{ QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/SNUpdater.exe" };

	if (!updater.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(this, "Erreur", "Erreur lors de l'installation de la mise à jours !");
		return;
	}

	updater.write(m_reply->readAll());
	updater.close();
	SMainWindow::SSettings->setValue("Maj/remind", true);

	QDesktopServices::openUrl(QUrl(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/SNUpdater.exe"));
	close();
}

void MaJDialog::closeEvent(QCloseEvent * event)
{
	if (m_installingUpdate) {
		event->ignore();
		return;
	}
	if (!m_updateSuccess) {
		if (!m_remindMaj->isChecked()) {
			SMainWindow::SSettings->setValue("Maj/remind", false);
		}

		SMainWindow* fen{ new SMainWindow() };

		QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
		QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, SMainWindow::SSettings->value("preferences/enablePlugins", true).toBool());
		QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, SMainWindow::SSettings->value("preferences/enableJavascript", true).toBool());

		fen->show();
	}
}
