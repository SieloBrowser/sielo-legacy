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

#include "MockupsMenu.hpp"

#include "Mockup/MockupItem.hpp"
#include "Mockup/Mockups.hpp"
#include "Mockup/MockupsManager.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
MockupsMenu::MockupsMenu(BrowserWindow* window) :
	QMenu(window),
	m_window(window)
{
	setTitle(tr("&Mockups"));

	addAction(tr("Open Mockups Manager"), this, &MockupsMenu::openMockupManager);
	addSeparator();

	connect(this, &QMenu::aboutToShow, this, &MockupsMenu::aboutToShow);

	connect(Application::instance()->mockups(), SIGNAL(mockupAdded(MockupItem*)), this, SLOT(mockupsChanged()));
	connect(Application::instance()->mockups(), SIGNAL(mockupRemoved(MockupItem*)), this, SLOT(mockupsChanged()));
	connect(Application::instance()->mockups(), SIGNAL(mockupChanged(MockupItem*)), this, SLOT(mockupsChanged()));
}

MockupsMenu::~MockupsMenu()
{
	// Empty
}

void MockupsMenu::aboutToShow()
{
	if (m_changed) {
		refresh();
		m_changed = false;
	}
}

void MockupsMenu::mockupsChanged()
{
	m_changed = true;
}

void MockupsMenu::openMockupManager()
{
	MockupsManager* manager{new MockupsManager(m_window)};
	manager->show();
}

void MockupsMenu::mockupActivated()
{
	if (QAction* action = qobject_cast<QAction*>(sender())) {
		MockupItem* item{static_cast<MockupItem*>(action->data().value<void*>())};
		Q_ASSERT(item);
		openMockup(item);
	}
}

void MockupsMenu::openMockup(MockupItem* item)
{
	Application::instance()->createWindow(item);
}

void MockupsMenu::refresh()
{
	while (actions().count() != 1) {
		QAction* action{actions()[1]};
		removeAction(action);
		delete action;
	}

	foreach (MockupItem* item, Application::instance()->mockups()->mockups()) {
		QAction* action{new QAction(Application::getAppIcon("new-window"), item->name())};
		action->setData(QVariant::fromValue<void*>(static_cast<void*>(item)));

		addAction(action);

		connect(action, &QAction::triggered, this, &MockupsMenu::mockupActivated);
	}
}
}
