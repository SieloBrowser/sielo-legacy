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
#ifndef SIELOBROWSER_TABCONTEXTMENU_HPP
#define SIELOBROWSER_TABCONTEXTMENU_HPP

#include "SharedDefines.hpp"

#include <QMenu>

namespace Sn
{

class TabWidget;

class SIELO_SHAREDLIB TabContextMenu: public QMenu {
	Q_OBJECT

public:
	enum Option {
		InvalidOption = 0,
		HorizontalTabs = 1 << 0,
		VerticalTabs = 1 << 1,
		ShowCloseOtherTabsActions = 1 << 2,
		ShowDetachTabAction = 1 << 3,

		DefaultOptions = HorizontalTabs | ShowCloseOtherTabsActions
	};
	Q_DECLARE_FLAGS(Options, Option)

	TabContextMenu(int index, TabWidget* tabWidget, Options options = DefaultOptions);
	~TabContextMenu() = default;

signals:
	void reloadTab(int index);
	void stopTab(int index);
	void tabCloseRequested(int index);
	void closeAllButCurrent(int index);
	void closeToRight(int index);
	void closeToLeft(int index);
	void duplicateTab(int index);
	void detachTab(int index);
	void loadTab(int index);
	void unloadTab(int index);

private slots:
	void reloadTab() { emit reloadTab(m_clickedTab); }
	void stopTab() { emit stopTab(m_clickedTab); }
	void closeTab() { emit tabCloseRequested(m_clickedTab); }
	void duplicateTab() { emit duplicateTab(m_clickedTab); }
	void detachTab() { emit detachTab(m_clickedTab); }
	void loadTab() { emit loadTab(m_clickedTab); }
	void unloadTab() { emit unloadTab(m_clickedTab); }

	void pinTab();
	void muteTab();

	void closeAllButCurrent();
	void closeToRight();
	void closeToLeft();

private:
	int m_clickedTab{0};
	TabWidget* m_tabWidget{nullptr};
	Options m_options{InvalidOption};
};
}

#endif //SIELOBROWSER_TABCONTEXTMENU_HPP