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
#ifndef SIELOBROWSER_PREFERENCESDIALOG_HPP
#define SIELOBROWSER_PREFERENCESDIALOG_HPP

#include <QDialog>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDialogButtonBox>
#include <QTabWidget>

namespace Sn {
class DownloadPage;
class AdBlockPage;

class PreferencesDialog: public QDialog {
Q_OBJECT

public:
	PreferencesDialog(QWidget* parent = nullptr);
	~PreferencesDialog();

private slots:
	void saveSettings();

	void buttonClicked(QAbstractButton* button);

private:
	void setupUI();

	QVBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_layoutButton{nullptr};

	QTabWidget* m_pages{nullptr};
	QSpacerItem* m_buttonSpacer{nullptr};
	QDialogButtonBox* m_buttonBox{nullptr};

	DownloadPage* m_pageDownload{nullptr};
	AdBlockPage* m_pageAdBlock{nullptr};
};

}

#endif //SIELOBROWSER_PREFERENCESDIALOG_HPP
