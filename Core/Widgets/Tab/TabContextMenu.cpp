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

#include "TabContextMenu.hpp"

#include <QMessageBox>

#include "Web/Tab/WebTab.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"

#include "BrowserWindow.hpp"

namespace Sn
{
TabContextMenu::TabContextMenu(int index, TabWidget* tabWidget, Options options) :
	QMenu(),
	m_clickedTab(index),
	m_tabWidget(tabWidget),
	m_options(options)
{
	connect(this, &TabContextMenu::tabCloseRequested, tabWidget->tabBar(), &MainTabBar::tabCloseRequested);
	connect(this, SIGNAL(reloadTab(int)), tabWidget, SLOT(reloadTab(int)));
	connect(this, SIGNAL(stopTab(int)), tabWidget, SLOT(stopTab(int)));
	connect(this, SIGNAL(closeAllButCurrent(int)), tabWidget, SLOT(closeAllButCurrent(int)));
	connect(this, SIGNAL(closeToRight(int)), tabWidget, SLOT(closeToRight(int)));
	connect(this, SIGNAL(closeToLeft(int)), tabWidget, SLOT(closeToLeft(int)));
	connect(this, SIGNAL(duplicateTab(int)), tabWidget, SLOT(duplicateTab(int)));
	connect(this, SIGNAL(detachTab(int)), tabWidget, SLOT(detachTab(int)));
	connect(this, SIGNAL(loadTab(int)), tabWidget, SLOT(loadTab(int)));
	connect(this, SIGNAL(unloadTab(int)), tabWidget, SLOT(unloadTab(int)));

	if (index != -1) {
		WebTab* webTab = qobject_cast<WebTab*>(m_tabWidget->widget(index));

		if (!webTab)
			return;

		if (tabWidget->webTab(m_clickedTab)->webView()->isLoading())
			addAction(Application::getAppIcon("stop"),
						   tr("&Stop Tab"),
						   this,
						   SLOT(stopTab()));
		else
			addAction(Application::getAppIcon("refresh"),
						   tr("&Reload Tab"),
						   this,
						   SLOT(reloadTab()));

		addAction(Application::getAppIcon("duplicate", "tabs"), tr("&Duplicate Tab"), this, SLOT(duplicateTab()));

		if (m_options & ShowDetachTabAction && (Application::instance()->windowCount() > 1 || m_tabWidget->count() > 1))
			addAction(QIcon::fromTheme("detach"), tr("D&etach Tab"), this, SLOT(detachTab()));

		addAction(webTab->isPinned() ? tr("Un&pin Tab") : tr("&Pin Tab"), this, SLOT(pinTab()));
		addAction(webTab->isMuted() ? tr("Un&mute Tab") : tr("&Mute Tab"), this, SLOT(muteTab()));
		addSeparator();

		if (!webTab->isRestored()) 
			addAction(tr("Load Tab"), this, SLOT(loadTab()));
		else 
			addAction(tr("Unload Tab"), this, SLOT(unloadTab()));

		addSeparator();

		addAction(tr("Re&load All Tabs"), m_tabWidget, SLOT(reloadAllTabs()));
		addAction(tr("Bookmark &All Tabs"), m_tabWidget->window(), SLOT(bookmarkAllTabs()));
		addSeparator();

		if (m_options & ShowCloseOtherTabsActions) {
			addAction(tr("Close Ot&her Tabs"), this, SLOT(closeAllButCurrent()));
			addAction(tr("Close Tabs To The Right"), this, SLOT(closeToRight()));
			addAction(tr("Close Tabs To The Left"), this, SLOT(closeToLeft()));
			addSeparator();
		}

		addAction(Application::getAppIcon("close"), tr("&Close Tab"), this, SLOT(closeTab()));
	}
	else {
		addAction(tr("&New Tab"), m_tabWidget->window(), SLOT(addTab()));
		addSeparator();
		addAction(tr("Reloa&d All Tabs"), m_tabWidget, SLOT(reloadAllTabs()));
		addAction(tr("Bookmark &All Tabs"), m_tabWidget->window(), SLOT(bookmarkAllTabs()));
		addSeparator();
	}
}

void TabContextMenu::pinTab()
{
	WebTab* webTab = qobject_cast<WebTab*>(m_tabWidget->widget(m_clickedTab));

	if (webTab)
		webTab->togglePinned();
}

void TabContextMenu::muteTab()
{
	WebTab* webTab = qobject_cast<WebTab*>(m_tabWidget->widget(m_clickedTab));

	if (webTab)
		webTab->toggleMuted();
}

void TabContextMenu::closeAllButCurrent()
{
	QMessageBox::StandardButton button = QMessageBox::question(this,
															   tr("Close Tabs"),
															   tr("Do you rally want to close other tabs?"),
															   QMessageBox::Yes | QMessageBox::No,
															   QMessageBox::No);

	if (button == QMessageBox::Yes)
		emit closeAllButCurrent(m_clickedTab);
}

void TabContextMenu::closeToRight()
{
	QMessageBox::StandardButton button = QMessageBox::question(this,
															   tr("Close Tabs"),
															   tr("Do you really want to close all tabs to right?"),
															   QMessageBox::Yes | QMessageBox::No,
															   QMessageBox::No);

	if (button == QMessageBox::Yes)
		emit closeToRight(m_clickedTab);
}

void TabContextMenu::closeToLeft()
{
	QMessageBox::StandardButton button = QMessageBox::question(this,
															   tr("Close Tabs"),
															   tr("Do you really want to close all tabs to left?"),
															   QMessageBox::Yes | QMessageBox::No,
															   QMessageBox::No);

	if (button == QMessageBox::Yes)
		emit closeToLeft(m_clickedTab);

}
}
