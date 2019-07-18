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

#pragma once
#ifndef SIELOBROWSER_DOWNLOADWIDGET_HPP
#define SIELOBROWSER_DOWNLOADWIDGET_HPP

#include "SharedDefines.hpp"

#include <QFileInfo>

#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSpacerItem>

#include <QTime>
#include <QScopedPointer>

#include <QUrl>

#include <QWebEngine/DownloadItem.hpp>

namespace Sn {
class DownloadManager;

class EllipseLabel;

class SIELO_SHAREDLIB DownloadWidget: public QWidget {
Q_OBJECT

public:
	DownloadWidget(Engine::DownloadItem* download, QWidget* parent = nullptr);

	bool downloading() const;
	bool downloadedSuccessfully() const;

	bool getFileName(bool promptForFileName = false);

	EllipseLabel* downloadInfoLabel() const { return m_downloadInfoLabel; }

signals:
	void statusChanged();

private slots:
	void stop();
	void open();

	void downloadProgress(quint64 byteReceived, quint64 bytesTotal);
	void finished();

private:
	void setupUI();

	void updateInfoLabel();
	QString dataString(int size) const;

	QUrl m_url{};
	QFileInfo m_file{};
	quint64 m_bytesReceived{0};
	QTime m_downloadTime{};
	bool m_stopped{};

	QScopedPointer<Engine::DownloadItem> m_download;

	QHBoxLayout* m_layout{nullptr};
	QVBoxLayout* m_layoutProgress{nullptr};
	QVBoxLayout* m_layoutButton{nullptr};
	EllipseLabel* m_fileNameLabel{nullptr};
	QProgressBar* m_progress{nullptr};
	QLabel* m_fileIcon{nullptr};
	EllipseLabel* m_downloadInfoLabel{nullptr};
	QSpacerItem* m_spacerTop{nullptr};
	QPushButton* m_buttonStop{nullptr};
	QPushButton* m_buttonOpen{nullptr};
	QSpacerItem* m_spacerBottom{nullptr};

	friend class DownloadManager;
};

}

#endif //SIELOBROWSER_DOWNLOADWIDGET_HPP
