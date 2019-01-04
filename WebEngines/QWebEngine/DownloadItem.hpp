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

#ifndef SIELO_BROWSER_DOWNLOADITEM_HPP
#define SIELO_BROWSER_DOWNLOADITEM_HPP

#include <QtWebEngineWidgets/QWebEngineDownloadItem>

#include <QUrl>

namespace Engine {
class DownloadItem : public QObject {
Q_OBJECT

public:
	enum DownloadState {
		DownloadRequested = 0,
		DownloadInProgress = 1,
		DownloadCompleted = 2,
		DownloadCancelled = 3,
		DownloadInterrupted = 4
	};

	DownloadItem(QWebEngineDownloadItem* item);
	~DownloadItem();

	bool isFinished() const;

	void accept();
	void cancel();
	void pause();
	void resume();

	void setPath(QString path);

	QString path() const;
	QUrl url() const;
	DownloadState state() const;

signals:
	void downloadProgress(quint64 bytesReceived, qint64 bytesTotal);
	void finished();

private slots:
	void emitDownloadProgress(quint64 bytesReceived, qint64 bytesTotal);
	void emitFinished();

private:
	QWebEngineDownloadItem* m_item{nullptr};
};
}

#endif //SIELO_BROWSER_DOWNLOADITEM_HPP
