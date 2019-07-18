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

#include "Network/NetworkUrlInterceptor.hpp"

#include <QList>

#include "Utils/Settings.hpp"

#include "Network/BaseUrlInterceptor.hpp"

namespace Sn {

NetworkUrlInterceptor::NetworkUrlInterceptor(QObject* parent) :
	Engine::UrlRequestInterceptor(parent),
	m_sendDNT(false)
{
	// Empty
}

void NetworkUrlInterceptor::interceptUrlRequest(Engine::UrlRequestInfo& info)
{
	if (m_sendDNT)
		info.setHttpHeader(QByteArrayLiteral("DNT"), QByteArrayLiteral("1"));

		foreach (BaseUrlInterceptor* interceptor, m_interceptors) interceptor->interceptRequest(info);
}

void NetworkUrlInterceptor::installUrlInterceptor(BaseUrlInterceptor* interceptor)
{
	if (!m_interceptors.contains(interceptor))
		m_interceptors.append(interceptor);
}

void NetworkUrlInterceptor::removeUrlInterceptor(BaseUrlInterceptor* interceptor)
{
	m_interceptors.removeOne(interceptor);
}

void NetworkUrlInterceptor::loadSettings()
{
	Settings settings{};

	settings.beginGroup("Web-Settings");

	m_sendDNT = settings.value("DoNotTrack", false).toBool();

	settings.endGroup();
}

}