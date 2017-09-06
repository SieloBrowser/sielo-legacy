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
#ifndef SIELO_BROWSER_ABOUTDIALOG_HPP
#define SIELO_BROWSER_ABOUTDIALOG_HPP

#include <QDialog>

#include <QVBoxLayout>

#include <QLabel>
#include <QTextBrowser>
#include <QDialogButtonBox>

namespace Sn {
class AboutDialog: public QDialog {
Q_OBJECT

public:
	AboutDialog(QWidget* parent = nullptr);
	~AboutDialog();

private:
	void setupUI();
	void showAbout();

	QVBoxLayout* m_layout{nullptr};

	QLabel* m_icon{nullptr};
	QTextBrowser* m_content{nullptr};
	QDialogButtonBox* m_closeButtonBox{nullptr};

	QString m_html{};
};

}

#endif //SIELO_BROWSER_ABOUTDIALOG_HPP
