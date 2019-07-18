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
#ifndef SIELO_BROWSER_COOKIEJAR_HPP
#define SIELO_BROWSER_COOKIEJAR_HPP

#include "SharedDefines.hpp"

#include <QVector>
#include <QStringList>

#include <QWebEngine/CookieStore.hpp>

namespace Sn {

class SIELO_SHAREDLIB CookieJar: public QObject {
Q_OBJECT

public:
	CookieJar(QObject* parent = nullptr);

	void loadSettings();

	void setAllowCookies(bool allow);

	void deleteCookie(const QNetworkCookie& cookie);

	QVector<QNetworkCookie> getAllCookies() const { return m_cookies; }
	void deleteAllCookies();

signals:
	void cookieAdded(const QNetworkCookie& cookie);
	void cookieRemoved(const QNetworkCookie& cookie);

protected:
	bool matchDomain(QString cookieDomain, QString siteDomain) const;
	bool listMatchesDomain(const QStringList& list, const QString& cookieDomain) const;

private:
	void sCookieAdded(const QNetworkCookie& cookie);
	void sCookieRemoved(const QNetworkCookie& cookie);

	bool acceptCookie(const QUrl& firstPartyUrl, const QByteArray& cookieLine, const QUrl& cookieSource) const;
	bool rejectCookie(const QString& domain, const QNetworkCookie& cookie, const QString& cookieDomain) const;

	bool m_allowCookies{};
	bool m_filterTrackingCookie{};
	bool m_filterThirdParty{};

	QStringList m_whiteList{};
	QStringList m_blackList{};

	Engine::CookieStore* m_client{nullptr};
	QVector<QNetworkCookie> m_cookies{};
};

}

#endif //SIELO_BROWSER_COOKIEJAR_HPP
