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

#include "BookmarksImportDialog.hpp"

#include <QMessageBox>

#include <QHeaderView>

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarkItem.hpp"
#include "Bookmarks/BookmarksModel.hpp"
#include "Bookmarks/BookmarksItemDelegate.hpp"
#include "Bookmarks/BookmarksImport/BookmarksImporter.hpp"
#include "Bookmarks/BookmarksImport/ChromeImporter.hpp"
#include "Bookmarks/BookmarksImport/OldSieloImporter.hpp"
#include "Bookmarks/BookmarksImport/HtmlImporter.hpp"

#include "Application.hpp"

namespace Sn {
BookmarksImportDialog::BookmarksImportDialog(QWidget* parent) :
	QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName(QLatin1String("bookmarks-dialog-import"));
	setWindowTitle(tr("Import Bookmarks"));
	setupUI();

	connect(m_buttonNext, &QPushButton::clicked, this, &BookmarksImportDialog::nextPage);
	connect(m_buttonBack, &QPushButton::clicked, this, &BookmarksImportDialog::previousPage);
	connect(m_buttonChooseFile, &QPushButton::clicked, this, &BookmarksImportDialog::setFile);
	connect(m_buttonCancel, &QPushButton::clicked, this, &BookmarksImportDialog::close);
}
BookmarksImportDialog::~BookmarksImportDialog()
{
	m_view->setModel(nullptr);

	delete m_model;
	delete m_importedFolder;
	delete m_importer;
}
void BookmarksImportDialog::nextPage()
{
	switch (m_currentPage) {
	case 0:
		if (!m_browserList->currentItem()) 
			return;

		switch (m_browserList->currentRow()) {
		case Chrome:
			m_importer = new ChromeImporter();
			break;
		case OldSielo:
			m_importer = new OldSieloImporter();
			break;
		case Html:
			m_importer = new HtmlImporter();
			break;
		default:
			Q_ASSERT(!"Unreachable");
			break;
		}

		m_chooseFile->clear();

		showImporterPage();

		m_buttonNext->setEnabled(false);
		m_buttonBack->setEnabled(true);
		m_pages->setCurrentIndex(++m_currentPage);
		break;

	case 1:
		if (m_chooseFile->text().isEmpty())
			return;

		if (m_importer->prepareImport()) 
			m_importedFolder = m_importer->importBookmarks();

		if (m_importer->error()) {
			QMessageBox::critical(this, tr("Error!"), m_importer->errorString());
			return;
		}

		if (!m_importedFolder || m_importedFolder->children().isEmpty()) {
			QMessageBox::warning(this, tr("Error!"), tr("No bookmarks were found."));
			return;
		}

		Q_ASSERT(m_importedFolder->isFolder());

		m_buttonNext->setText(tr("Finish"));
		m_pages->setCurrentIndex(++m_currentPage);
		
		showExportedBookmarks();
		
		break;

	case 2:
		addExportedBookmarks();
		close();
		break;

	default:
		Q_ASSERT(!"Unreachable");
	}
}
void BookmarksImportDialog::previousPage()
{
	switch (m_currentPage) {
	case 0:
		break;

	case 1:
		m_buttonNext->setEnabled(true);
		m_buttonBack->setEnabled(false);
		m_pages->setCurrentIndex(--m_currentPage);

		delete m_importer;
		m_importer = nullptr;
		break;

	case 2:
		showImporterPage();

		m_buttonNext->setText(tr("Next"));
		m_buttonNext->setEnabled(true);
		m_buttonBack->setEnabled(true);
		m_pages->setCurrentIndex(--m_currentPage);

		m_view->setModel(nullptr);
		delete m_model;
		m_model = nullptr;

		delete m_importedFolder;
		m_importedFolder = nullptr;
		break;

	default:
		Q_ASSERT(!"Unreachable");
	}
}
void BookmarksImportDialog::setFile()
{
	Q_ASSERT(m_importer);

	m_chooseFile->setText(m_importer->getPath(this));
	m_buttonNext->setEnabled(!m_chooseFile->text().isEmpty());
}
void BookmarksImportDialog::showImporterPage()
{
	m_importDesc->setText(tr("<b>Importing from %1</b>").arg(m_browserList->currentItem()->text()));
	m_importIcon->setPixmap(m_browserList->currentItem()->icon().pixmap(48));
	
	m_importInstructions->setText(m_importer->description());
	m_importStandardPath->setText(QString("<i>%1</i>").arg(m_importer->standardPath()));
}
void BookmarksImportDialog::showExportedBookmarks()
{
	m_model = new BookmarksModel(m_importedFolder, nullptr, this);
	
	m_view->setModel(m_model);
	m_view->header()->resizeSection(0, m_view->header()->width() / 2);
	m_view->expandAll();
}
void BookmarksImportDialog::addExportedBookmarks()
{
	Application::instance()->bookmarks()->addBookmark(Application::instance()->bookmarks()->unsortedFolder(), m_importedFolder);
	m_importedFolder = nullptr;
}

void BookmarksImportDialog::setupUI()
{
	resize(570, 350);

	// Pages
	m_pages = new QStackedWidget(this);
	m_browserPage = new QWidget(this);
	m_filePage = new QWidget(this);
	m_finalPage = new QWidget(this);

	// Layouts
	m_layout = new QVBoxLayout(this);
	m_buttonsLayout = new QHBoxLayout();

	m_browserLayout = new QVBoxLayout(m_browserPage);
	m_fileLayout = new QGridLayout(m_filePage);
	m_finalLayout = new QVBoxLayout(m_finalPage);

	m_desc = new QLabel(tr("<b>Import Bookmarks</b>"), this);

	m_pages->addWidget(m_browserPage);
	m_pages->addWidget(m_filePage);
	m_pages->addWidget(m_finalPage);

	// Browser Page
	m_browserDesc = new QLabel(tr("Choose from which you want to import bookmarks"), this);
	m_browserList = new QListWidget(this);

	m_browserList->addItem(new QListWidgetItem(Application::getAppIcon("chrome"), "Google Chrome"));
	m_browserList->addItem(new QListWidgetItem(Application::getAppIcon("old-sielo"), "Sielo Open Beta"));
	m_browserList->addItem(new QListWidgetItem(Application::getAppIcon("html"), tr("Html file")));

	m_browserList->setCurrentRow(0);

	// File Page
	m_importDesc = new QLabel(this);
	m_importIcon = new QLabel(this);
	m_importInstructions = new QLabel(this);
	m_importStandardPath = new QLabel(this);

	m_chooseFile = new QLineEdit(this);
	m_buttonChooseFile = new QPushButton(tr("Choose..."), this);

	// Final page
	m_view = new QTreeView(this);
	m_view->setItemDelegate(new BookmarksItemDelegate(m_view));

	// Buttons
	m_buttonsSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_buttonBack = new QPushButton(tr("Back"), this);
	m_buttonNext = new QPushButton(tr("Next"), this);
	m_buttonCancel = new QPushButton(tr("Cancel"), this);

	// Add Widget to Layouts
	m_browserLayout->addWidget(m_browserDesc);
	m_browserLayout->addWidget(m_browserList);

	m_fileLayout->addWidget(m_importDesc, 0, 0, 1, 1);
	m_fileLayout->addWidget(m_importIcon, 0, 1, 1, 1);
	m_fileLayout->addWidget(m_importInstructions, 1, 0, 1, 2);
	m_fileLayout->addWidget(m_importStandardPath, 2, 0, 1, 2);
	m_fileLayout->addWidget(m_chooseFile, 3, 0, 1, 1);
	m_fileLayout->addWidget(m_buttonChooseFile, 3, 1, 1, 1);

	m_finalLayout->addWidget(m_view);

	m_buttonsLayout->addItem(m_buttonsSpacer);
	m_buttonsLayout->addWidget(m_buttonBack);
	m_buttonsLayout->addWidget(m_buttonNext);
	m_buttonsLayout->addWidget(m_buttonCancel);

	m_layout->addWidget(m_desc);
	m_layout->addWidget(m_pages);
	m_layout->addLayout(m_buttonsLayout);
}
}
