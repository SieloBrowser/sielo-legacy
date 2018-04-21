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

#include "Bookmarks/AddBookmarkDialog.hpp"

#include <QTreeView>
#include <QHeaderView>

#include "Bookmarks/BookmarkManager.hpp"
#include "Bookmarks/AddBookmarkProxyModel.hpp"
#include "Bookmarks/BookmarksModel.hpp"
#include "Bookmarks/BookmarkNode.hpp"

#include "Application.hpp"

namespace Sn {

AddBookmarkDialog::AddBookmarkDialog(const QString& url, const QString& title, QWidget* parent,
									 BookmarksManager* bookmarksManager) :
	QDialog(parent),
	m_bookmarksManager(bookmarksManager)
{
	if (!m_bookmarksManager)
		m_bookmarksManager = Application::instance()->bookmarksManager();

	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag) & Qt::Sheet;
	setWindowFlags(flags);
	setWindowIcon(icon);
	setupUI();

	QTreeView* view{new QTreeView(this)};
	BookmarksModel* model = m_bookmarksManager->bookmarksModel();

	m_proxyModel = new AddBookmarkProxyModel(this);
	m_proxyModel->setSourceModel(model);

	view->setModel(m_proxyModel);
	view->expandAll();
	view->header()->setStretchLastSection(true);
	view->header()->hide();
	view->setItemsExpandable(false);
	view->setRootIsDecorated(false);
	view->setIndentation(10);
	view->show();

	m_location->setModel(m_proxyModel);
	m_location->setView(view);

	m_name->setText(title);
        m_urlEdit->setText(url);

	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &AddBookmarkDialog::accept);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &AddBookmarkDialog::reject);
}

void AddBookmarkDialog::accept()
{
	QModelIndex index{m_location->view()->currentIndex()};

	index = m_proxyModel->mapToSource(index);

	if (!index.isValid())
		index = m_bookmarksManager->bookmarksModel()->index(0, 0);

	BookmarkNode* parent{m_bookmarksManager->bookmarksModel()->node(index)};
	BookmarkNode* bookmark{new BookmarkNode(BookmarkNode::Bookmark)};

	bookmark->url = m_urlEdit->text();
	bookmark->title = m_name->text();

	m_bookmarksManager->addBookmark(parent, bookmark);

	QDialog::accept();
}

void AddBookmarkDialog::setupUI()
{
	resize(240, 168);

	m_layout = new QVBoxLayout(this);

	m_description = new QLabel(tr("Type a name for the bookmark and choose where to keep it"), this);
	m_description->setTextFormat(Qt::PlainText);
	m_description->setWordWrap(true);

	m_name = new QLineEdit(this);
	m_name->setPlaceholderText(tr("Name of bookmark"));
	
	m_urlEdit = new QLineEdit(this);

	m_location = new QComboBox(this);

	m_spacer = new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal, this);
	m_buttonBox->setCenterButtons(false);

	m_layout->addWidget(m_description);
	m_layout->addWidget(m_name);
	m_layout->addWidget(m_urlEdit);
	m_layout->addWidget(m_location);
	m_layout->addItem(m_spacer);
	m_layout->addWidget(m_buttonBox);
}

}
