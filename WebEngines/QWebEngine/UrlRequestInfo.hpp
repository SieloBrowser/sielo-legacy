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

#ifndef SIELO_BROWSER_URLREQUESTINFO_HPP
#define SIELO_BROWSER_URLREQUESTINFO_HPP

#include "SharedDefines.hpp"

#include <QObject>

#include <QtWebEngineCore/QWebEngineUrlRequestInfo>

namespace Engine {
class SIELO_SHAREDLIB UrlRequestInfo: public QObject {
Q_OBJECT

public:
	enum ResourceType {
		ResourceTypeMainFrame = 0,
		ResourceTypeSubFrame = 1,
		ResourceTypeStylesheet = 2,
		ResourceTypeScript = 3,
		ResourceTypeImage = 4,
		ResourceTypeFontResource = 5,
		ResourceTypeSubResource = 6,
		ResourceTypeObject = 7,
		ResourceTypeMedia = 8,
		ResourceTypeWorker = 9,
		ResourceTypeSharedWorker = 10,
		ResourceTypePrefetch = 11,
		ResourceTypeFavicon = 12,
		ResourceTypeXhr = 13,
		ResourceTypePing = 14,
		ResourceTypeServiceWorker = 15,
		ResourceTypeCspReport = 16,
		ResourceTypePluginResource = 17,
		ResourceTypeUnknown = 255
	};

	UrlRequestInfo(QWebEngineUrlRequestInfo* request);
	~UrlRequestInfo() = default;

	void redirect(const QUrl& url);
	void block(bool shouldBlock);

	QUrl firstPartyUrl() const;
	QUrl requestUrl() const;
	ResourceType resourceType() const;

	void setHttpHeader(const QByteArray& name, const QByteArray& value);
private:
	QWebEngineUrlRequestInfo* m_request{nullptr};
};
}

#endif //SIELO_BROWSER_URLREQUESTINFO_HPP
