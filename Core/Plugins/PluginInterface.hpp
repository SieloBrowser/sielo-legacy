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
#ifndef CORE_PLUGININTERFACE_HPP
#define CORE_PLUGININTERFACE_HPP

#include "SharedDefines.hpp"

#include <QtPlugin>
#include <QPixmap>
#include <QObject>

#include <QWebEngine/WebPage.hpp>

#include "Application.hpp"

namespace Sn
{
class WebView;
class WebPage;
class WebHitTestResult;

class NavigationToolBar;

class TabWidget;
class BrowserWindow;

struct SIELO_SHAREDLIB PluginProp {
	QString name{};
	QString info{};
	QString desc{};
	QString author{};
	QString version{};
	QPixmap icon{};
	bool hasSettings{false};

	PluginProp() {}

	bool operator==(const PluginProp& other) const
	{
		return (name == other.name &&
				info == other.info &&
				desc == other.desc &&
				author == other.author &&
				version == other.version);
	}
};

class SIELO_SHAREDLIB PluginInterface {
public:
	enum InitState { StartupInitState, LateInitState };

	virtual PluginProp pluginProp() = 0;
	virtual void init(InitState state, const QString& settingsPath) = 0;
	virtual void unload() = 0;
	virtual bool testPlugin() = 0;

	virtual ~PluginInterface() {}
	virtual QTranslator* getTranslator(const QString& locale) { Q_UNUSED(locale) return nullptr; }
	virtual void showSettings(QWidget* parent = nullptr) { Q_UNUSED(parent) }

	virtual QWidget* navigationBarButton(BrowserWindow* window) { return nullptr; }

	virtual void populateWebViewMenu(QMenu* menu, WebView* view, const WebHitTestResult& result)
	{
		Q_UNUSED(menu)
			Q_UNUSED(view)
			Q_UNUSED(menu)
	}
	virtual void populateExtensionsMenu(QMenu *menu, TabWidget* tabWidget) { Q_UNUSED(menu) Q_UNUSED(tabWidget) }

	virtual bool mouseDoubleClick(const Application::ObjectName& objName, QObject* obj, QMouseEvent* event)
	{
		Q_UNUSED(objName)
			Q_UNUSED(obj)
			Q_UNUSED(event)
			return false;
	}
	virtual bool mousePress(const Application::ObjectName& objName, QObject* obj, QMouseEvent* event)
	{
		Q_UNUSED(objName)
			Q_UNUSED(obj)
			Q_UNUSED(event)
			return false;
	}
	virtual bool mouseRelease(const Application::ObjectName& objName, QObject* obj, QMouseEvent* event)
	{
		Q_UNUSED(objName)
			Q_UNUSED(obj)
			Q_UNUSED(event)
			return false;
	}
	virtual bool mouseMouve(const Application::ObjectName& objName, QObject* obj, QMouseEvent* event)
	{
		Q_UNUSED(objName)
			Q_UNUSED(obj)
			Q_UNUSED(event)
			return false;
	}

	virtual bool wheelEvent(const Application::ObjectName& objName, QObject* obj, QWheelEvent* event)
	{
		Q_UNUSED(objName)
			Q_UNUSED(obj)
			Q_UNUSED(event)
			return false;
	}

	virtual bool keyPress(const Application::ObjectName& objName, QObject* obj, QKeyEvent* event)
	{
		Q_UNUSED(objName)
			Q_UNUSED(obj)
			Q_UNUSED(event)
			return false;
	}
	virtual bool keyRelease(const Application::ObjectName& objName, QObject* obj, QKeyEvent* event)
	{
		Q_UNUSED(objName)
			Q_UNUSED(obj)
			Q_UNUSED(event)
			return false;
	}

	virtual bool processCommand(const QString& command, const QStringList& args)
	{
		Q_UNUSED(command);
		Q_UNUSED(args);

		return false;
	}

	virtual bool acceptNavigationRequest(WebPage* page, const QUrl& url, Engine::WebPage::NavigationType type,
										 bool isMainFrame)
	{
		Q_UNUSED(page);
		Q_UNUSED(url);
		Q_UNUSED(type);
		Q_UNUSED(isMainFrame);
		return true;
	}

};
}

QT_BEGIN_NAMESPACE

#define PluginInterface_iid "com.feldrise.Sielo.PluginInterface"
Q_DECLARE_INTERFACE(Sn::PluginInterface, PluginInterface_iid)

QT_END_NAMESPACE

#endif // CORE_PLUGININTERFACE_HPP
