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

#include "CookieStore.hpp"

namespace Engine {

CookieStore::CookieStore(QWebEngineCookieStore* store) :
		QObject(),
		m_store(store)
{
	connect(m_store, &QWebEngineCookieStore::cookieAdded, this, &CookieStore::emitCookieAdded);
	connect(m_store, &QWebEngineCookieStore::cookieRemoved, this, &CookieStore::emitCookieRemoved);
}

CookieStore::~CookieStore()
{
	delete m_store;
}

void CookieStore::deleteCookie(const QNetworkCookie& cookie, const QUrl& origin)
{
	m_store->deleteCookie(cookie, origin);
}

void CookieStore::deleteAllCookies()
{
	m_store->deleteAllCookies();
}

void CookieStore::loadAllCookies()
{
	m_store->loadAllCookies();
}

void CookieStore::emitCookieAdded(const QNetworkCookie& cookie)
{
	emit cookieAdded(cookie);
}

void CookieStore::emitCookieRemoved(const QNetworkCookie& cookie)
{
	emit cookieRemoved(cookie);
}

}