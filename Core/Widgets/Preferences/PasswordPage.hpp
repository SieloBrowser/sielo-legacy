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
#ifndef SIELO_BROWSER_PASSWORDPAGE_HPP
#define SIELO_BROWSER_PASSWORDPAGE_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QGridLayout>

#include <QLabel>
#include <QCheckBox>
#include <QFrame>
#include <QSpacerItem>

#include "Password/AutoFill/AutoFillManager.hpp"

namespace Sn {

class SIELO_SHAREDLIB PasswordPage: public QWidget {
Q_OBJECT

public:
	PasswordPage(QWidget* parent);
	~PasswordPage();

	void loadSettings();
	void save();

private slots:
	void allowChanged(bool allow);

private:
	void setupUI();

	QGridLayout* m_layout{nullptr};
	QHBoxLayout* m_autoFillLayout{nullptr};

	QLabel* m_desc{nullptr};
	QCheckBox* m_allowCheck{nullptr};
	QFrame* m_autoFillFrame{nullptr};
	AutoFillManager* m_autoFillManager{nullptr};

	QSpacerItem* m_leftSpacer{nullptr};
	QSpacerItem* m_rightSpacer{nullptr};
};

}

#endif //SIELO_BROWSER_PASSWORDPAGE_HPP
