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

#include "Cookies/CookieJar.hpp"

#include "Utils/Settings.hpp"

#include "Application.hpp"

namespace Sn {

CookieJar::CookieJar(QObject* parent) :
	QObject(parent),
	m_client(Application::instance()->webProfile()->cookieStore())
{
	loadSettings();
	m_client->loadAllCookies();

	connect(m_client, &Engine::CookieStore::cookieAdded, this, &CookieJar::sCookieAdded);
	connect(m_client, &Engine::CookieStore::cookieRemoved, this, &CookieJar::sCookieRemoved);
}

void CookieJar::loadSettings()
{
	Settings settings{};

	settings.beginGroup("Cookie-Settings");

	m_allowCookies = settings.value("allowCookies", true).toBool();
	m_filterThirdParty = settings.value("filterThirdPartyCookies", false).toBool();
	m_filterTrackingCookie = settings.value("filterTrackingCookies", false).toBool();
	m_whiteList = settings.value("whiteList", QStringList()).toStringList();
	m_blackList = settings.value("blackList", QStringList()).toStringList();

	settings.endGroup();
}

void CookieJar::setAllowCookies(bool allow)
{
	m_allowCookies = allow;
}

void CookieJar::deleteCookie(const QNetworkCookie& cookie)
{
	m_client->deleteCookie(cookie);
}

void CookieJar::deleteAllCookies()
{
	m_client->deleteAllCookies();
}

bool CookieJar::matchDomain(QString cookieDomain, QString siteDomain) const
{
	if (cookieDomain.startsWith(QLatin1Char('.')))
		cookieDomain = cookieDomain.mid(1);

	if (siteDomain.startsWith(QLatin1Char('.')))
		siteDomain = siteDomain.mid(1);

	if (cookieDomain == siteDomain)
		return true;

	if (!siteDomain.endsWith(cookieDomain))
		return false;

	return siteDomain.indexOf(cookieDomain) > 0 && siteDomain[siteDomain.indexOf(cookieDomain) - 1] == QLatin1Char('.');
}

bool CookieJar::listMatchesDomain(const QStringList& list, const QString& cookieDomain) const
{
		foreach (const QString& domain, list) {
			if (matchDomain(domain, cookieDomain))
				return true;
		}

	return false;
}

void CookieJar::sCookieAdded(const QNetworkCookie& cookie)
{
	if (rejectCookie(QString(), cookie, cookie.domain())) {
		m_client->deleteCookie(cookie);
		return;
	}

	m_cookies.append(cookie);

	emit cookieAdded(cookie);
}

void CookieJar::sCookieRemoved(const QNetworkCookie& cookie)
{
	if (m_cookies.removeOne(cookie))
		emit cookieRemoved(cookie);
}

bool CookieJar::acceptCookie(const QUrl& firstPartyUrl, const QByteArray& cookieLine, const QUrl& cookieSource) const
{
	const QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(cookieLine);
	Q_ASSERT(cookies.size() == 1);

	const QNetworkCookie cookie{cookies[0]};

	return !rejectCookie(firstPartyUrl.host(), cookie, cookieSource.host());
}

bool CookieJar::rejectCookie(const QString& domain, const QNetworkCookie& cookie, const QString& cookieDomain) const
{
	if (!m_allowCookies) {
		bool result{listMatchesDomain(m_whiteList, cookieDomain)};

		if (!result) {
			return true;
		}
	}

	if (m_allowCookies) {
		bool result{listMatchesDomain(m_blackList, cookieDomain)};

		if (result)
			return true;
	}

	if (m_filterThirdParty) {
		bool result{matchDomain(cookieDomain, domain)};

		if (!result)
			return true;
	}

	if (m_filterTrackingCookie && cookie.name().startsWith("__utm"))
		return true;

	return false;
}

}