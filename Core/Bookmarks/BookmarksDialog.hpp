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

#pragma once
#ifndef SIELOBROWSER_BOOKMARKSDIALOG_HPP
#define SIELOBROWSER_BOOKMARKSDIALOG_HPP

#include <QWidget>
#include <QDialog>

#include <QGridLayout>
#include <QHBoxLayout>

#include <QLineEdit>
#include <QSpacerItem>
#include <QPushButton>
#include <QDialogButtonBox>

namespace Sn {
class TreeProxyModel;

class TreeView;

class BookmarksManager;
class BookmarkNode;
class BookmarksModel;

class BookmarksDialog: public QDialog {
Q_OBJECT
public:
	BookmarksDialog(QWidget* parent = nullptr, BookmarksManager* manager = nullptr);
	~BookmarksDialog();

signals:
	void openUrl(const QUrl& url);

private slots:
	void customContextMenuRequested(const QPoint& pos);
	void open();
	void newFolder();

private:
	void expandNodes(BookmarkNode* node);
	bool saveExpandedNodes(const QModelIndex& parent);

	void setupUi();

	BookmarksManager* m_bookmarksManager{nullptr};
	BookmarksModel* m_bookmarksModel{nullptr};
	TreeProxyModel* m_proxyModel{nullptr};

	QGridLayout* m_layout{nullptr};
	QSpacerItem* m_searchSpacer{nullptr};
	QLineEdit* m_search{nullptr};
	TreeView* m_tree{nullptr};
	QHBoxLayout* m_buttonLayout{nullptr};
	QPushButton* m_removeButton{nullptr};
	QPushButton* m_addFolderButton{nullptr};
	QSpacerItem* m_buttonSpacer{nullptr};
	QDialogButtonBox* m_buttonBox{nullptr};
};
}
#endif //SIELOBROWSER_BOOKMARKSDIALOG_HPP
