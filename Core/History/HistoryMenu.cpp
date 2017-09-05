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

#include "HistoryMenu.hpp"


#include "History/HistoryManager.hpp"
#include "History/HistoryDialog.hpp"
#include "History/HistoryMenuModel.hpp"
#include "HistoryModel.hpp"

#include "Application.hpp"

namespace Sn {

HistoryMenu::HistoryMenu(QWidget* parent) :
	ModelMenu(parent)
{
	connect(this, &ModelMenu::activated, this, &HistoryMenu::activated);

	setHoverRole(HistoryModel::UrlStringRole);
}

void HistoryMenu::setInitialActions(QList<QAction*> actions)
{
	m_initialActions = actions;

	for (int i{0}; i < m_initialActions.count(); ++i)
		addAction(m_initialActions[i]);
}

bool HistoryMenu::prePopulated()
{
	if (!m_history) {
		m_history = Application::instance()->historyManager();
		m_historyMenuModel = new HistoryMenuModel(m_history->historyTreeModel(), this);

		setModel(m_historyMenuModel);
	}

	for (int i{0}; i < m_initialActions.count(); ++i)
		addAction(m_initialActions[i]);

	if (!m_initialActions.isEmpty())
		addSeparator();

	setFirstSeparator(m_historyMenuModel->bumpedRows());

	return false;
}

void HistoryMenu::postPopulated()
{
	if (m_history->history().count() > 0)
		addSeparator();

	QAction* showAllAction{new QAction(tr("Show All History"), this)};
	QAction* clearAction{new QAction(tr("Clear History"), this)};

	connect(showAllAction, &QAction::triggered, this, &HistoryMenu::showHistoryDialog);
	connect(clearAction, &QAction::triggered, m_history, &HistoryManager::clear);

	addAction(showAllAction);
	addAction(clearAction);
}

void HistoryMenu::activated(const QModelIndex& index)
{
	emit openUrl(index.data(HistoryModel::UrlRole).toUrl());
}

void HistoryMenu::showHistoryDialog()
{
	HistoryDialog* dialog{new HistoryDialog(this)};

	connect(dialog, SIGNAL(openUrl(QUrl)), this, SIGNAL(openUrl(QUrl)));

	dialog->show();
}

}