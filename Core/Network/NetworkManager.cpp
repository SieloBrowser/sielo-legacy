/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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

#include "Network/NetworkManager.hpp"

#include <QNetworkRequest>

#include "Application.hpp"

#include "Network/BaseUrlInterceptor.hpp"
#include <Network/NetworkUrlInterceptor.hpp>

namespace Sn {

NetworkManager::NetworkManager(QObject* parent) :
	QNetworkAccessManager(parent)
{
	m_urlInterceptor = new NetworkUrlInterceptor(this);
	Application::instance()->webProfile()->setRequestInterceptor(m_urlInterceptor);
}

void NetworkManager::installUrlInterceptor(BaseUrlInterceptor* interceptor)
{
	m_urlInterceptor->installUrlInterceptor(interceptor);
}

void NetworkManager::removeUrlInterceptor(BaseUrlInterceptor* interceptor)
{
	m_urlInterceptor->removeUrlInterceptor(interceptor);
}

void NetworkManager::loadSettings()
{
	m_urlInterceptor->loadSettings();
}

QNetworkReply* NetworkManager::createRequest(Operation operation, const QNetworkRequest& request,
											 QIODevice* outgoingData)
{
	QNetworkRequest req = request;
	req.setAttribute(QNetworkRequest::SpdyAllowedAttribute, true);
	req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

	return QNetworkAccessManager::createRequest(operation, req, outgoingData);
}

}