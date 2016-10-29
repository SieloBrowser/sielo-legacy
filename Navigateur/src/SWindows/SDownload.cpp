#include "includes/SWindows/SDownload.hpp"
#include "includes/SMainWindow.hpp"

#include <QFileInfo>

SDownloadItem::SDownloadItem(QWebEngineDownloadItem *download, SMainWindow *parent) :
	QDialog(parent),
	m_parent(parent),
	m_download(download)
{
	// Info about download
	QFileInfo infos{ m_download->url().toString() };
	m_fileName = infos.fileName();

	// Set attributes for widgets
	setAttribute(Qt::WA_DeleteOnClose);
	m_boxBtn->setStandardButtons(QDialogButtonBox::Cancel);
	m_label->setText(m_fileName);

	// Add all widgets in layout
	m_layout->addWidget(m_label);
	m_layout->addWidget(m_progressBar);
	m_layout->addWidget(m_boxBtn);

	// Connection
	connect(m_download, &QWebEngineDownloadItem::downloadProgress, this, &SDownloadItem::progress);
	connect(m_download, &QWebEngineDownloadItem::finished, this, &SDownloadItem::finish);

	connect(m_boxBtn, &QDialogButtonBox::rejected, this, &SDownloadItem::undo);
	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &SDownloadItem::open);

	m_download->accept();
	QMessageBox::information(m_parent, tr("Téléchargement"), tr("Vous avez lancé le téléchargement de ") + m_fileName);
}

SDownloadItem::~SDownloadItem()
{
	if(!m_dlFinished)
		m_download->cancel();

}



void SDownloadItem::progress(qint64 recept, qint64 total)
{
	if(total != -1) {
		m_progressBar->setRange(0, total);
		m_progressBar->setValue(recept);
	}

	return;
}

void SDownloadItem::undo()
{

		m_dlCanceled = true;
		m_dlFinished = true;
		m_download->cancel();
		m_progressBar->setValue(0);
		m_label->setText(m_label->text() + tr(" - Annuler"));
		disconnect(m_boxBtn, &QDialogButtonBox::rejected, 0, 0);

}

void SDownloadItem::open()
{
		QDesktopServices::openUrl(QString("file:///" + m_download->path()));
}

void SDownloadItem::finish()
{
	if(!m_dlCanceled) {
		m_dlFinished = true;
		m_label->setText(m_label->text() + tr(" - Terminé"));
		m_progressBar->setRange(0, 100);
		m_progressBar->setValue(100);
		m_boxBtn->setStandardButtons(QDialogButtonBox::Open);
	}
}
