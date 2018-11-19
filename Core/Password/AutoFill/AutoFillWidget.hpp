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
#ifndef SIELO_BROWSER_AUTOFILLWIDGET_HPP
#define SIELO_BROWSER_AUTOFILLWIDGET_HPP

#include "SharedDefines.hpp"

#include <QFrame>

#include <QVector>

#include <QGridLayout>
#include <QLabel>

#include "Password/PasswordManager.hpp"

namespace Sn {
class WebView;

class SIELO_SHAREDLIB AutoFillWidget: public QFrame {
Q_OBJECT

public:
	AutoFillWidget(WebView* view, QWidget* parent = nullptr);
	~AutoFillWidget();

	void setFormData(const QVector<PasswordEntry>& data);

	void showAt(QWidget* parent);

	Qt::Alignment popupAlignement() const { return m_alignement; }
	void setPopupAlignement(Qt::Alignment alignement);

private slots:
	void loginToPage();

private:
	void setupUI();

	QGridLayout* m_layout{nullptr};
	QGridLayout* m_loginLayout{nullptr};
	QGridLayout* m_buttonLayout{nullptr};

	QLabel* m_loginLabel{nullptr};

	WebView* m_view{nullptr};
	QVector<PasswordEntry> m_data;

	Qt::Alignment m_alignement;

};

}

#endif //SIELO_BROWSER_AUTOFILLWIDGET_HPP
