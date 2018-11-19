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
#ifndef SIELO_BROWSER_ABOUTDIALOG_HPP
#define SIELO_BROWSER_ABOUTDIALOG_HPP

#include "SharedDefines.hpp"

#include <QDialog>

#include <QVBoxLayout>

#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QDialogButtonBox>

namespace Sn {
class SIELO_SHAREDLIB AboutDialog: public QDialog {
Q_OBJECT

public:
	AboutDialog(QWidget* parent = nullptr);
	~AboutDialog();

private slots:
	void previous();
	void next();

private:
	void setupUI();
	void setupAbout();
	void setupPartners();

	void updateLabels();

	int m_currentIndex{ 0 };

	// UI
	QVBoxLayout* m_layout{ nullptr };
	QHBoxLayout* m_buttonsLayout{ nullptr };

	QLabel* m_image{ nullptr };
	QLabel* m_desc{ nullptr };

	QSpacerItem* m_spacer{ nullptr };
	QPushButton* m_previous{ nullptr };
	QPushButton* m_next{ nullptr };

	QStringList m_images{};
	QStringList m_descs{};
};

}

#endif //SIELO_BROWSER_ABOUTDIALOG_HPP
