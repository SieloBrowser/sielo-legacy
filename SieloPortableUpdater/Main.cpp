#include <QApplication>

#include <QMessageBox>
#include <QWidget>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QGroupBox>
#include <QCloseEvent>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QDesktopServices>
#include <QFile>
#include <QDir>

class SUpdateWidget : public QWidget
{
private:
	bool m_updateIsProgress{ true };

	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QGroupBox *m_box{ new QGroupBox(this) };
	QVBoxLayout *m_boxLayout{ new QVBoxLayout(m_box) };

	QProgressBar *m_progress{ new QProgressBar(m_box) };

	QNetworkReply *m_reply{};
	QNetworkAccessManager m_netManager{};

public:
	SUpdateWidget(QWidget *parent = nullptr) :
		QWidget(parent)
	{
		setWindowTitle(tr("Mise à jour"));
		m_box->setTitle(tr("Téléchargement de la mise à jour..."));

		m_layout->addWidget(m_box);
		m_boxLayout->addWidget(m_progress);

		m_reply = m_netManager.get(QNetworkRequest(QUrl("http://feldrise.com/Sielo/SieloNavigateurV3-WebEngine.exe")));
		connect(m_reply, &QNetworkReply::downloadProgress, this, &SUpdateWidget::downloadProgress);
		connect(m_reply, &QNetworkReply::finished, this, &SUpdateWidget::save);
	}

	~SUpdateWidget() {}

public slots:
	void downloadProgress(quint64 bytesReceived, quint16 bytesTotal) 
	{
		if (bytesTotal != -1) {
			m_progress->setRange(0, bytesTotal);
			m_progress->setValue(bytesReceived);
		}
	}

	void save() 
	{
		m_updateIsProgress = false;
		m_reply->deleteLater();
		QFile updater{ QDir(QCoreApplication::applicationDirPath()).absolutePath() + "/SieloNavigateurV3-WebEngine.exe" };

		if (!updater.open(QIODevice::WriteOnly)) {
			QMessageBox::critical(this, tr("Erreur"), tr("Erreur lors de l'installation de la mise à jours !"));
			return;
		}

		updater.write(m_reply->readAll());
		updater.close();

		QDesktopServices::openUrl(QUrl(QDir(QCoreApplication::applicationDirPath()).absolutePath() + "/SieloNavigateurV3-WebEngine.exe"));
		close();
	}

protected:
	virtual void closeEvent(QCloseEvent *event)
	{
		if (m_updateIsProgress) 
			event->ignore();
		else
			event->accept();
	}

};

int main(int argc, char *argv[]) {
	QApplication app{ argc, argv };
	SUpdateWidget *update{ new SUpdateWidget(nullptr) };
	update->show();

	return app.exec();
}
