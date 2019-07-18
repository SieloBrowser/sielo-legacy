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

#include "Plugins/Plugins.hpp"

#include <QDir>
#include <QPluginLoader>
#include <QMessageBox>

#include <QDebug>

#include "Utils/DataPaths.hpp"
#include "Utils/Settings.hpp"

#include "Widgets/NavigationBar.hpp"
#include "Widgets/TitleBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{

Plugins::Plugins(QObject* parent) :
	QObject(parent)
{
	loadSettings();
}

QList<Plugins::Plugin> Plugins::getAvailablePlugins()
{
	loadAvailablePlugins();

	return m_availablePlugins;
}

bool Plugins::loadPlugin(Plugin* plugin)
{
	if (plugin->isLoaded())
		return true;

	plugin->pluginLoader->setFileName(plugin->fullPath);
	PluginInterface* iPlugin{qobject_cast<PluginInterface*>(plugin->pluginLoader->instance())};

	if (!iPlugin)
		return false;

	m_availablePlugins.removeOne(*plugin);
	plugin->instance = initPlugin(PluginInterface::LateInitState, iPlugin, plugin->pluginLoader);
	m_availablePlugins.prepend(*plugin);

	refreshLoadedPlugins();

	if (plugin->isLoaded()) {
		foreach(BrowserWindow* window, Application::instance()->windows())
		{
			window->titleBar()->navigationToolBar()->addExtensionAction(plugin->instance->navigationBarButton(window));
		}
	}

	return plugin->isLoaded();
}

void Plugins::unloadPlugin(Plugin* plugin)
{
	if (!plugin->isLoaded())
		return;

	plugin->instance->unload();
	plugin->pluginLoader->unload();
	emit pluginUnloaded(plugin->instance);

	m_availablePlugins.removeOne(*plugin);
	plugin->instance = nullptr;
	m_availablePlugins.append(*plugin);

	refreshLoadedPlugins();
}

void Plugins::loadSettings()
{
	Settings settings{};

	settings.beginGroup("Plugin-Settings");
	m_allowedPlugins = settings.value("AllowedPlugins", QStringList()).toStringList();
	settings.endGroup();
}

void Plugins::shutdown()
{
	foreach(PluginInterface* iPlugin, m_loadedPlugins) iPlugin->unload();
}

void Plugins::loadPlugins()
{
	QDir settingsDir(DataPaths::currentProfilePath() + "/plugins/");

	if (!settingsDir.exists())
		settingsDir.mkdir(settingsDir.absolutePath());

	foreach(const QString& fullPath, m_allowedPlugins)
	{
		QPluginLoader* loader{new QPluginLoader(fullPath)};
		PluginInterface* iPlugin{qobject_cast<PluginInterface*>(loader->instance())};

		if (!iPlugin) {
			qWarning() << "Loading " << fullPath << " plugin failed: " << loader->errorString();
			continue;
		}

		Plugin plugin{};
		plugin.fileName = QFileInfo(fullPath).fileName();
		plugin.fullPath = fullPath;
		plugin.pluginLoader = loader;
		plugin.instance = initPlugin(PluginInterface::StartupInitState, iPlugin, loader);

		if (plugin.isLoaded()) {
			plugin.pluginProp = iPlugin->pluginProp();

			m_loadedPlugins.append(plugin.instance);
			m_availablePlugins.append(plugin);
		}

	}

	refreshLoadedPlugins();

	qDebug() << "Plugins are loaded! ";
}

void Plugins::loadAvailablePlugins()
{
	if (m_pluginsLoaded)
		return;

	m_pluginsLoaded = true;

	QDir pluginsDir{QDir(DataPaths::currentProfilePath() + "/plugins/")};

	foreach(const QString& fileName,
			pluginsDir.entryList(QDir::Files))
	{
		const QString absolutPath{pluginsDir.absoluteFilePath(fileName)};

		QPluginLoader* loader{new QPluginLoader(absolutPath)};
		PluginInterface* iPlugin{qobject_cast<PluginInterface*>(loader->instance())};

		if (!iPlugin) {
			qWarning() << "Available plugin loading error: " << loader->errorString();
			continue;
		}

		Plugin plugin{};
		plugin.fileName = fileName;
		plugin.fullPath = absolutPath;
		plugin.pluginProp = iPlugin->pluginProp();
		plugin.pluginLoader = loader;
		plugin.instance = nullptr;

		loader->unload();

		if (!alreadyPropInAvailable(plugin.pluginProp))
			m_availablePlugins.append(plugin);
	}
}

PluginInterface* Plugins::initPlugin(PluginInterface::InitState state, PluginInterface* pluginInterface,
									 QPluginLoader* loader)
{
	if (!pluginInterface)
		return nullptr;

	pluginInterface->init(state, DataPaths::currentProfilePath() + "/plugins/");

	if (!pluginInterface->testPlugin()) {
		pluginInterface->unload();
		if (loader)
			loader->unload();

		emit pluginUnloaded(pluginInterface);

		return nullptr;
	}

	QApplication::installTranslator(pluginInterface->getTranslator(Application::instance()->currentLanguageFile()));

	return pluginInterface;
}

void Plugins::refreshLoadedPlugins()
{
	m_loadedPlugins.clear();

	foreach(const Plugin& plugin, m_availablePlugins)
	{
		if (plugin.isLoaded())
			m_loadedPlugins.append(plugin.instance);
	}
}

bool Plugins::alreadyPropInAvailable(const PluginProp& prop)
{
	foreach(const Plugin& plugin, m_availablePlugins)
	{
		if (plugin.pluginProp == prop)
			return true;
	}

	return false;
}
}
