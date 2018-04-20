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
#ifndef SIELOBROWSER_HISTORYDIALOG_HPP
#define SIELOBROWSER_HISTORYDIALOG_HPP

#include <QDialog>
#include <QPoint>

#include <QUrl>

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>

#include "View/TreeView.hpp"

namespace Sn {
class SearchLineEdit;

class HistoryManager;

class HistoryDialog: public QDialog {
Q_OBJECT
public:
	HistoryDialog(QWidget* parent = nullptr, HistoryManager* setHistory = nullptr);

signals:
	void openUrl(const QUrl& url);

private slots:
	void setupUi();

	void customContextMenuRequested(const QPoint& pos);
	void open();
	void copy();

private:
	QGridLayout* m_layout{nullptr};
	QSpacerItem* m_searchSpacer{nullptr};
	SearchLineEdit* m_searchLineEdit{nullptr};
	TreeView* m_tree{nullptr};
	QHBoxLayout* m_buttonLayout{nullptr};
	QPushButton* m_removeButton{nullptr};
	QPushButton* m_removeAllButton{nullptr};
	QSpacerItem* m_buttonSpacer{nullptr};
	QDialogButtonBox* m_buttonBox{nullptr};
};
}
#endif //SIELOBROWSER_HISTORYDIALOG_HPP
