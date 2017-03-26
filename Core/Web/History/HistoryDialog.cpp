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

#include "Web/History/HistoryDialog.hpp"

#include <QAbstractItemModel>

#include <QFontMetrics>
#include <QCursor>
#include <QClipboard>

#include <QMenu>

#include "Application.hpp"

#include "Web/History/HistoryManager.hpp"
#include "Web/History/HistoryFilterModel.hpp"
#include "Web/History/HistoryTreeModel.hpp"
#include "Web/History/TreeProxyModel.hpp"
#include "HistoryModel.hpp"

namespace Sn {
HistoryDialog::HistoryDialog(QWidget* parent, HistoryManager* setHistory) :
	QDialog(parent)
{
	HistoryManager* history = setHistory;

	if (!history)
		history = Application::instance()->historyManager();

	setupUi();

	m_tree->setUniformRowHeights(true);
	m_tree->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tree->setTextElideMode(Qt::ElideMiddle);

	QAbstractItemModel* model{history->historyFilterModel()};
	TreeProxyModel* proxyModel{new TreeProxyModel(this)};

	proxyModel->setSourceModel(model);
	proxyModel->setFilterKeyColumn(1);

	m_tree->setModel(proxyModel);
	m_tree->setSortingEnabled(true);
	m_tree->setExpanded(proxyModel->index(0, 0), true);
	m_tree->setAlternatingRowColors(true);
	m_tree->setContextMenuPolicy(Qt::CustomContextMenu);

	QFontMetrics fm{font()};
	int header{fm.width(QLatin1Char('m')) * 40};
	m_tree->header()->resizeSection(0, header);
	m_tree->header()->setStretchLastSection(true);

	connect(m_tree, &TreeView::activated, this, &HistoryDialog::open);
	connect(m_tree, &TreeView::customContextMenuRequested, this, &HistoryDialog::customContextMenuRequested);
	connect(m_removeButton, &QPushButton::clicked, m_tree, &TreeView::removeOne);
	connect(m_removeAllButton, &QPushButton::clicked, m_tree, &TreeView::removeAll);
}

void HistoryDialog::setupUi()
{
	resize(758, 450);
	setWindowTitle(tr("History"));

	m_layout = new QGridLayout(this);

	m_searchSpacer = new QSpacerItem(252, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_searchLineEdit = new QLineEdit(this);
	m_searchLineEdit->setPlaceholderText(tr("Not working for now"));

	m_tree = new TreeView(this);

	m_buttonLayout = new QHBoxLayout();

	m_removeButton = new QPushButton(tr("&Remove"), this);
	m_removeAllButton = new QPushButton(tr("Remove &All"), this);

	m_buttonSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);

	m_buttonLayout->addWidget(m_removeButton);
	m_buttonLayout->addWidget(m_removeAllButton);
	m_buttonLayout->addItem(m_buttonSpacer);
	m_buttonLayout->addWidget(m_buttonBox);

	m_layout->addItem(m_searchSpacer, 0, 0, 1, 1);
	m_layout->addWidget(m_searchLineEdit, 0, 1, 1, 1);
	m_layout->addWidget(m_tree, 1, 0, 1, 2);
	m_layout->addLayout(m_buttonLayout, 2, 0, 1, 2);

	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &HistoryDialog::accepted);

}

void HistoryDialog::customContextMenuRequested(const QPoint& pos)
{
	QMenu menu{};
	QModelIndex index{m_tree->indexAt(pos)};
	index = index.sibling(index.row(), 0);

	if (index.isValid() && !m_tree->model()->hasChildren(index)) {
		menu.addAction(tr("Open"), this, &HistoryDialog::open);
		menu.addSeparator();
		menu.addAction(tr("Copy"), this, &HistoryDialog::copy);
	}
	menu.addAction(tr("Delete"), m_tree, &TreeView::removeOne);

	menu.exec(QCursor::pos());
}

void HistoryDialog::open()
{
	QModelIndex index{m_tree->currentIndex()};

	if (!index.parent().isValid())
		return;

	emit openUrl(index.data(HistoryModel::UrlRole).toUrl());
}

void HistoryDialog::copy()
{
	QModelIndex index{m_tree->currentIndex()};

	if (!index.parent().isValid())
		return;

	QString url{index.data(HistoryModel::UrlStringRole).toString()};

	QClipboard* clipboard{QApplication::clipboard()};
	clipboard->setText(url);
}
}