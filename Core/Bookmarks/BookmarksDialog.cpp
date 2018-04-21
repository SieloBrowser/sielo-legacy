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

#include "BookmarksDialog.hpp"

#include <QFontMetrics>
#include <QHeaderView>

#include <QCursor>

#include <QMenu>

#include "Application.hpp"

#include "Utils/TreeProxyModel.hpp"

#include "View/TreeView.hpp"

#include "Widgets/SearchLineEdit.hpp"

#include "Bookmarks/BookmarkManager.hpp"
#include "Bookmarks/BookmarkNode.hpp"
#include "Bookmarks/BookmarksModel.hpp"

namespace Sn {
BookmarksDialog::BookmarksDialog(QWidget* parent, BookmarksManager* manager) :
	QDialog(parent),
	m_bookmarksManager(manager)
{
	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	setWindowFlags(flags);
	setWindowIcon(icon);

	if (!m_bookmarksManager)
		m_bookmarksManager = Application::instance()->bookmarksManager();

	setupUi();

	m_tree->setUniformRowHeights(true);
	m_tree->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tree->setSelectionMode(QAbstractItemView::ContiguousSelection);
	m_tree->setTextElideMode(Qt::ElideMiddle);

	m_bookmarksModel = m_bookmarksManager->bookmarksModel();
	m_proxyModel = new TreeProxyModel(this);
	m_proxyModel->setSourceModel(m_bookmarksModel);

	m_tree->setModel(m_proxyModel);
	m_tree->setDragDropMode(QAbstractItemView::InternalMove);
	m_tree->setExpanded(m_proxyModel->index(0, 0), true);
	m_tree->setAlternatingRowColors(true);

	QFontMetrics fontMetrics{font()};
	int header{fontMetrics.width(QLatin1Char('m')) * 40};

	m_tree->header()->resizeSection(0, header);
	m_tree->header()->setStretchLastSection(true);
	m_tree->setContextMenuPolicy(Qt::CustomContextMenu);

	expandNodes(m_bookmarksManager->bookmarks());

	setAttribute(Qt::WA_DeleteOnClose);

	connect(m_search, &SearchLineEdit::textChanged, m_proxyModel, &TreeProxyModel::setFilterFixedString);
	connect(m_removeButton, &QPushButton::clicked, this, &BookmarksDialog::removeBookmark);
	connect(m_addFolderButton, &QPushButton::clicked, this, &BookmarksDialog::newFolder);
	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &BookmarksDialog::accept);
	connect(m_tree, &TreeView::activated, this, &BookmarksDialog::open);
	connect(m_tree, &TreeView::customContextMenuRequested, this, &BookmarksDialog::customContextMenuRequested);
}

BookmarksDialog::~BookmarksDialog()
{
	if (saveExpandedNodes(m_tree->rootIndex()))
		m_bookmarksManager->changeExpanded();
}

void BookmarksDialog::customContextMenuRequested(const QPoint& pos)
{
	QMenu menu{};
	QModelIndex index{m_tree->indexAt(pos)};

	index = index.sibling(index.row(), 0);

	if (index.isValid() && !m_tree->model()->hasChildren(index)) {
		menu.addAction(tr("Open"), this, &BookmarksDialog::open);
		menu.addSeparator();
	}

	menu.addAction(tr("Delete"), this, &BookmarksDialog::removeBookmark);

	menu.exec(QCursor::pos());
}

void BookmarksDialog::open()
{
	QModelIndex index{m_tree->currentIndex()};

	if (!index.parent().isValid())
		return;

	emit openUrl(index.sibling(index.row(), 1).data(BookmarksModel::UrlRole).toUrl());
}

void BookmarksDialog::newFolder()
{
	QModelIndex currentIndex{m_tree->currentIndex()};
	QModelIndex idx{currentIndex};

	if (idx.isValid() && !idx.model()->hasChildren(idx))
		idx = idx.parent();

	if (!idx.isValid())
		idx = m_tree->rootIndex();

	idx = m_proxyModel->mapToSource(idx);

	BookmarkNode* parent{m_bookmarksManager->bookmarksModel()->node(idx)};
	BookmarkNode* node{new BookmarkNode(BookmarkNode::Folder)};

	node->title = tr("New Folder");
	m_bookmarksManager->addBookmark(parent, node, currentIndex.row() + 1);
}

void BookmarksDialog::removeBookmark()
{
	QModelIndex currentIndex{m_tree->currentIndex()};

	if (!currentIndex.isValid() || currentIndex == m_tree->rootIndex())
		return;

	currentIndex = m_proxyModel->mapToSource(currentIndex);

	m_bookmarksManager->removeBookmark(m_bookmarksManager->bookmarksModel()->node(currentIndex));
}

void BookmarksDialog::expandNodes(BookmarkNode* node)
{
	for (BookmarkNode* chidlNode : node->children()) {
		if (chidlNode->expanded) {
			QModelIndex idx{m_bookmarksModel->index(chidlNode)};
			idx = m_proxyModel->mapFromSource(idx);
			m_tree->setExpanded(idx, true);
			expandNodes(chidlNode);
		}
	}
}

bool BookmarksDialog::saveExpandedNodes(const QModelIndex& parent)
{
	bool changed{false};

	for (int i{0}; i < m_proxyModel->rowCount(parent); ++i) {
		QModelIndex child{m_proxyModel->index(i, 0, parent)};
		QModelIndex sourceIndex{m_proxyModel->mapToSource(child)};
		BookmarkNode* childNode{m_bookmarksModel->node(sourceIndex)};
		bool wasExpanded{childNode->expanded};

		if (m_tree->isExpanded(child)) {
			childNode->expanded = true;
			changed |= saveExpandedNodes(child);
		}
		else
			childNode->expanded = false;

		changed |= (wasExpanded != childNode->expanded);
	}

	return changed;
}

void BookmarksDialog::setupUi()
{
	resize(758, 450);
	setWindowTitle(tr("Bookmarks"));

	m_layout = new QGridLayout(this);

	m_searchSpacer = new QSpacerItem(252, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_search = new SearchLineEdit(this);

	m_tree = new TreeView(this);

	m_buttonLayout = new QHBoxLayout();

	m_removeButton = new QPushButton(tr("&Remove"), this);
	m_addFolderButton = new QPushButton(tr("Add Folder"), this);

	m_buttonSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);

	m_buttonLayout->addWidget(m_removeButton);
	m_buttonLayout->addWidget(m_addFolderButton);
	m_buttonLayout->addItem(m_buttonSpacer);
	m_buttonLayout->addWidget(m_buttonBox);

	m_layout->addItem(m_searchSpacer, 0, 0, 1, 1);
	m_layout->addWidget(m_search, 0, 1, 1, 1);
	m_layout->addWidget(m_tree, 1, 0, 1, 2);
	m_layout->addLayout(m_buttonLayout, 2, 0, 1, 2);
}
}