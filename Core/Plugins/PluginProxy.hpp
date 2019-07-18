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

#pragma once
#ifndef CORE_PLUGINPROXY_HPP
#define CORE_PLUGINPROXY_HPP

#include "SharedDefines.hpp"

#include <QList>

#include <QWebEngine/WebPage.hpp>

#include "Web/WebPage.hpp"

#include "Plugins/Plugins.hpp"
#include "Application.hpp"

namespace Sn
{

class WebView;
class WebTab;
class TabWidget;
class WebHitTestResult;

class NavigationToolBar;

class TabWidget;
class BrowserWindow;

class SIELO_SHAREDLIB PluginProxy: public Plugins {
	Q_OBJECT

public:
	enum EventHandlerType {
		MouseDoubleClickHandler,
		MousePressHandler,
		MouseReleaseHandler,
		MouseMoveHandler,
		KeyPressHandler,
		KeyReleaseHandler,
		WheelEventHandler,
		CommandsHandler
	};

	explicit PluginProxy();

	void registerAppEventHandler(const EventHandlerType& type, PluginInterface* obj);

	QList<QWidget*> navigationBarButton(BrowserWindow* window);

	void populateWebViewMenu(QMenu* menu, WebView* view, const WebHitTestResult& result);
	void populateExtensionsMenu(QMenu* menu, TabWidget* tabWidget);

	bool processMouseDoubleClick(const Application::ObjectName& type, QObject* obj, QMouseEvent* event);
	bool processMousePress(const Application::ObjectName& type, QObject* obj, QMouseEvent* event);
	bool processMouseRelease(const Application::ObjectName& type, QObject* obj, QMouseEvent* event);
	bool processMouseMove(const Application::ObjectName& type, QObject* obj, QMouseEvent* event);

	bool processWheelEvent(const Application::ObjectName& type, QObject* obj, QWheelEvent* event);

	bool processKeyPress(const Application::ObjectName& type, QObject* obj, QKeyEvent* event);
	bool processKeyRelease(const Application::ObjectName& type, QObject* obj, QKeyEvent* event);

	bool processCommand(const QString& command, const QStringList& args);

	bool acceptNavigationRequest(WebPage* page, const QUrl& url, Engine::WebPage::NavigationType type, bool isMainFrame);


	void emitWebPageCreated(WebPage* page);
	void emitWebPageDeleted(WebPage* page);

	void emitWebTabCreated(WebTab* tab);
	void emitWebTabDeleted(WebTab* tab);

	void emitTabsSpaceCreated(TabWidget* tabSpace);
	void emitTabsSpaceDeleted(TabWidget* tabSpace);

	void emitMainWindowCreated(BrowserWindow* window);
	void emitMainWindowDeleted(BrowserWindow* window);

	void emitThemeChanged(bool isDark);

signals:
	void webPageCreated(WebPage* page);
	void webPageDeleted(WebPage* page);

	void webTabCreated(WebTab* tab);
	void webTabDeleted(WebTab* tab);

	void tabsSpaceCreated(TabWidget* tabsSpace);
	void tabsSpaceDeleted(TabWidget* tabsSpace);
	
	void mainWindowCreated(BrowserWindow* window);
	void mainWindowDeleted(BrowserWindow* window);

	void themeChanged(bool isDark);

private slots:
	void pluginUnloaded(PluginInterface* plugin);

private:
	QList<PluginInterface*> m_mouseDoubleClickHandlers;
	QList<PluginInterface*> m_mousePressHandlers;
	QList<PluginInterface*> m_mouseReleaseHandlers;
	QList<PluginInterface*> m_mouseMoveHandlers;

	QList<PluginInterface*> m_wheelEventHandlers;

	QList<PluginInterface*> m_keyPressHandlers;
	QList<PluginInterface*> m_keyReleaseHandlers;

	QList<PluginInterface*> m_commandHandlers;
};
}

#endif //CORE_PLUGINPROXY_HPP
