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

#include "BookmarksToolBar.hpp"

#include <QMimeData>

#include <QToolButton>

#include "Widgets/ModelMenu.hpp"

#include "Bookmarks/BookmarksModel.hpp"
#include "Bookmarks/BookmarkNode.hpp"
#include "Bookmarks/BookmarkManager.hpp"

namespace Sn {
BookmarksToolBar::BookmarksToolBar(Sn::BookmarksModel* model, QWidget* parent) :
	QToolBar(tr("Bookmark"), parent),
	m_bookmarksModel(model)
{
	setObjectName(QLatin1String("bookmarks-toolbar"));
	setRootIndex(model->index(0, 0));
	setAcceptDrops(true);

	connect(this, &QToolBar::actionTriggered, this, &BookmarksToolBar::triggered);

	connect(m_bookmarksModel, &BookmarksModel::modelReset, this, &BookmarksToolBar::build);
	connect(m_bookmarksModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(build()));
	connect(m_bookmarksModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(build()));
	connect(m_bookmarksModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(build()));
}

void BookmarksToolBar::setRootIndex(const QModelIndex& index)
{
	m_root = index;
	build();
}

void BookmarksToolBar::dragEnterEvent(QDragEnterEvent* event)
{
	const QMimeData* mimeData{event->mimeData()};

	if (mimeData->hasUrls())
		event->acceptProposedAction();

	QToolBar::dragEnterEvent(event);
}

void BookmarksToolBar::dropEvent(QDropEvent* event) {
	const QMimeData* mimeData{event->mimeData()};

	if (mimeData->hasUrls() && mimeData->hasText()) {
		QList<QUrl> urls = mimeData->urls();
		QAction* action{actionAt(event->pos())};
		QString dropText;

		if (action)
			dropText = action->text();

		int row{-1};
		QModelIndex parentIndex = m_root;

		for (int i{0}; i < m_bookmarksModel->rowCount(m_root); ++i) {
			QModelIndex idx{m_bookmarksModel->index(i, 0, m_root)};
			QString title = idx.data().toString();

			if (title == dropText) {
				row = i;
				if (m_bookmarksModel->hasChildren(idx)) {
					parentIndex = idx;
					row = -1;
				}
				break;
			}
		}

		BookmarkNode* bookmark{new BookmarkNode(BookmarkNode::Bookmark)};
		bookmark->url = urls[0].toString();
		bookmark->title = mimeData->text();

		BookmarkNode* parent{m_bookmarksModel->node(parentIndex)};
		BookmarksManager* bookmarksManager{m_bookmarksModel->bookmarksManager()};
		bookmarksManager->addBookmark(parent, bookmark, row);

		event->acceptProposedAction();
	}

	QToolBar::dropEvent(event);
}

void BookmarksToolBar::triggered(QAction* action)
{
	QVariant variant{action->data()};

	if (variant.canConvert<QUrl>())
		emit openUrl(variant.toUrl());
}

void BookmarksToolBar::activated(const QModelIndex& index)
{
	emit openUrl(index.data(BookmarksModel::UrlRole).toUrl());
}

void BookmarksToolBar::build()
{
	clear();

	for (int i{0}; i < m_bookmarksModel->rowCount(m_root); ++i) {
		QModelIndex idx{m_bookmarksModel->index(i, 0, m_root)};

		if (m_bookmarksModel->hasChildren(idx)) {
			QToolButton* button{new QToolButton(this)};
			button->setPopupMode(QToolButton::InstantPopup);
			button->setArrowType(Qt::DownArrow);
			button->setText(idx.data().toString());
			button->setToolButtonStyle(Qt::ToolButtonTextOnly);

			ModelMenu* menu{new ModelMenu(this)};
			menu->setModel(m_bookmarksModel);
			menu->setRootIndex(idx);
			menu->addAction(new QAction(menu));
			connect(menu, &ModelMenu::activated, this, &BookmarksToolBar::activated);

			button->setMenu(menu);

			QAction* action{addWidget(button)};
			action->setText(idx.data().toString());
		}
		else {
			QAction* action{addAction(idx.data().toString())};
			action->setData(idx.data(BookmarksModel::UrlRole));
		}
	}
}

}