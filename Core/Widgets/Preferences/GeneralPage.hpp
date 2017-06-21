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
#ifndef SIELOBROWSER_GENERALPAGE_HPP
#define SIELOBROWSER_GENERALPAGE_HPP

#include <QWidget>

#include <QVBoxLayout>

#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>

namespace Sn {

class GeneralPage: public QWidget {
Q_OBJECT

public:
	GeneralPage(QWidget* parent = nullptr);
	~GeneralPage();

	void loadSettings();
	void save();

private slots:
	void homePageActionChanged(bool enabled);
	void newTabActionChanged(bool enabled);
	void startupActionChanged(const QString& currentIndex);

	void saveCurrentSession();

private:
	void setupUI();

	QVBoxLayout* m_layout{nullptr};
	QVBoxLayout* m_layoutGroup1{nullptr};
	QVBoxLayout* m_layoutGroup2{nullptr};

	// Group Box 1
	QGroupBox* m_groupHomePage{nullptr};
	QRadioButton* m_radioHPBlank{nullptr};
	QRadioButton* m_radioHPCustomUrl{nullptr};
	QLineEdit* m_homePageUrl{nullptr};

	// Group Box 2
	QGroupBox* m_groupNewTab{nullptr};
	QRadioButton* m_radioNTOpenBlankPage{nullptr};
	QRadioButton* m_radioNTOpenHomePage{nullptr};
	QRadioButton* m_radioNTOpenCutomUrl{nullptr};
	QLineEdit* m_newTabUrl{nullptr};

	QLabel* m_descActionOnNewSession{nullptr};
	QPushButton* m_btnSaveCurrentSession{nullptr};
	QComboBox* m_comboActionOnNewSession{nullptr};


};

}

#endif //SIELOBROWSER_GENERALPAGE_HPP
