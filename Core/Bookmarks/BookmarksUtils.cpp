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

#include "BookmarksUtils.hpp"

#include <QDialog>

#include <QVBoxLayout>
#include <QFormLayout>

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDialogButtonBox>

#include <QMessageBox>

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarkItem.hpp"
#include "Bookmarks/BookmarksFoldersButton.hpp"

#include "Utils/IconProvider.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
bool BookmarksUtils::addBookmarkDialog(QWidget* parent, const QUrl& url, const QString& title, BookmarkItem* folder)
{
	if (url.isEmpty() || title.isEmpty())
		return false;

	QDialog* dialog{new QDialog(parent)};
	dialog->setWindowIcon(IconProvider::iconForUrl(url));
	dialog->setWindowTitle(Bookmarks::tr("Add New Bookmark"));
	dialog->resize(360, 360);

	QVBoxLayout* layout{new QVBoxLayout(dialog)};

	QLabel* label{new QLabel(Bookmarks::tr("Choose name and location of this bookmark."), dialog)};
	QLineEdit* edit{new QLineEdit(dialog)};

	edit->setText(title);
	edit->setCursorPosition(0);

	BookmarksFoldersButton* folderButton{new BookmarksFoldersButton(dialog, folder)};
	QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);

	layout->addWidget(label);
	layout->addWidget(edit);
	layout->addWidget(folderButton);
	layout->addWidget(box);

	QObject::connect(box, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
	QObject::connect(box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

	dialog->exec();

	if (dialog->result() == QDialog::Rejected || edit->text().isEmpty()) {
		delete dialog;
		return false;
	}

	BookmarkItem* bookmark{new BookmarkItem(BookmarkItem::Url)};
	bookmark->setTitle(edit->text());
	bookmark->setUrl(url);

	Application::instance()->bookmarks()->addBookmark(folderButton->selectedFolder(), bookmark);

	delete dialog;
	return true;
}

bool BookmarksUtils::bookmarkAllTabsDialog(QWidget* parent, TabWidget* tabWidget, BookmarkItem* folder)
{
	Q_ASSERT(tabWidget);

	QDialog* dialog{new QDialog(parent)};
	dialog->setWindowTitle(Bookmarks::tr("Bookmark All Tabs"));
	dialog->resize(360, 360);

	QVBoxLayout* layout{new QVBoxLayout(dialog)};

	QLabel* label = new QLabel(Bookmarks::tr("Choose folder for bookmarks:"), dialog);
	BookmarksFoldersButton* folderButton = new BookmarksFoldersButton(dialog, folder);
	QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);

	layout->addWidget(label);
	layout->addWidget(folderButton);
	layout->addWidget(box);

	QObject::connect(box, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
	QObject::connect(box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

	dialog->exec();

	if (dialog->result() == QDialog::Rejected)
		return false;

	foreach(WebTab* tab, tabWidget->allTabs(false)) {
		if (!tab->url().isEmpty()) {
			BookmarkItem* bookmark{new BookmarkItem(BookmarkItem::Url)};
			bookmark->setTitle(tab->title());
			bookmark->setUrl(tab->url());

			Application::instance()->bookmarks()->addBookmark(folderButton->selectedFolder(), bookmark);
		}
	}

	delete dialog;
	return true;
}

bool BookmarksUtils::editBookmarkDialog(QWidget* parent, BookmarkItem* item)
{
	QDialog* dialog = new QDialog(parent);
	dialog->setWindowIcon(item->icon());
	dialog->setWindowTitle(Bookmarks::tr("Edit Bookmark"));

	QFormLayout* layout = new QFormLayout(dialog);

	QLineEdit* title = new QLineEdit{dialog};
	QLineEdit* address = new QLineEdit{dialog};
	QLineEdit* keyword = new QLineEdit{dialog};
	QPlainTextEdit* description = new QPlainTextEdit{dialog};
	QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);

	title->setText(item->title());
	description->document()->setPlainText(item->description());

	layout->addRow(Bookmarks::tr("Title:"), title);

	if (!item->isFolder()) {
		layout->addRow(Bookmarks::tr("Address:"), address);
		layout->addRow(Bookmarks::tr("Keyword:"), keyword);

		address->setText(item->urlString());
		keyword->setText(item->keyword());
	}

	layout->addRow(Bookmarks::tr("Description:"), description);
	layout->addWidget(box);

	QObject::connect(box, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
	QObject::connect(box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

	dialog->exec();

	if (dialog->result() == QDialog::Rejected) {
		delete dialog;
		return false;
	}

	item->setTitle(title->text());

	if (!item->isFolder()) {
		item->setUrl(QUrl::fromEncoded(address->text().toUtf8()));
		item->setKeyword(keyword->text());
	}

	item->setDescription(description->toPlainText());

	delete dialog;
	return true;
}

void BookmarksUtils::openBookmark(BrowserWindow* window, BookmarkItem* item)
{
	Q_ASSERT(window);

	if (!item)
		return;

	if (item->isFolder())
		openFolderInTabs(window, item);
	else if (item->isUrl()) {
		item->updateVisitCount();
		window->loadUrl(item->url());
	}
}

void BookmarksUtils::openBookmarkInNewTab(BrowserWindow* window, BookmarkItem* item)
{
	Q_ASSERT(window);

	if (!item)
		return;

	if (item->isFolder())
		openFolderInTabs(window, item);
	else if (item->isUrl()) {
		item->updateVisitCount();
		window->loadUrlInNewTab(item->url());
	}
}

void BookmarksUtils::openBookmarkInNewWindow(BookmarkItem* item)
{
	if (!item->isUrl())
		return;

	item->updateVisitCount();

	Application::instance()->createWindow(Application::WT_NewWindow, item->url());
}

void BookmarksUtils::openBookmarkInNewPrivateWindow(BookmarkItem* item)
{
	if (!item->isUrl())
		return;

	item->updateVisitCount();

	Application::instance()->startPrivateBrowsing(item->url());
}

void BookmarksUtils::openFolderInTabs(BrowserWindow* window, BookmarkItem* folder)
{
	Q_ASSERT(window);
	Q_ASSERT(folder->isFolder());

	bool showWarning{folder->children().size() > 10};

	if (!showWarning) {
		foreach (BookmarkItem* child, folder->children()) {
			if (child->isFolder()) {
				showWarning = true;
				break;
			}
		}
	}

	if (showWarning) {
		QMessageBox::StandardButton button = QMessageBox::warning(window, Bookmarks::tr("Confirmation"),
		                                                          Bookmarks::tr(
			                                                          "Are you sure you want to open all bookmarks from '%1' folder in tabs?")
		                                                          .arg(folder->title()),
		                                                          QMessageBox::Yes | QMessageBox::No);

		if (button != QMessageBox::Yes)
			return;
	}

	foreach (BookmarkItem* child, folder->children()) {
		if (child->isUrl())
			openBookmarkInNewTab(window, child);
		else if (child->isFolder())
			openFolderInTabs(window, child);
	}
}
}
