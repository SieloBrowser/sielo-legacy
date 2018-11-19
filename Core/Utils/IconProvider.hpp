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
#ifndef SIELOBROWSER_ICONPROVIDER_HPP
#define SIELOBROWSER_ICONPROVIDER_HPP

#include "SharedDefines.hpp"

#include <QIcon>
#include <QImage>

#include <QPair>

#include <QUrl>

namespace Sn
{
using BufferedIcon = QPair<QUrl, QImage>;
class AutoSaver;

class WebView;

class SIELO_SHAREDLIB IconProvider: public QObject {
	Q_OBJECT

public:
	static QByteArray encodeUrl(const QUrl& url);

	IconProvider();
	~IconProvider();

	void saveIcon(WebView* view);

	// Icon for url (only available for urls in history)
	static QIcon iconForUrl(const QUrl &url, bool allowNull = false);
	static QImage imageForUrl(const QUrl &url, bool allowNull = false);

	// Icon for domain (only available for urls in history)
	static QIcon iconForDomain(const QUrl &url, bool allowNull = false);
	static QImage imageForDomain(const QUrl &url, bool allowNull = false);

	static IconProvider* instance();

public slots:
	void save();

private:
	QIcon iconFromImage(const QImage &image);

	QVector<BufferedIcon> m_iconBuffer;

	AutoSaver* m_autoSaver;
};
}

#endif //SIELOBROWSER_ICONPROVIDER_HPP
