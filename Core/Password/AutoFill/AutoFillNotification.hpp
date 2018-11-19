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
#ifndef SIELO_BROWSER_AUTOFILLNOTIFICATION_HPP
#define SIELO_BROWSER_AUTOFILLNOTIFICATION_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QHBoxLayout>

#include <QLabel>
#include <QPushButton>

#include <QUrl>

#include "Password/PasswordManager.hpp"
#include "Password/AutoFill/AutoFill.hpp"

namespace Sn {

class SIELO_SHAREDLIB AutoFillNotification: public QWidget {
Q_OBJECT

public:
	AutoFillNotification(const QUrl& url, const PageFormData& formData, const PasswordEntry& updateData);
	~AutoFillNotification();

private slots:
	void update();
	void remember();
	void never();

private:
	void setupUI();

	QHBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_buttonLayout{nullptr};

	QLabel* m_icon{nullptr};
	QLabel* m_desc{nullptr};
	QPushButton* m_updateButton{nullptr};
	QPushButton* m_rememberButton{nullptr};
	QPushButton* m_neverButton{nullptr};
	QPushButton* m_notNowButton{nullptr};

	QUrl m_url{};
	PageFormData m_formData;
	PasswordEntry m_updateData;
};

}

#endif //SIELO_BROWSER_AUTOFILLNOTIFICATION_HPP
