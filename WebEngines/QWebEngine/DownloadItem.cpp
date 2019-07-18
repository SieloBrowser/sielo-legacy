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

#include "DownloadItem.hpp"

namespace Engine {
DownloadItem::DownloadItem(QWebEngineDownloadItem* item) :
		QObject(),
		m_item(item)
{
	connect(m_item, &QWebEngineDownloadItem::downloadProgress, this, &DownloadItem::emitDownloadProgress);
	connect(m_item, &QWebEngineDownloadItem::finished, this, &DownloadItem::emitFinished);
}

DownloadItem::~DownloadItem()
{
	delete m_item;
}

bool DownloadItem::isFinished() const
{
	return m_item->isFinished();
}

void DownloadItem::accept()
{
	m_item->accept();
}

void DownloadItem::cancel()
{
	m_item->cancel();
}

void DownloadItem::pause()
{
	m_item->pause();
}

void DownloadItem::resume()
{
	m_item->resume();
}

DownloadItem::DownloadState DownloadItem::state() const
{
	return static_cast<DownloadState>(m_item->state());
}

void DownloadItem::setPath(QString path)
{
	m_item->setPath(path);
}

QString DownloadItem::path() const
{
	return m_item->path();
}

QUrl DownloadItem::url() const {
	return m_item->url();
}

void DownloadItem::emitDownloadProgress(quint64 bytesReceived, qint64 bytesTotal)
{
	emit downloadProgress(bytesReceived, bytesTotal);
}

void DownloadItem::emitFinished()
{
	emit finished();
}
}