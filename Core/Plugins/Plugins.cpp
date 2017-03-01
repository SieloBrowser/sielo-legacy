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

#include "Plugins/Plugins.hpp"

#include <QDir>
#include <QPluginLoader>
#include <QMessageBox>

#include <QSettings>

#include "Application.hpp"

namespace Sn {

Plugins::Plugins(QObject *parent) :
	QObject(parent)
{
	loadSettings();
}

QList<Plugins::Plugin> Plugins::getAvailablePlugins()
{
	loadAvailablePlugins();

	return m_availablePlugins;
}

bool Plugins::loadPlugin(Plugin *plugin)
{
	if (plugin->isLoaded())
		return true;

	plugin->pluginLoader->setFileName(plugin->fullPath);
	PluginInterface *iPlugin{ qobject_cast<PluginInterface*>(plugin->pluginLoader->instance()) };

	if (!iPlugin)
		return false;

	m_availablePlugins.removeOne(*plugin);
	plugin->instance = initPlugin(PluginInterface::LateInitState, iPlugin, plugin->pluginLoader);
	m_availablePlugins.prepend(*plugin);

	refreshLoadedPlugins();

	return plugin->isLoaded();
}

void Plugins::unloadPlugin(Plugin *plugin)
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
	QSettings settings{};

	settings.beginGroup("Plugin-Settings");
	m_allowedPlugins = settings.value("AllowedPlugins", QStringList()).toStringList();
	settings.endGroup();
}

}
