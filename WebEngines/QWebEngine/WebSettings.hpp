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

#ifndef SIELO_BROWSER_WEBSETTINGS_HPP
#define SIELO_BROWSER_WEBSETTINGS_HPP

#include "SharedDefines.hpp"

#include <QObject>

#include <QtWebEngineWidgets/QWebEngineSettings>

namespace Engine {
class SIELO_SHAREDLIB WebSettings: public QObject {
Q_OBJECT

public:
	enum WebAttribute {
		AutoLoadImages = 0,
		JavascriptEnabled = 1,
		JavascriptCanOpenWindows = 2,
		JavascriptCanAccessClipboard = 3,
		LinksIncludedInFocusChain = 4,
		LocalStorageEnabled = 5,
		LocalContentCanAccessRemoteUrls = 6,
		XSSAuditingEnabled = 7,
		SpatialNavigationEnabled = 8,
		LocalContentCanAccessFileUrls = 9,
		HyperlinkAuditingEnabled = 10,
		ScrollAnimatorEnabled = 11,
		ErrorPageEnabled = 12,
		PluginsEnabled = 13,
		FullScreenSupportEnabled = 14,
		ScreenCaptureEnabled = 15,
		WebGLEnabled = 16,
		Accelerated2dCanvasEnabled = 17,
		AutoLoadIconsForPage = 18,
		TouchIconsEnabled = 19,
		FocusOnNavigationEnabled = 20,
		PrintElementBackgrounds = 21,
		AllowRunningInsecureContent = 22,
		AllowGeolocationOnInsecureOrigins = 23,
		AllowWindowActivationFromJavaScript = 24,
		ShowScrollBars = 25,
		PlaybackRequiresUserGesture = 26,
		WebRTCPublicInterfacesOnly = 27,
		JavascriptCanPaste = 28,
		DnsPrefetchEnabled = 29
	};

	enum FontFamily {
		StandardFont = 0,
		FixedFont = 1,
		SerifFont = 2,
		SansSerifFont = 3,
		CursiveFont = 4,
		FantasyFont = 5,
		PictographFont = 6
	};

	WebSettings();
	WebSettings(QWebEngineSettings* settings);
	~WebSettings() = default;

	void setAttribute(WebAttribute attribute, bool on);

private:
	QWebEngineSettings* m_settings{nullptr};
};
}


#endif //SIELO_BROWSER_WEBSETTINGS_HPP
