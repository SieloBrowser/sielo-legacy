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
#ifndef SIELOBROWSER_ADDBOOKMARKDIALOG_HPP
#define SIELOBROWSER_ADDBOOKMARKDIALOG_HPP

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

#include <QSpacerItem>

#include <QVBoxLayout>

namespace Sn {
class BookmarksManager;
class AddBookmarkProxyModel;

class AddBookmarkDialog: public QDialog {
Q_OBJECT

public:
	AddBookmarkDialog(const QString& url, const QString& title, QWidget* parent = nullptr,
					  BookmarksManager* bookmarksManager = nullptr);

private slots:
	void accept();

private:
	void setupUI();

	QString m_url{};

	QVBoxLayout* m_layout{nullptr};

	QLabel* m_description{nullptr};
	QLineEdit* m_name{nullptr};
	QComboBox* m_location{nullptr};

	QSpacerItem* m_spacer{nullptr};

	QDialogButtonBox* m_buttonBox{nullptr};

	BookmarksManager* m_bookmarksManager{nullptr};
	AddBookmarkProxyModel* m_proxyModel{nullptr};
};
}

#endif //SIELOBROWSER_ADDBOOKMARKDIALOG_HPP
