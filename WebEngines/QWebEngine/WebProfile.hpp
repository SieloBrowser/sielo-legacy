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

#ifndef SIELO_BROWSER_WEBPROFILE_HPP
#define SIELO_BROWSER_WEBPROFILE_HPP

#include "SharedDefines.hpp"
#include "DownloadItem.hpp"

#include <QtWebEngineWidgets/QWebEngineProfile>

namespace Engine {
class WebSettings;

class CookieStore;

class SIELO_SHAREDLIB WebProfile : public QWebEngineProfile {
Q_OBJECT

public:
	enum ScriptInjectionPoint {
		Deferred = 0,
		DocumentReady = 1,
		DocumentCreation = 2
	};

	enum ScriptWorldId {
		MainWorld = 0,
		ApplicationWorld = 1,
		UserWorld
	};

	WebProfile(QObject* parent = nullptr);
	~WebProfile() = default;

	void insertScript(QString name, QString source, ScriptInjectionPoint injectionPoint, ScriptWorldId worldId,
					  bool runsOnSubFrames);

	WebSettings* settings() const;
	CookieStore* cookieStore();

	static WebProfile* defaultWebProfile();
	static WebProfile* s_defaultProfile;

signals:
	void downloadRequested(Engine::DownloadItem* item);

private slots:
	void emitDownloadRequest(QWebEngineDownloadItem* download);
};
}

#endif //SIELO_BROWSER_WEBPROFILE_HPP
