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

#include "UrlRequestInfo.hpp"

namespace Engine {

UrlRequestInfo::UrlRequestInfo(QWebEngineUrlRequestInfo* request) :
	QObject(),
	m_request(request)
{
	// Empty
}

UrlRequestInfo::ResourceType UrlRequestInfo::resourceType() const
{
	return static_cast<ResourceType>(m_request->resourceType());
}

QUrl UrlRequestInfo::requestUrl() const
{
	return m_request->requestUrl();
}

void UrlRequestInfo::redirect(const QUrl& url)
{
	m_request->redirect(url);
}

void UrlRequestInfo::block(bool shouldBlock)
{
	m_request->block(shouldBlock);
}

QUrl UrlRequestInfo::firstPartyUrl() const
{
	return m_request->firstPartyUrl();
}

void UrlRequestInfo::setHttpHeader(const QByteArray& name, const QByteArray& value)
{
	m_request->setHttpHeader(name, value);
}
}