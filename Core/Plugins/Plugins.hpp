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
#ifndef CORE_PLUGINS_HPP
#define CORE_PLUGINS_HPP

#include "SharedDefines.hpp"

#include <QObject>
#include <QList>

#include "Plugins/PluginInterface.hpp"

class QPluginLoader;

namespace Sn {

class SIELO_SHAREDLIB Plugins: public QObject {
Q_OBJECT

public:
	struct Plugin {
		QString fileName{};
		QString fullPath{};
		PluginProp pluginProp{};
		QPluginLoader* pluginLoader{nullptr};
		PluginInterface* instance{nullptr};

		Plugin() {}

		bool isLoaded() const { return instance; }
		bool operator==(const Plugin& other) const
		{
			return (fileName == other.fileName &&
					fullPath == other.fullPath &&
					pluginProp == other.pluginProp &&
					instance == other.instance);
		}
	};

	explicit Plugins(QObject* parent = nullptr);

	QList<Plugin> getAvailablePlugins();

	bool loadPlugin(Plugin* plugin);
	void unloadPlugin(Plugin* plugin);

	void shutdown();

public slots:
	void loadSettings();
	void loadPlugins();

protected:
	QList<PluginInterface*> m_loadedPlugins{};

signals:
	void pluginUnloaded(PluginInterface* plugin);

private:
	bool alreadyPropInAvailable(const PluginProp& prop);
	PluginInterface* initPlugin(PluginInterface::InitState state, PluginInterface* pluginInterface,
								QPluginLoader* loader);

	void refreshLoadedPlugins();
	void loadAvailablePlugins();

	QList<Plugin> m_availablePlugins{};
	QList<PluginInterface*> m_internalPlugins;
	QStringList m_allowedPlugins{};

	bool m_pluginsLoaded{false};
};
}
Q_DECLARE_METATYPE(Sn::Plugins::Plugin)
	
#endif // CORE_PLUGINS_HPP
