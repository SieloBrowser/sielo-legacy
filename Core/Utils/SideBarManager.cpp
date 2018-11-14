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

#include "SideBarManager.hpp"

#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/SideBar/SideBar.hpp"
#include "Widgets/SideBar/SideBarInterface.hpp"

#include "BrowserWindow.hpp"

namespace Sn
{
QHash<QString, QPointer<SideBarInterface>> SideBarManager::s_sidebars;

void SideBarManager::addSidebar(const QString& id, SideBarInterface* interface)
{
	s_sidebars[id] = interface;
}

void SideBarManager::removeSidebar(SideBarInterface* interface)
{
	const QString id{s_sidebars.key(interface)};
	if (id.isEmpty())
		return;

	s_sidebars.remove(id);

	foreach (BrowserWindow* window, Application::instance()->windows())
	{
		foreach(TabWidget* tabWidget, window->tabsSpaceSplitter()->tabWidgets())
			tabWidget->sideBarManager()->sideBarRemoved(id);
	}
}

void SideBarManager::sShowSideBar()
{
	if (QAction* action = qobject_cast<QAction*>(sender())) 
		showSideBar(action->data().toString());
}

SideBarManager::SideBarManager(TabStackedWidget* tabWidget) :
	QObject(tabWidget),
	m_tabWidget(tabWidget)
{
	// Empty
}

void SideBarManager::createMenu(QMenu* menu)
{
	foreach(QAction* action, menu->actions())
		m_tabWidget->removeAction(action);

	menu->clear();

	foreach(const QPointer<SideBarInterface>& sidebar, s_sidebars)
	{
		if (sidebar) {
			QAction* action{sidebar.data()->createMenuAction()};
			action->setData(s_sidebars.key(sidebar));
			action->setChecked(m_activeBar == s_sidebars.key(sidebar));

			connect(action, &QAction::triggered, this, &SideBarManager::sShowSideBar);

			menu->addAction(action);
		}
	}

	m_tabWidget->addActions(menu->actions());
}

void SideBarManager::showSideBar(const QString& id, bool toggle)
{
	if (id.isEmpty() || id == QLatin1String("None"))
		return;

	if (!m_sideBar)
		m_sideBar = m_tabWidget->addSideBar();

	if (id == m_activeBar) {
		if (!toggle)
			return;

		m_sideBar.data()->close();
		m_activeBar.clear();
		m_tabWidget->saveSideBarSettings();

		return;
	}

	SideBarInterface* sideBar{s_sidebars[id].data()};

	if (!sideBar) {
		m_sideBar.data()->close();
		return;
	}

	m_sideBar.data()->setTitle(sideBar->title());
	m_sideBar.data()->setWidget(sideBar->createSideBarWidget(dynamic_cast<TabWidget*>(m_tabWidget)));

	m_activeBar = id;
	m_tabWidget->saveSideBarSettings();
}

void SideBarManager::sideBarRemoved(const QString& id)
{
	if (m_activeBar == id && m_sideBar) {
		m_sideBar.data()->setWidget(nullptr);
		m_sideBar.data()->close();
	}
}

void SideBarManager::closeSideBar()
{
	if (Application::instance()->isClosing())
		return;

	m_activeBar.clear();
	m_tabWidget->saveSideBarSettings();
}
}
