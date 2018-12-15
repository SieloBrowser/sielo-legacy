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

#include "MaquetteGridMenu.hpp"

#include "MaquetteGrid/MaquetteGridItem.hpp"
#include "MaquetteGrid/MaquetteGrid.hpp"
#include "MaquetteGrid/MaquetteGridManager.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
MaquetteGridMenu::MaquetteGridMenu(TabWidget* tabWidget) :
	QMenu(tabWidget),
	m_tabWidget(tabWidget)
{
	setTitle(tr("&MaquetteGrid"));

	addAction(tr("Open MaquetteGrid Manager"), this, &MaquetteGridMenu::openMaquetteGridManager);
	addSeparator();

	connect(this, &QMenu::aboutToShow, this, &MaquetteGridMenu::aboutToShow);

	connect(Application::instance()->maquetteGrid(), SIGNAL(maquetteGridAdded(MaquetteGridItem*)), this, SLOT(maquetteGridChanged()));
	connect(Application::instance()->maquetteGrid(), SIGNAL(maquetteGridRemoved(MaquetteGridItem*)), this, SLOT(maquetteGridChanged()));
	connect(Application::instance()->maquetteGrid(), SIGNAL(maquetteGridChanged(MaquetteGridItem*)), this, SLOT(maquetteGridChanged()));
}

MaquetteGridMenu::~MaquetteGridMenu()
{
	// Empty
}

void MaquetteGridMenu::aboutToShow()
{
	if (m_changed) {
		refresh();
		m_changed = false;
	}
}

void MaquetteGridMenu::maquetteGridChanged()
{
	m_changed = true;
}

void MaquetteGridMenu::openMaquetteGridManager()
{
	if (!m_manager && m_tabWidget) {
		m_manager = new MaquetteGridManager(m_tabWidget->window());
		m_tabWidget->addApplication(m_manager);
	}

	m_tabWidget->goToApplication(m_manager);
}

void MaquetteGridMenu::maquetteGridActivated()
{
	if (QAction* action = qobject_cast<QAction*>(sender())) {
		MaquetteGridItem* item{static_cast<MaquetteGridItem*>(action->data().value<void*>())};
		Q_ASSERT(item);
		openMaquetteGrid(item);
	}
}

void MaquetteGridMenu::openMaquetteGrid(MaquetteGridItem* item)
{
	Application::instance()->createWindow(item);
}

void MaquetteGridMenu::refresh()
{
	while (actions().count() != 1) {
		QAction* action{actions()[1]};
		removeAction(action);
		delete action;
	}

	foreach (MaquetteGridItem* item, Application::instance()->maquetteGrid()->maquetteGrid()) {
		QAction* action{new QAction(Application::getAppIcon("new-window"), item->name())};
		action->setData(QVariant::fromValue<void*>(static_cast<void*>(item)));

		addAction(action);

		connect(action, &QAction::triggered, this, &MaquetteGridMenu::maquetteGridActivated);
	}
}
}
