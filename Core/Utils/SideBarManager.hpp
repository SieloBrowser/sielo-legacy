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
#ifndef SIELOBROWSER_SIDEBARMANAGER_HPP
#define SIELOBROWSER_SIDEBARMANAGER_HPP

#include "SharedDefines.hpp"

#include <QObject>
#include <QMenu>

#include <QString>
#include <QHash>

#include <QPointer>

namespace Sn
{
class TabStackedWidget;

class SideBar;
class SideBarInterface;

class SIELO_SHAREDLIB SideBarManager: public QObject {
	Q_OBJECT

public:
	SideBarManager(TabStackedWidget* tabWidget);
	~SideBarManager() = default;

	QString activeSideBar() const { return m_activeBar; }

	void createMenu(QMenu* menu);

	void showSideBar(const QString& id, bool toggle = true);
	void sideBarRemoved(const QString& id);
	void closeSideBar();

private slots:
	void sShowSideBar();

private:
	TabStackedWidget* m_tabWidget{nullptr};
	QPointer<SideBar> m_sideBar{};
	QMenu* m_menu{nullptr};

	QString m_activeBar{};
};
}

#endif //SIELOBROWSER_SIDEBARMANAGER_HPP