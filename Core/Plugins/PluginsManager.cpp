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

#include "PluginsManager.hpp"

#include <QMessageBox>

#include "Utils/Settings.hpp"

#include "Plugins/Plugins.hpp"
#include "Plugins/PluginProxy.hpp"
#include "Plugins/PluginListDelegate.hpp"

#include "Application.hpp"

namespace Sn
{
PluginsManager::PluginsManager(QWidget* parent) :
	QWidget(parent)
{
	setupUI();

	Settings settings{};
	bool pluginsEnabled{settings.value("Plugin-Settings/EnablePlugin", true).toBool()};

	m_list->setEnabled(pluginsEnabled);
	m_list->setItemDelegate(new PluginListDelegate(m_list));

	connect(m_settingsButton, &QPushButton::clicked, this, &PluginsManager::settingsClicked);
	connect(m_list, &QListWidget::currentItemChanged, this, &PluginsManager::currentChanged);
	connect(m_list, &QListWidget::itemChanged, this, &PluginsManager::itemChanged);
}

void PluginsManager::load()
{
	if (!m_loaded) {
		refresh();
		m_loaded = true;
	}
}

void PluginsManager::save()
{
	if (!m_loaded)
		return;

	Settings settings{};
	QStringList allowedPlugins{};

	for (int i{0}; i < m_list->count(); ++i) {
		QListWidgetItem* item{m_list->item(i)};

		if (item->checkState() == Qt::Checked) {
			const Plugins::Plugin plugin{item->data(Qt::UserRole + 10).value<Plugins::Plugin>()};
			allowedPlugins.append(plugin.fullPath);
		}
	}

	settings.setValue("Plugin-Settings/AllowedPlugins", allowedPlugins);
}

void PluginsManager::settingsClicked()
{
	QListWidgetItem* item{m_list->currentItem()};

	if (!item || item->checkState() == Qt::Unchecked)
		return;

	Plugins::Plugin plugin{item->data(Qt::UserRole + 10).value<Plugins::Plugin>()};

	if (!plugin.isLoaded()) {
		Application::instance()->plugins()->loadPlugin(&plugin);
		item->setData(Qt::UserRole + 10, QVariant::fromValue(plugin));
	}

	if (plugin.isLoaded() && plugin.pluginProp.hasSettings)
		plugin.instance->showSettings(this);
}

void PluginsManager::currentChanged(QListWidgetItem* item)
{
	if (!item)
		return;

	const Plugins::Plugin plugin{item->data(Qt::UserRole + 10).value<Plugins::Plugin>()};
	bool showSettings{plugin.pluginProp.hasSettings};

	if (!plugin.isLoaded())
		showSettings = false;

	m_settingsButton->setEnabled(showSettings);
}

void PluginsManager::itemChanged(QListWidgetItem* item)
{
	if (!item)
		return;

	Plugins::Plugin plugin{item->data(Qt::UserRole + 10).value<Plugins::Plugin>()};

	if (item->checkState() == Qt::Checked)
		Application::instance()->plugins()->loadPlugin(&plugin);
	else
		Application::instance()->plugins()->unloadPlugin(&plugin);

	disconnect(m_list, &QListWidget::itemChanged, this, &PluginsManager::itemChanged);

	if (item->checkState() == Qt::Checked && !plugin.isLoaded()) {
		item->setCheckState(Qt::Unchecked);
		QMessageBox::critical(this, tr("Error"), tr("Cannot load plugin!"));
	}

	item->setData(Qt::UserRole + 10, QVariant::fromValue(plugin));

	connect(m_list, &QListWidget::itemChanged, this, &PluginsManager::itemChanged);

	currentChanged(m_list->currentItem());
}

void PluginsManager::refresh()
{
	m_list->clear();
	m_settingsButton->setEnabled(false);

	disconnect(m_list, &QListWidget::itemChanged, this, &PluginsManager::itemChanged);

	const QList<Plugins::Plugin> allPlugins = Application::instance()->plugins()->getAvailablePlugins();

	foreach(const Plugins::Plugin& plugin, allPlugins)
	{
		PluginProp prop{plugin.pluginProp};
		QListWidgetItem* item{new QListWidgetItem(m_list)};
		QIcon icon{QIcon(prop.icon)};

		if (icon.isNull())
			icon = Application::getAppIcon("plugins", "preferences");

		item->setIcon(icon);

		QString pluginInfo{QString("<b>%1</b> %2<br/><i>%3</i><br/>%4").arg(prop.name, prop.version, prop.author.toHtmlEscaped(), prop.info)};

		item->setToolTip(pluginInfo);
		item->setText(prop.name);
		item->setData(Qt::UserRole, prop.version);
		item->setData(Qt::UserRole + 1, prop.info);
		item->setData(Qt::UserRole + 2, prop.desc);

		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(plugin.isLoaded() ? Qt::Checked : Qt::Unchecked);
		item->setData(Qt::UserRole + 10, QVariant::fromValue(plugin));

		m_list->addItem(item);
	}

	sortItem();

	connect(m_list, &QListWidget::itemChanged, this, &PluginsManager::itemChanged);
}

void PluginsManager::setupUI()
{
	m_layout = new QGridLayout(this);

	m_list = new QListWidget(this);
	m_list->setAlternatingRowColors(true);
	m_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_list->setLayoutDirection(Qt::LeftToRight);
	m_list->setUniformItemSizes(true);
	m_list->setWordWrap(true);

	m_settingsButton = new QPushButton(tr("Settings"), this);
	m_settingsButton->setEnabled(false);

	m_spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_layout->addWidget(m_list, 0, 0, 1, 2);
	m_layout->addWidget(m_settingsButton, 1, 0, 1, 1);
	m_layout->addItem(m_spacer, 1, 1, 1, 1);
}

void PluginsManager::sortItem()
{
	m_list->sortItems();

	bool itemMoved{false};

	do {
		itemMoved = false;
		for (int i{0}; i < m_list->count(); ++i) {
			QListWidgetItem* topItem{m_list->item(i)};
			QListWidgetItem* bottomItem{m_list->item(i + 1)};

			if (!topItem || !bottomItem)
				continue;

			if (topItem->checkState() == Qt::Unchecked && bottomItem->checkState() == Qt::Checked) {
				QListWidgetItem* item{m_list->takeItem(i + 1)};
				m_list->insertItem(i, item);

				itemMoved = true;
			}
		}
	} while (itemMoved);
}
}
