#include "includes/SWindows/SDownload.hpp"
#include "includes/SMainWindow.hpp"

#include <QFileInfo>

SDownloadItem::SDownloadItem(QWebEngineDownloadItem *download, SMainWindow *parent) :
    QDialog(parent),
    m_parent(parent),
    m_download(download)
{
    QFileInfo infos{ m_download->url().toString() };
    m_fileName = infos.fileName();

    m_boxBtn->setStandardButtons(QDialogButtonBox::Cancel);
    m_label->setText(m_fileName);

    m_layout->addWidget(m_label);
    m_layout->addWidget(m_progressBar);
    m_layout->addWidget(m_boxBtn);

    connect(m_download, &QWebEngineDownloadItem::downloadProgress, this, &SDownloadItem::progress);
    connect(m_download, &QWebEngineDownloadItem::finished, this, &SDownloadItem::finish);

    m_download->accept();
    connect(m_boxBtn, &QDialogButtonBox::rejected, this, &SDownloadItem::undo);
    connect(m_boxBtn, &QDialogButtonBox::accepted, this, &SDownloadItem::open);

	setAttribute(Qt::WA_DeleteOnClose);
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
        m_label->setText(m_label->text() + " - Annuler");
        m_boxBtn->setStandardButtons(QDialogButtonBox::Close);
        disconnect(m_boxBtn, &QDialogButtonBox::rejected, 0, 0);
        connect(m_boxBtn, &QDialogButtonBox::rejected, this, &SDownloadItem::close);
}

void SDownloadItem::open()
{
        QDesktopServices::openUrl(QString("file:///" + m_download->path()));
}

void SDownloadItem::finish()
{
    if(!m_dlCanceled) {
        m_dlFinished = true;
        m_label->setText(m_label->text() + " - Terminé");
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(100);
        m_boxBtn->setStandardButtons(QDialogButtonBox::Open);
    }
}
