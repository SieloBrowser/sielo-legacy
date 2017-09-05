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

#pragma once
#ifndef SIELO_BROWSER_HISTORYMENU_HPP
#define SIELO_BROWSER_HISTORYMENU_HPP

#include <QWidget>

#include <QAction>

#include <QList>

#include "Widgets/ModelMenu.hpp"
#include "HistoryMenuModel.hpp"

namespace Sn {
class HistoryManager;
class HistoryMenuModel;

class HistoryMenu: public ModelMenu {
Q_OBJECT

public:
	HistoryMenu(QWidget* parent = 0);

	void setInitialActions(QList<QAction*> actions);

signals:
	void openUrl(const QUrl& url);

protected:
	bool prePopulated();
	void postPopulated();

private slots:
	void activated(const QModelIndex& index);
	void showHistoryDialog();

private:
	HistoryManager* m_history{nullptr};
	HistoryMenuModel* m_historyMenuModel{nullptr};
	QList<QAction*> m_initialActions{};
};

}

#endif //SIELO_BROWSER_HISTORYMENU_HPP
