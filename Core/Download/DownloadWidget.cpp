/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2018 Victor DENIS (victordenis01@gmail.com)                      **
**                                                                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
***********************************************************************************/

#include "Download/DownloadWidget.hpp"

#include <math.h>

#include <QFileDialog>

#include <QDir>
#include <QStandardPaths>
#include <QDesktopServices>

#include "Utils/Settings.hpp"

#include "Widgets/EllipseLabel.hpp"

#include "Download/DownloadManager.hpp"

namespace Sn {

DownloadWidget::DownloadWidget(Engine::DownloadItem* download, QWidget* parent) :
	QWidget(parent),
	m_bytesReceived(0),
	m_download(download)
{
	setupUI();

	QPalette palette{m_downloadInfoLabel->palette()};
	palette.setColor(QPalette::Text, Qt::darkGray);

	m_downloadInfoLabel->setPalette(palette);

	m_progress->setMaximum(0);

	if (download) {
		m_file.setFile(download->path());
		m_url = download->url();

		connect(m_download.data(), &Engine::DownloadItem::downloadProgress, this, &DownloadWidget::downloadProgress);
		connect(m_download.data(), &Engine::DownloadItem::finished, this, &DownloadWidget::finished);
	}

	m_downloadInfoLabel->clear();
	m_progress->setValue(0);
	getFileName();

	m_downloadTime.start();

	m_buttonOpen->setEnabled(false);
	m_buttonOpen->hide();

	connect(m_buttonStop, &QPushButton::clicked, this, &DownloadWidget::stop);
	connect(m_buttonOpen, &QPushButton::clicked, this, &DownloadWidget::open);
}

bool DownloadWidget::downloading() const
{
	return m_buttonOpen->isVisible();
}

bool DownloadWidget::downloadedSuccessfully() const
{
	bool completed
		{m_download && m_download->isFinished() && m_download->state() == Engine::DownloadItem::DownloadCompleted};

	return completed || !m_buttonStop->isVisible();
}

bool DownloadWidget::getFileName(bool promptForFileName)
{
	Settings settings{};

	settings.beginGroup(QLatin1String("Download-Settings"));

	bool alwaysAsk{settings.value(QLatin1String("alwaysAsk"), true).toBool()};
	QString defaultLocation{QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)};

	if (m_file.absoluteDir().exists())
		defaultLocation = m_file.absolutePath();

	QString downloadDirectory{settings.value(QLatin1String("downloadDirectory"), defaultLocation).toString()};

	if (!downloadDirectory.isEmpty())
		downloadDirectory += QLatin1Char('/');

	QString defaultFileName{QFileInfo(downloadDirectory, m_file.fileName()).absoluteFilePath()};
	QString fileName{defaultFileName};

	if (promptForFileName || alwaysAsk) {
		fileName = QFileDialog::getSaveFileName(this, tr("Save File"), defaultFileName);
		if (fileName.isEmpty()) {
			if (m_download)
				m_download->cancel();

			m_fileNameLabel->setText(tr("Download canceled: %1").arg(QFileInfo(defaultFileName).fileName()));
			return false;
		}
	}

	m_file.setFile(fileName);

	if (m_download && m_download->state() == Engine::DownloadItem::DownloadRequested)
		m_download->setPath(m_file.absoluteFilePath());

	m_fileNameLabel->setText(m_file.fileName());

	return true;
}

void DownloadWidget::stop()
{
	setUpdatesEnabled(false);

	m_buttonStop->setEnabled(false);
	m_buttonStop->hide();

	setUpdatesEnabled(true);

	if (m_download) {
		m_download->cancel();
		m_buttonOpen->setEnabled(false);
	}

	emit statusChanged();
}

void DownloadWidget::open()
{
	QUrl url{QUrl::fromLocalFile(m_file.absoluteFilePath())};

	QDesktopServices::openUrl(url);
}

void DownloadWidget::downloadProgress(quint64 byteReceived, quint64 bytesTotal)
{
	m_bytesReceived = byteReceived;

	if (bytesTotal == -1) {
		m_progress->setValue(0);
		m_progress->setMaximum(0);
	}
	else {
		m_progress->setValue(byteReceived);
		m_progress->setMaximum(bytesTotal);
	}

	updateInfoLabel();
}

void DownloadWidget::finished()
{
	if (m_download) {
		Engine::DownloadItem::DownloadState state{m_download->state()};
		QString message{};
		bool interupted{false};

		switch (state) {
		case Engine::DownloadItem::DownloadRequested:
		case Engine::DownloadItem::DownloadInProgress:
			Q_UNREACHABLE();
			break;
		case Engine::DownloadItem::DownloadCompleted:
			break;
		case Engine::DownloadItem::DownloadCancelled:
			message = QStringLiteral("Download cancelled");
			interupted = true;
			break;
		case Engine::DownloadItem::DownloadInterrupted:
			message = QStringLiteral("Download interrupted");
			interupted = true;
			break;
		}

		if (interupted) {
			m_downloadInfoLabel->setText(message);
			m_buttonOpen->setEnabled(false);
			return;
		}
	}

	//m_progress->hide();
	m_progress->setTextVisible(true);
	m_progress->setFormat(tr("Download completed"));

	m_buttonOpen->setEnabled(true);
	m_buttonOpen->show();

	m_buttonStop->setEnabled(false);
	m_buttonStop->hide();

	updateInfoLabel();

	emit statusChanged();
}

void DownloadWidget::setupUI()
{
	resize(423, 110);
	setWindowTitle(tr("Form"));

	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(5, 0, 5, 0);

	m_layoutProgress = new QVBoxLayout();

	m_layoutButton = new QVBoxLayout();

	m_fileIcon = new QLabel(tr("ico"), this);

	QSizePolicy sizePolicy{QSizePolicy::Minimum, QSizePolicy::Minimum};
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(m_fileIcon->sizePolicy().hasHeightForWidth());

	m_fileNameLabel = new EllipseLabel(this);
	m_fileNameLabel->setProperty("text", QVariant(tr("Filename")));

	QSizePolicy sizePolicy1{QSizePolicy::Minimum, QSizePolicy::Minimum};
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(m_fileNameLabel->sizePolicy().hasHeightForWidth());

	m_fileNameLabel->setSizePolicy(sizePolicy1);

	m_fileIcon->setSizePolicy(sizePolicy);

	m_progress = new QProgressBar(this);
	m_progress->setValue(0);

	m_downloadInfoLabel = new EllipseLabel(this);

	QSizePolicy sizePolicy2{QSizePolicy::Minimum, QSizePolicy::Minimum};
	sizePolicy2.setHorizontalStretch(0);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(m_downloadInfoLabel->sizePolicy().hasHeightForWidth());

	m_downloadInfoLabel->setSizePolicy(sizePolicy2);

	m_spacerTop = new QSpacerItem(17, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_buttonStop = new QPushButton(tr("Stop"), this);

	m_buttonOpen = new QPushButton(tr("Open"), this);

	m_spacerBottom = new QSpacerItem(17, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_layoutProgress->addWidget(m_fileNameLabel);
	m_layoutProgress->addWidget(m_progress);
	m_layoutProgress->addWidget(m_downloadInfoLabel);

	m_layoutButton->addItem(m_spacerTop);
	m_layoutButton->addWidget(m_buttonStop);
	m_layoutButton->addWidget(m_buttonOpen);
	m_layoutButton->addItem(m_spacerBottom);

	m_layout->addWidget(m_fileIcon);
	m_layout->addLayout(m_layoutProgress);
	m_layout->addLayout(m_layoutButton);
}

void DownloadWidget::updateInfoLabel()
{
	quint64 byteTotal{static_cast<quint64>(m_progress->maximum())};
	double speed{m_bytesReceived * 1000.0 / m_downloadTime.elapsed()};
	double timeRemaining{(static_cast<double>(byteTotal - m_bytesReceived)) / speed};
	QString timeRemainingString{tr("seconds")};

	if (timeRemaining > 60) {
		timeRemaining = timeRemaining / 60;
		timeRemainingString = tr("minutes");
	}

	timeRemaining = floor(timeRemaining);

	if (timeRemaining == 0)
		timeRemaining = 1;

	QString info{};

	if (!downloadedSuccessfully()) {
		QString remaining{};

		if (byteTotal != 0) {
			remaining = tr("- %4 %5 remaining").arg(timeRemaining).arg(timeRemainingString);
			info = tr("%1 of %2 (%3/sec) %4").arg(dataString(m_bytesReceived))
				.arg(byteTotal == 0 ? tr("?") : dataString(byteTotal)).arg(dataString(
					static_cast<int>(speed))).arg(remaining);
		}
		else {
			if (m_bytesReceived != byteTotal)
				info = tr("%1 of %2 - Stopped").arg(dataString(m_bytesReceived)).arg(dataString(byteTotal));
			else
				info = dataString(m_bytesReceived);
		}
	}

	m_downloadInfoLabel->setText(info);
}

QString DownloadWidget::dataString(int size) const
{
	QString unit{};

	if (size < 1024)
		unit = tr("bytes");
	else if (size < 1024 * 1024) {
		size /= 1024;
		unit = tr("kB");
	}
	else {
		size /= 1024 * 1024;
		unit = tr("MB");
	}

	return QString(QLatin1String("%1 %2")).arg(size).arg(unit);
}

}
