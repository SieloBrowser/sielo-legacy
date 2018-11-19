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

#pragma once
#ifndef SIELOBROWSER_BOOKMARKSIMPORTDIALOG_HPP
#define SIELOBROWSER_BOOKMARKSIMPORTDIALOG_HPP

#include "SharedDefines.hpp"

#include <QDialog>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QStackedWidget>
#include <QListWidget>
#include <QTreeView>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>

namespace Sn
{
class BookmarkItem;
class BookmarksModel;
class BookmarksImporter;

class SIELO_SHAREDLIB BookmarksImportDialog: public QDialog {
Q_OBJECT

public:
	BookmarksImportDialog(QWidget* parent = nullptr);
	~BookmarksImportDialog();

private slots:
	void nextPage();
	void previousPage();
	void setFile();

private:
	enum Browser {
		Chrome = 0,
		OldSielo = 1,
		Html = 2
	};

	void showImporterPage();
	void showExportedBookmarks();
	void addExportedBookmarks();

	void setupUI();

	QVBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_buttonsLayout{nullptr};

	QVBoxLayout* m_browserLayout{nullptr};
	QGridLayout* m_fileLayout{nullptr};
	QVBoxLayout* m_finalLayout{nullptr};

	QLabel* m_desc{nullptr};

	// Buttons
	QSpacerItem* m_buttonsSpacer{nullptr};
	QPushButton* m_buttonBack{nullptr};
	QPushButton* m_buttonNext{nullptr};
	QPushButton* m_buttonCancel{nullptr};

	// Pages
	QStackedWidget* m_pages{nullptr};
	QWidget* m_browserPage{nullptr};
	QWidget* m_filePage{nullptr};
	QWidget* m_finalPage{nullptr};

	// Browser Page
	QLabel* m_browserDesc{nullptr};
	QListWidget* m_browserList{nullptr};

	// File page
	QLabel* m_importDesc{nullptr};
	QLabel* m_importIcon{nullptr};
	QLabel* m_importInstructions{nullptr};
	QLabel* m_importStandardPath{nullptr};
	QLineEdit* m_chooseFile{nullptr};
	QPushButton* m_buttonChooseFile{nullptr};

	// Final Page
	QTreeView* m_view{nullptr};

	int m_currentPage{0};

	BookmarksImporter* m_importer{nullptr};
	BookmarkItem* m_importedFolder{nullptr};
	BookmarksModel* m_model{nullptr};
};
}

#endif //SIELOBROWSER_BOOKMARKSIMPORTDIALOG_HPP
