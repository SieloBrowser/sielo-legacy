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

#include "WebInspector.hpp"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QNetworkReply>

#include "Network/NetworkManager.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn {
QList<QWebEngineView*> WebInspector::s_views;

WebInspector::WebInspector(QWidget* parent) :
	QWebEngineView(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName("web-inspector");
	setMinimumHeight(80);

	registerView(this);

	connect(page(), &QWebEnginePage::windowCloseRequested, this, &WebInspector::deleteLater);
	connect(page(), &QWebEnginePage::loadFinished, this, &WebInspector::loadFinished);
}

WebInspector::~WebInspector()
{
	unregisterView(this);
	// Empty
}

void WebInspector::setView(QWebEngineView* view)
{
	m_view = view;
	Q_ASSERT(isEnabled());

	int port = qEnvironmentVariableIntValue("QTWEBENGINE_REMOTE_DEBUGGING");
	QUrl inspectorUrl = QUrl(QStringLiteral("http://localhost:%1").arg(port));
	int index = s_views.indexOf(m_view);

	QNetworkReply *reply = Application::instance()->networkManager()->get(QNetworkRequest(inspectorUrl.resolved(QUrl("json/list"))));
	connect(reply, &QNetworkReply::finished, this, [=]() {
		QJsonArray clients = QJsonDocument::fromJson(reply->readAll()).array();
		QUrl pageUrl;
		if (clients.size() > index) {
			QJsonObject object = clients.at(index).toObject();
			pageUrl = inspectorUrl.resolved(QUrl(object.value(QStringLiteral("devtoolsFrontendUrl")).toString()));
		}
		load(pageUrl);
		pushView(this);
		show();
	});
}

void WebInspector::inspectElement()
{
	m_inspectElement = true;
}

bool WebInspector::isEnabled()
{
	return qEnvironmentVariableIsSet("QTWEBENGINE_REMOTE_DEBUGGING");
}

void WebInspector::pushView(QWebEngineView *view)
{
	s_views.removeOne(view);
	s_views.prepend(view);
}

void WebInspector::registerView(QWebEngineView *view)
{
	s_views.prepend(view);
}

void WebInspector::unregisterView(QWebEngineView *view)
{
	s_views.removeOne(view);
}

void WebInspector::loadFinished()
{
	if (m_inspectElement) {
		m_view->triggerPageAction(QWebEnginePage::InspectElement);
		m_inspectElement = false;
	}
}

void WebInspector::keyPressEvent(QKeyEvent* event)
{
	Q_UNUSED(event)
}

void WebInspector::keyReleaseEvent(QKeyEvent* event)
{
	Q_UNUSED(event)
}
}
