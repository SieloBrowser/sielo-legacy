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

#include "Utils/ClosedTabsManager.hpp"

#include <QWebEngine/WebHistory.hpp>

#include "Application.hpp"

#include "Web/Tab/WebTab.hpp"

namespace Sn {

ClosedTabsManager::ClosedTabsManager()
{
	// Empty
}

void ClosedTabsManager::saveTab(WebTab* tab, int position)
{
	if (Application::instance()->privateBrowsing())
		return;

	if (tab->url().isEmpty() && tab->history()->itemCount() == 0)
		return;

	Tab closedTab;
	closedTab.url = tab->url();
	closedTab.title = tab->title();
	closedTab.icon = tab->icon();
	closedTab.position = position;
	closedTab.history = tab->historyData();
	closedTab.zoomLevel = tab->zoomLevel();

	m_closedTabs.prepend(closedTab);

}

ClosedTabsManager::Tab ClosedTabsManager::takeLastClosedTab()
{
	Tab tab;
	tab.position = -1;

	if (m_closedTabs.count() > 0)
		tab = m_closedTabs.takeFirst();

	return tab;
}

ClosedTabsManager::Tab ClosedTabsManager::takeTabAt(int index)
{
	Tab tab;
	tab.position = -1;

	QLinkedList<Tab>::iterator it;
	int i = 0;

	for (it = m_closedTabs.begin(); it != m_closedTabs.end(); ++it, ++i) {
		if (i == index) {
			tab = *it;
			m_closedTabs.erase(it);
			break;
		}
	}

	return tab;
}

void ClosedTabsManager::clearList()
{
	m_closedTabs.clear();
}

}