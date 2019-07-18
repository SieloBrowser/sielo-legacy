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

#include "WebInspector.hpp"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QNetworkReply>

#include "Network/NetworkManager.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn {
WebInspector::WebInspector(QWidget* parent) :
	QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName("web-inspector");
	setMinimumHeight(80);

	setupUI();

	connect(m_closeButton, &QPushButton::clicked, this, &WebInspector::close);
}

void WebInspector::setView(Engine::WebView* view, bool inspectElement)
{
	m_inspectedView = view;
	m_view->page()->setInspectedPage(view->page());

	if (inspectElement)
		view->triggerPageAction(QWebEnginePage::InspectElement);
}


void WebInspector::setupUI()
{
	m_layout = new QVBoxLayout(this);

	m_closeButton = new QPushButton("X", this);
	m_closeButton->setObjectName("web-inspector-btn-close");

	m_view = new Engine::WebView(this);

	m_layout->addWidget(m_closeButton);
	m_layout->addWidget(m_view);

	m_layout->setAlignment(m_closeButton, Qt::AlignRight);
}
}
