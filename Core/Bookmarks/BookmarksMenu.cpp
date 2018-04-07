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

#include "BookmarksMenu.hpp"


#include "Bookmarks/BookmarkManager.hpp"
#include "Bookmarks/BookmarksModel.hpp"

#include "Application.hpp"

namespace Sn {

BookmarksMenu::BookmarksMenu(QWidget* parent) :
	ModelMenu(parent)
{
	connect(this, &ModelMenu::activated, this, &BookmarksMenu::activated);

	setMaxRows(-1);
	setHoverRole(BookmarksModel::UrlStringRole);
	setSeparatorRole(BookmarksModel::SeparatorRole);
}

void BookmarksMenu::setInitialActions(QList<QAction*> actions)
{
	m_initialActions = actions;

	for (int i{0}; i < m_initialActions.count(); ++i)
		addAction(m_initialActions[i]);
}

bool BookmarksMenu::prePopulated()
{
	m_bookmarksManager = Application::instance()->bookmarksManager();

	setModel(m_bookmarksManager->bookmarksModel());
	setRootIndex(QModelIndex());

	for (int i{0}; i < m_initialActions.count(); ++i)
		addAction(m_initialActions[i]);

	if (!m_initialActions.isEmpty())
		addSeparator();

	return true;
}

void BookmarksMenu::activated(const QModelIndex& index)
{
	emit openUrl(index.data(BookmarksModel::UrlRole).toUrl());
}

}