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

#include "Web/WebPage.hpp"

#include <QPointer>
#include <QTimer>
#include <QWebEngineSettings>
#include <QDesktopServices>

#include "Web/WebHitTestResult.hpp"
#include "Web/WebView.hpp"

#include "Utils/CheckBoxDialog.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Application.hpp"

namespace Sn {
WebPage::WebPage(QObject* parent) :
	QWebEnginePage(parent),
	m_runningLoop(nullptr),
	m_loadProgress(-1),
	m_blockAlerts(false),
	m_secureStatus(false),
	m_adjustingSheduled(false)
{
	connect(this, &QWebEnginePage::loadProgress, this, &WebPage::progress);
	connect(this, &QWebEnginePage::loadFinished, this, &WebPage::finished);
	connect(this, &QWebEnginePage::urlChanged, this, &WebPage::urlChanged);
	connect(this, &QWebEnginePage::featurePermissionRequested, this, &WebPage::featurePermissionRequested);
	connect(this, &QWebEnginePage::windowCloseRequested, this, &WebPage::windowCloseRequested);
	connect(this, &QWebEnginePage::fullScreenRequested, this, &WebPage::fullScreenRequested);
}

WebPage::~WebPage()
{
}
}