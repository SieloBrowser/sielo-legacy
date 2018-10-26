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

#include "IconProvider.hpp"

#include <QBuffer>

#include <QSqlQuery>

#include "Database/SqlDatabase.hpp"

#include "Utils/AutoSaver.hpp"

#include "Web/WebView.hpp"

#include "Application.hpp"

Q_GLOBAL_STATIC(Sn::IconProvider, sn_icon_provider)

namespace Sn
{
QByteArray IconProvider::encodeUrl(const QUrl& url)
{
	return url.toEncoded(QUrl::RemoveFragment | QUrl::StripTrailingSlash);
}

IconProvider::IconProvider() :
	QObject(),
	m_autoSaver(new AutoSaver(this))
{
	// Empty
}

IconProvider::~IconProvider()
{
	// Empty
}

void IconProvider::saveIcon(WebView* view)
{
	// Don't save icons in private mode.
	if (Application::instance()->privateBrowsing())
		return;

	const QIcon icon{view->icon(true)};
	if (icon.isNull()) // We don't want to store nulle icon
		return;

	// Some schem must be ignored beceause we don't want to save the icon
	const QStringList ignoredSchemes = {
		QStringLiteral("sielo"),
		QStringLiteral("ftp"),
		QStringLiteral("file"),
		QStringLiteral("view-source")
	};

	if (ignoredSchemes.contains(view->url().scheme()))
		return;

	// We should not save an icon twice
	for (int i{0}; i < m_iconBuffer.size(); ++i) {
		if (m_iconBuffer[i].first == view->url()) {
			m_iconBuffer.removeAt(i);
			break;
		}
	}

	BufferedIcon item{};
	item.first = view->url();
	item.second = icon.pixmap(16).toImage();

	m_autoSaver->changeOccurred();
	m_iconBuffer.append(item);
}

QIcon IconProvider::iconForUrl(const QUrl& url, bool allowNull)
{
	return instance()->iconFromImage(imageForUrl(url, allowNull));
}

QImage IconProvider::imageForUrl(const QUrl& url, bool allowNull)
{
	if (url.path().isEmpty())
		return allowNull ? QImage() : Application::getAppIcon("webpage").pixmap(16).toImage();

	const QByteArray encodedUrl = encodeUrl(url);

	// Check if we aleady have the image loaded in the buffer
	foreach(const BufferedIcon &ic, instance()->m_iconBuffer)
	{
		if (encodeUrl(ic.first) == encodedUrl)
			return ic.second;
	}

	QString urlString{QString::fromUtf8(encodedUrl)};
	urlString.replace(QLatin1Char('['), QStringLiteral("[["));
	urlString.replace(QLatin1Char(']'), QStringLiteral("[]]"));
	urlString.replace(QStringLiteral("[["), QStringLiteral("[[]"));
	urlString.replace(QLatin1Char('*'), QStringLiteral("[*]"));
	urlString.replace(QLatin1Char('?'), QStringLiteral("[?]"));

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT icon FROM icons WHERE url GLOB ? LIMIT 1");
	query.addBindValue(QString("%1*").arg(urlString));
	query.exec();

	if (query.next())
		return QImage::fromData(query.value(0).toByteArray());

	return allowNull ? QImage() : Application::getAppIcon("webpage").pixmap(16).toImage();
}

QIcon IconProvider::iconForDomain(const QUrl& url, bool allowNull)
{
	return instance()->iconFromImage(imageForDomain(url, allowNull));
}
QImage IconProvider::imageForDomain(const QUrl& url, bool allowNull)
{
	if (url.path().isEmpty())
		return allowNull ? QImage() : Application::getAppIcon("webpage").pixmap(16).toImage();

	// Check if we aleady have the image loaded in the buffer
	foreach(const BufferedIcon &ic, instance()->m_iconBuffer)
	{
		if (ic.first.host() == url.host())
			return ic.second;
	}

	QString urlString{url.host()};
	urlString.replace(QLatin1Char('['), QStringLiteral("[["));
	urlString.replace(QLatin1Char(']'), QStringLiteral("[]]"));
	urlString.replace(QStringLiteral("[["), QStringLiteral("[[]"));
	urlString.replace(QLatin1Char('*'), QStringLiteral("[*]"));
	urlString.replace(QLatin1Char('?'), QStringLiteral("[?]"));

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT icon FROM icons WHERE url GLOB ? LIMIT 1");
	query.addBindValue(QString("*%1*").arg(urlString));
	query.exec();

	if (query.next())
		return QImage::fromData(query.value(0).toByteArray());

	return allowNull ? QImage() : Application::getAppIcon("webpage").pixmap(16).toImage();
}
IconProvider *IconProvider::instance()
{
	return sn_icon_provider();

}
void IconProvider::save()
{
	foreach(const BufferedIcon &ic, m_iconBuffer)
	{

		QSqlQuery query{SqlDatabase::instance()->database()};
		query.prepare("SELECT id FROM icons WHERE url = ?");
		query.bindValue(0, encodeUrl(ic.first));
		query.exec();

		if (query.next())
			query.prepare("UPDATE icons SET icon = ? WHERE url = ?");
		else
			query.prepare("INSERT INTO icons (icon, url) VALUES (?,?)");

		QByteArray ba{};
		QBuffer buffer(&ba);

		buffer.open(QIODevice::WriteOnly);
		ic.second.save(&buffer, "PNG");

		query.bindValue(0, buffer.data());
		query.bindValue(1, QString::fromUtf8(encodeUrl(ic.first)));
		query.exec();
	}

	m_iconBuffer.clear();
}
QIcon IconProvider::iconFromImage(const QImage& image)
{
	return QIcon(QPixmap::fromImage(image));
}
}
