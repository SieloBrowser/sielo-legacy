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

#include "WebPage.hpp"

namespace Engine {

WebPage::WebPage(QObject* parent) :
	QWebEnginePage(parent)
{
	m_history = new WebHistory(QWebEnginePage::history());

	connect(this, &QWebEnginePage::fullScreenRequested, this, &WebPage::emitFullScreenRequest);
}

WebPage::WebPage(Engine::WebProfile* profile, QObject* parent) :
	QWebEnginePage(profile, parent)
{
	m_history = new WebHistory(QWebEnginePage::history());

	connect(this, &QWebEnginePage::fullScreenRequested, this, &WebPage::emitFullScreenRequest);
}

WebPage::~WebPage()
{
	delete m_history;
}

const ContextMenuData& WebPage::contextMenuData() const
{
	QWebEngineContextMenuData newData{QWebEnginePage::contextMenuData()};
	ContextMenuData contextMenuData{&newData};

	return contextMenuData;
}

WebHistory* WebPage::history() const
{
	return m_history;
}

WebView* WebPage::view() const
{
	return qobject_cast<WebView*>(QWebEnginePage::view());
}

QWebEnginePage* WebPage::createWindow(QWebEnginePage::WebWindowType type)
{
	return qobject_cast<QWebEnginePage*>(createNewWindow(type));
}

Engine::WebPage* WebPage::createNewWindow(QWebEnginePage::WebWindowType type)
{
	return qobject_cast<Engine::WebPage*>(QWebEnginePage::createWindow(type));
}

void WebPage::emitFullScreenRequest(QWebEngineFullScreenRequest request)
{
	FullScreenRequest newRequest{&request};

	emit fullScreenRequested(newRequest);
}

}