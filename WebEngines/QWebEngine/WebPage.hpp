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

#ifndef SIELO_BROWSER_WEBPAGE_HPP
#define SIELO_BROWSER_WEBPAGE_HPP

#include "SharedDefines.hpp"

#include <QObject>
#include <QtWebEngineWidgets/QWebEnginePage>

#include "ContextMenuData.hpp"
#include "FullScreenRequest.hpp"
#include "WebView.hpp"
#include "WebProfile.hpp"

namespace Engine {
class SIELO_SHAREDLIB WebPage : public QWebEnginePage {
Q_OBJECT

public:
	WebPage(QObject* parent = nullptr);
	WebPage(WebProfile* profile, QObject* parent = nullptr);
	~WebPage();

	const ContextMenuData& contextMenuData() const;
	WebHistory* history() const;
	WebView* view() const;

	QWebEnginePage* createWindow(WebWindowType type) Q_DECL_OVERRIDE;
	virtual Engine::WebPage* createNewWindow(WebWindowType type);

signals:
	void fullScreenRequested(FullScreenRequest& request);

private slots:
	void emitFullScreenRequest(QWebEngineFullScreenRequest request);

private:
	WebHistory* m_history{nullptr};
};
}

#endif //SIELO_BROWSER_WEBPAGE_HPP
