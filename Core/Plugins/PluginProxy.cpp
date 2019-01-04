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

#include "Plugins/PluginProxy.hpp"

#include <QDebug>

#include "Web/WebPage.hpp"
#include "Web/WebView.hpp"
#include "Web/WebHitTestResult.hpp"

#include "BrowserWindow.hpp"

namespace Sn
{
PluginProxy::PluginProxy() :
	Plugins()
{
	connect(this, &Plugins::pluginUnloaded, this, &PluginProxy::pluginUnloaded);
}

void PluginProxy::registerAppEventHandler(const EventHandlerType& type, PluginInterface* obj)
{
	switch (type) {
	case MouseDoubleClickHandler:
		if (!m_mouseDoubleClickHandlers.contains(obj))
			m_mouseDoubleClickHandlers.append(obj);
		break;

	case MousePressHandler:
		if (!m_mousePressHandlers.contains(obj))
			m_mousePressHandlers.append(obj);
		break;

	case MouseReleaseHandler:
		if (!m_mouseReleaseHandlers.contains(obj))
			m_mouseReleaseHandlers.append(obj);
		break;

	case MouseMoveHandler:
		if (!m_mouseMoveHandlers.contains(obj))
			m_mouseMoveHandlers.append(obj);
		break;

	case KeyPressHandler:
		if (!m_keyPressHandlers.contains(obj))
			m_keyPressHandlers.append(obj);
		break;

	case KeyReleaseHandler:
		if (!m_keyReleaseHandlers.contains(obj))
			m_keyReleaseHandlers.append(obj);
		break;

	case WheelEventHandler:
		if (!m_wheelEventHandlers.contains(obj))
			m_wheelEventHandlers.append(obj);
		break;

	case CommandsHandler:
		if (!m_commandHandlers.contains(obj))
			m_commandHandlers.append(obj);
		break;

	default:
		qWarning("Plugins: Registering unknown event handler type");
		break;
	}
}

QList<QWidget*> PluginProxy::navigationBarButton(BrowserWindow* window)
{
	QList<QWidget*> buttons{};

	foreach (PluginInterface* iPlugin, m_loadedPlugins)
	{
		buttons.append(iPlugin->navigationBarButton(window));
	}

	return buttons;
}

void PluginProxy::populateWebViewMenu(QMenu* menu, WebView* view, const WebHitTestResult& result)
{
	if (!menu || !view)
		return;

	foreach(PluginInterface* iPlugin, m_loadedPlugins)
		iPlugin->populateWebViewMenu(menu, view, result);
}

void PluginProxy::populateExtensionsMenu(QMenu* menu, TabWidget* tabWidget)
{
	if (!menu || !tabWidget)
		return;

	foreach(PluginInterface* iPlugin, m_loadedPlugins)
		iPlugin->populateExtensionsMenu(menu, tabWidget);
}

bool PluginProxy::processMouseDoubleClick(const Application::ObjectName& type, QObject* obj, QMouseEvent* event)
{
	bool accepted{false};

	foreach(PluginInterface* iPlugin, m_mouseDoubleClickHandlers)
	{
		if (iPlugin->mouseDoubleClick(type, obj, event))
			accepted = true;
	}

	return accepted;
}
bool PluginProxy::processMousePress(const Application::ObjectName& type, QObject* obj, QMouseEvent* event)
{
	bool accepted{false};

	foreach(PluginInterface* iPlugin, m_mousePressHandlers)
	{
		if (iPlugin->mousePress(type, obj, event))
			accepted = true;
	}

	return accepted;
}
bool PluginProxy::processMouseRelease(const Application::ObjectName& type, QObject* obj, QMouseEvent* event)
{
	bool accepted{false};

	foreach(PluginInterface* iPlugin, m_mouseReleaseHandlers)
	{
		if (iPlugin->mouseRelease(type, obj, event))
			accepted = true;
	}

	return accepted;
}
bool PluginProxy::processMouseMove(const Application::ObjectName& type, QObject* obj, QMouseEvent* event)
{
	bool accepted{false};

	foreach(PluginInterface* iPlugin, m_mouseMoveHandlers)
	{
		if (iPlugin->mouseDoubleClick(type, obj, event))
			accepted = true;
	}

	return accepted;
}

bool PluginProxy::processKeyPress(const Application::ObjectName& type, QObject* obj, QKeyEvent* event)
{
	bool accepted{false};

	foreach(PluginInterface* iPlugin, m_keyPressHandlers)
	{
		if (iPlugin->keyPress(type, obj, event))
			accepted = true;
	}

	return accepted;
}
bool PluginProxy::processKeyRelease(const Application::ObjectName& type, QObject* obj, QKeyEvent* event)
{
	bool accepted{false};

	foreach(PluginInterface* iPlugin, m_keyReleaseHandlers)
	{
		if (iPlugin->keyRelease(type, obj, event))
			accepted = true;
	}

	return accepted;
}

bool PluginProxy::processWheelEvent(const Application::ObjectName& type, QObject* obj, QWheelEvent* event)
{
	bool accepted{false};

	foreach(PluginInterface* iPlugin, m_wheelEventHandlers)
	{
		if (iPlugin->wheelEvent(type, obj, event))
			accepted = true;
	}

	return accepted;
}

bool PluginProxy::processCommand(const QString& command, const QStringList& args)
{
	bool accepted{false};

	foreach(PluginInterface* iPlugin, m_commandHandlers)
	{
		if (iPlugin->processCommand(command, args))
			accepted = true;
	}

	return accepted;
}

bool PluginProxy::acceptNavigationRequest(WebPage* page, const QUrl& url, Engine::WebPage::NavigationType type,
										  bool isMainFrame)
{
	bool accepted{true};

	foreach(PluginInterface* iPlugin, m_loadedPlugins)
	{
		if (!iPlugin->acceptNavigationRequest(page, url, type, isMainFrame))
			accepted = false;
	}

	return accepted;
}

void PluginProxy::emitWebPageCreated(WebPage* page)
{
	emit webPageCreated(page);
}
void PluginProxy::emitWebPageDeleted(WebPage* page)
{
	emit webPageDeleted(page);
}

void PluginProxy::emitWebTabCreated(WebTab* tab)
{
	emit webTabCreated(tab);
}

void PluginProxy::emitWebTabDeleted(WebTab* tab)
{
	emit webTabDeleted(tab);
}

void PluginProxy::emitTabsSpaceCreated(TabWidget* tabsSpace)
{
	emit tabsSpaceCreated(tabsSpace);
}

void PluginProxy::emitTabsSpaceDeleted(TabWidget* tabsSpace)
{
	emit tabsSpaceDeleted(tabsSpace);
}

void PluginProxy::emitMainWindowCreated(BrowserWindow* window)
{
	emit mainWindowCreated(window);
}
void PluginProxy::emitMainWindowDeleted(BrowserWindow* window)
{
	emit mainWindowDeleted(window);
}

void PluginProxy::emitThemeChanged(bool isDark)
{
	emit themeChanged(isDark);
}

void PluginProxy::pluginUnloaded(PluginInterface* plugin)
{
	m_mouseDoubleClickHandlers.removeOne(plugin);
	m_mousePressHandlers.removeOne(plugin);
	m_mouseReleaseHandlers.removeOne(plugin);
	m_mouseMoveHandlers.removeOne(plugin);

	m_wheelEventHandlers.removeOne(plugin);

	m_keyPressHandlers.removeOne(plugin);
	m_keyReleaseHandlers.removeOne(plugin);

	m_commandHandlers.removeOne(plugin);
}
}
