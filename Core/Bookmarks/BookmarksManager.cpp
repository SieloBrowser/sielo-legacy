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

#include "BookmarksManager.hpp"

#include <QHeaderView>

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarkItem.hpp"
#include "Bookmarks/BookmarksTreeView.hpp"
#include "Bookmarks/BookmarksUtils.hpp"
#include "Bookmarks/BookmarksImport/BookmarksImportDialog.hpp"
#include "Bookmarks/BookmarksExport/BookmarksExportDialog.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
BookmarksManager::BookmarksManager(BrowserWindow* window, QWidget* parent) :
	QDialog(parent),
	m_window(window),
	m_bookmarks(Application::instance()->bookmarks())
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowIcon(Application::getAppIcon("bookmarks"));
	setWindowTitle(tr("Bookmarks"));

	setupUI();

	connect(m_view, &BookmarksTreeView::bookmarkActivated, this, &BookmarksManager::bookmarkActivated);
	connect(m_view, &BookmarksTreeView::bookmarkCtrlActivated, this, &BookmarksManager::bookmarkCtrlActivated);
	connect(m_view, &BookmarksTreeView::bookmarkShiftActivated, this, &BookmarksManager::bookmarkShiftActivated);
	connect(m_view, &BookmarksTreeView::bookmarksSelected, this, &BookmarksManager::bookmarksSelected);
	connect(m_view, &BookmarksTreeView::contextMenuRequested, this, &BookmarksManager::createContextMenu);

	updateEditBox(nullptr);

	connect(m_search, &QLineEdit::textEdited, this, &BookmarksManager::search);
	connect(m_title, &QLineEdit::textEdited, this, &BookmarksManager::bookmarkEdited);
	connect(m_address, &QLineEdit::textEdited, this, &BookmarksManager::bookmarkEdited);
	connect(m_keyword, &QLineEdit::textEdited, this, &BookmarksManager::bookmarkEdited);
	connect(m_description, &QPlainTextEdit::textChanged, this, &BookmarksManager::descriptionEdited);

	QMenu* m = new QMenu(this);
	m->addAction(tr("Import Bookmarks..."), this, &BookmarksManager::importBookmarks);
	m->addAction(tr("Export Bookmarks..."), this, &BookmarksManager::exportBookmarks);
	m_importExport->setMenu(m);
}

BookmarksManager::~BookmarksManager()
{
	// Empty
}

void BookmarksManager::setMainWindow(BrowserWindow* window)
{
	if (window)
		m_window = window;
}

void BookmarksManager::search(const QString& string)
{
	m_view->search(string);
}

void BookmarksManager::bookmarkActivated(BookmarkItem* item)
{
	openBookmark(item);
}

void BookmarksManager::bookmarkCtrlActivated(BookmarkItem* item)
{
	openBookmarkInNewTab(item);
}

void BookmarksManager::bookmarkShiftActivated(BookmarkItem* item)
{
	openBookmarkInNewWindow(item);
}

void BookmarksManager::bookmarksSelected(const QList<BookmarkItem*>& items)
{
	if (items.size() != 1) {
		m_selectedBookmark = nullptr;
		updateEditBox(nullptr);
	}
	else {
		m_selectedBookmark = items[0];
		updateEditBox(m_selectedBookmark);
	}
}

void BookmarksManager::createContextMenu(const QPoint& pos)
{
	QMenu menu{};
	QAction* actionNewTab{menu.addAction(tr("Open in new tab"))};
	QAction* actionNewWindow{menu.addAction(Application::getAppIcon("new-window"), tr("Open in new window"))};
	QAction* actionNewPrivateWindow{menu.addAction(tr("Open in new private window"))};

	menu.addSeparator();
	menu.addAction(tr("New Bookmark"), this, SLOT(addBookmark())); // What a strange build error...
	menu.addAction(tr("New Folder"), this, &BookmarksManager::addFolder);
	menu.addAction(tr("New Separator"), this, &BookmarksManager::addSeparator);
	menu.addSeparator();

	QAction* actionDelete = menu.addAction(tr("Delete"));

	connect(actionNewTab, SIGNAL(triggered()), this, SLOT(openBookmarkInNewTab()));
	connect(actionNewWindow, SIGNAL(triggered()), this, SLOT(openBookmarkInNewWindow()));
	connect(actionNewPrivateWindow, SIGNAL(triggered()), this, SLOT(openBookmarkInNewPrivateWindow()));
	connect(actionDelete, &QAction::triggered, this, &BookmarksManager::deleteBookmarks);

	bool canBeDeleted{false};
	QList<BookmarkItem*> items = m_view->selectedBookmarks();

	foreach(BookmarkItem* item, items) {
		if (m_bookmarks->canBeModified(item)) {
			canBeDeleted = true;
			break;
		}
	}

	if (!canBeDeleted)
		actionDelete->setEnabled(false);

	if (!m_selectedBookmark || !m_selectedBookmark->isUrl()) {
		actionNewTab->setEnabled(false);
		actionNewWindow->setEnabled(false);
		actionNewPrivateWindow->setEnabled(false);
	}

	menu.exec(pos);
}

void BookmarksManager::importBookmarks()
{
	BookmarksImportDialog* dialog{new BookmarksImportDialog(this)};
	dialog->open();
}

void BookmarksManager::exportBookmarks()
{
	BookmarksExportDialog* dialog{new BookmarksExportDialog(this)};
	dialog->open();
}

void BookmarksManager::openBookmark(BookmarkItem* item)
{
	item = item ? item : m_selectedBookmark;

	BookmarksUtils::openBookmark(getWindow(), item);
}

void BookmarksManager::openBookmarkInNewTab(BookmarkItem* item)
{
	item = item ? item : m_selectedBookmark;

	BookmarksUtils::openBookmarkInNewTab(getWindow(), item);
}

void BookmarksManager::openBookmarkInNewWindow(BookmarkItem* item)
{
	item = item ? item : m_selectedBookmark;

	BookmarksUtils::openBookmarkInNewWindow(item);
}

void BookmarksManager::openBookmarkInNewPrivateWindow(BookmarkItem* item)
{
	item = item ? item : m_selectedBookmark;

	BookmarksUtils::openBookmarkInNewPrivateWindow(item);
}

void BookmarksManager::addBookmark()
{
	BookmarkItem* item{new BookmarkItem(BookmarkItem::Url)};

	item->setTitle(tr("New Bookmark"));
	item->setUrl(QUrl("https://"));

	addBookmark(item);
}

void BookmarksManager::addFolder()
{
	BookmarkItem* item{new BookmarkItem(BookmarkItem::Folder)};

	item->setTitle(tr("New Folder"));

	addBookmark(item);
}

void BookmarksManager::addSeparator()
{
	BookmarkItem* item{new BookmarkItem(BookmarkItem::Separator)};

	addBookmark(item);
}

void BookmarksManager::deleteBookmarks()
{
	QList<BookmarkItem*> items = m_view->selectedBookmarks();

	foreach(BookmarkItem* item, items) {
		if (m_bookmarks->canBeModified(item))
			m_bookmarks->removeBookmark(item);
	}
}

void BookmarksManager::bookmarkEdited()
{
	Q_ASSERT(m_view->selectedBookmarks().count() == 1);

	BookmarkItem* item = m_view->selectedBookmarks()[0];

	item->setTitle(m_title->text());
	item->setUrl(QUrl::fromEncoded(m_address->text().toUtf8()));
	item->setKeyword(m_keyword->text());
	item->setDescription(m_description->toPlainText());

	m_bookmarks->changeBookmark(item);
}

void BookmarksManager::descriptionEdited()
{
	if (!m_blockDescriptionChangedSignal)
		bookmarkEdited();
}

void BookmarksManager::enableUpdates()
{
	setUpdatesEnabled(true);
}

void BookmarksManager::setupUI()
{
	resize(689, 486);

	m_editBox = new QGroupBox(this);

	m_layout = new QGridLayout(this);
	m_editLayout = new QFormLayout(m_editBox);

	m_importExport = new QPushButton(tr("Import and export"), this);
	m_search = new QLineEdit(this);
	m_search->setPlaceholderText(tr("Search..."));

	m_view = new BookmarksTreeView(this);

	m_titleDesc = new QLabel(tr("Title:"), this);
	m_title = new QLineEdit(this);
	m_addressDesc = new QLabel(tr("Address:"), this);
	m_address = new QLineEdit(this);
	m_keywordDesc = new QLabel(tr("Keyword:"), this);
	m_keyword = new QLineEdit(this);
	m_descriptionDesc = new QLabel(tr("Description:"), this);
	m_description = new QPlainTextEdit(this);

	m_editLayout->setWidget(0, QFormLayout::LabelRole, m_titleDesc);
	m_editLayout->setWidget(0, QFormLayout::FieldRole, m_title);
	m_editLayout->setWidget(1, QFormLayout::LabelRole, m_addressDesc);
	m_editLayout->setWidget(1, QFormLayout::FieldRole, m_address);
	m_editLayout->setWidget(2, QFormLayout::LabelRole, m_keywordDesc);
	m_editLayout->setWidget(2, QFormLayout::FieldRole, m_keyword);
	m_editLayout->setWidget(3, QFormLayout::LabelRole, m_descriptionDesc);
	m_editLayout->setWidget(3, QFormLayout::FieldRole, m_description);

	m_layout->addWidget(m_importExport, 0, 0, 1, 1);
	m_layout->addWidget(m_search, 0, 1, 1, 1);
	m_layout->addWidget(m_view, 1, 0, 1, 2);
	m_layout->addWidget(m_editBox, 2, 0, 1, 2);
}

void BookmarksManager::updateEditBox(BookmarkItem* item)
{
	setUpdatesEnabled(false);
	m_blockDescriptionChangedSignal = true;

	bool editable{bookmarkEditable(item)};
	bool showAddressAndKeyword{item && item->isUrl()};

	if (!item) {
		m_title->clear();
		m_address->clear();
		m_keyword->clear();
		m_description->clear();
	}
	else {
		m_title->setText(item->title());
		m_address->setText(item->url().toEncoded());
		m_keyword->setText(item->keyword());
		m_description->setPlainText(item->description());

		m_title->setCursorPosition(0);
		m_address->setCursorPosition(0);
		m_keyword->setCursorPosition(0);
		m_description->moveCursor(QTextCursor::Start);
	}

	m_title->setReadOnly(!editable);
	m_address->setReadOnly(!editable);
	m_keyword->setReadOnly(!editable);
	m_description->setReadOnly(!editable);

	m_addressDesc->setVisible(showAddressAndKeyword);
	m_address->setVisible(showAddressAndKeyword);
	m_keywordDesc->setVisible(showAddressAndKeyword);
	m_keyword->setVisible(showAddressAndKeyword);

	if (showAddressAndKeyword) {
		// Show Address + Keyword
		m_editLayout->insertRow(1, m_addressDesc, m_address);
		m_editLayout->insertRow(2, m_keywordDesc, m_keyword);
	}
	else {
		// Hide Address + Keyword
		m_editLayout->removeWidget(m_addressDesc);
		m_editLayout->removeWidget(m_keywordDesc);
		m_editLayout->removeWidget(m_address);
		m_editLayout->removeWidget(m_keyword);
	}

	m_blockDescriptionChangedSignal = false;

	setUpdatesEnabled(true);
}

bool BookmarksManager::bookmarkEditable(BookmarkItem* item) const
{
	return item && (item->isFolder() || item->isUrl()) && m_bookmarks->canBeModified(item);
}

void BookmarksManager::addBookmark(BookmarkItem* item)
{
	BookmarkItem* parent{parentForNewBookmark()};
	Q_ASSERT(parent);

	m_bookmarks->insertBookmark(parent, 0, item);

	m_view->selectBookmark(item);
	m_view->ensureBookmarkVisible(item);

	if (!item->isSeparator()) {
		m_title->setFocus();
		m_title->selectAll();
	}
}

BookmarkItem *BookmarksManager::parentForNewBookmark() const
{
	if (m_selectedBookmark && m_selectedBookmark->isFolder())
		return m_selectedBookmark;
	if (!m_selectedBookmark || m_selectedBookmark->parent() == m_bookmarks->rootItem())
		return m_bookmarks->unsortedFolder();

	return m_selectedBookmark->parent();
}

BrowserWindow *BookmarksManager::getWindow() const
{
	if (!m_window)
		return Application::instance()->getWindow();

	return m_window.data();
}

void BookmarksManager::showEvent(QShowEvent* event)
{
	QDialog::showEvent(event);

	if (m_adjustHeaderSizesOnShow) {
		m_view->header()->resizeSection(0, m_view->header()->width() / 1.9);
		m_adjustHeaderSizesOnShow = false;
	}
}

void BookmarksManager::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
	case Qt::Key_Delete:
		deleteBookmarks();
		break;
	}

	QDialog::keyPressEvent(event);
}
}
