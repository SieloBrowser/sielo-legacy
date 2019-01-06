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

#include "WebProfile.hpp"

#include "WebSettings.hpp"
#include "CookieStore.hpp"

#include <QtWebEngineWidgets/QWebEngineScript>
#include <QtWebEngineWidgets/QWebEngineScriptCollection>

namespace Engine {

WebProfile* WebProfile::s_defaultProfile = nullptr;

WebProfile* WebProfile::defaultWebProfile()
{
	if (!WebProfile::s_defaultProfile) {
		WebProfile::s_defaultProfile = static_cast<WebProfile*>(QWebEngineProfile::defaultProfile());

		connect(WebProfile::s_defaultProfile, &QWebEngineProfile::downloadRequested, WebProfile::s_defaultProfile, &WebProfile::emitDownloadRequest);
	}

	return WebProfile::s_defaultProfile;
}

WebProfile::WebProfile(QObject* parent) :
	QWebEngineProfile(parent)
{
	connect(this, &QWebEngineProfile::downloadRequested, this, &WebProfile::emitDownloadRequest);
}

void WebProfile::insertScript(QString name, QString source, Engine::WebProfile::ScriptInjectionPoint injectionPoint,
							  Engine::WebProfile::ScriptWorldId worldId, bool runsOnSubFrames)
{
	QWebEngineScript script{};

	script.setName(name);
	script.setInjectionPoint(static_cast<QWebEngineScript::InjectionPoint>(injectionPoint));
	script.setWorldId(static_cast<QWebEngineScript::ScriptWorldId>(worldId));
	script.setRunsOnSubFrames(runsOnSubFrames);
	script.setSourceCode(source);

	scripts()->insert(script);
}

WebSettings* WebProfile::settings() const
{
	return new WebSettings(QWebEngineProfile::settings());
}

CookieStore* WebProfile::cookieStore()
{
	return new CookieStore(QWebEngineProfile::cookieStore());
}

void WebProfile::emitDownloadRequest(QWebEngineDownloadItem* download)
{
	emit downloadRequested(new DownloadItem(download));
}

}
