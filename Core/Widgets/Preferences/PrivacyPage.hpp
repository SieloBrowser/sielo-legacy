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
#ifndef SIELO_BROWSER_PRIVACYPAGE_HPP
#define SIELO_BROWSER_PRIVACYPAGE_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QGridLayout>

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSpacerItem>

namespace Sn {
class SIELO_SHAREDLIB PrivacyPage: public QWidget {
Q_OBJECT

public:
	PrivacyPage(QWidget* parent = nullptr);
	~PrivacyPage();

	void loadSettings();
	void save();

private slots:
	void showCookieManager();
	void showJSOptions();
	void showHTML5Permissions();

private:
	void setupUI();

	QGridLayout* m_layout{nullptr};

	QLabel* m_labelCookies{nullptr};
	QLabel* m_descCookies{nullptr};
	QPushButton* m_cookieManager{nullptr};

	QLabel* m_labelJS{nullptr};
	QLabel* m_descJS{nullptr};
	QPushButton* m_JSOptions{nullptr};

	QLabel* m_labelHTML5Permissions{nullptr};
	QLabel* m_descHTML5Permissions{nullptr};
	QPushButton* m_HTML5Permissions{nullptr};

	QLabel* m_labelOther{nullptr};
	QCheckBox* m_doNotTrack{nullptr};

	QSpacerItem* m_cookiesSpacer{nullptr};
	QSpacerItem* m_doNotTrackSpacer{nullptr};
	QSpacerItem* m_spacer{nullptr};
};
}

#endif //SIELO_BROWSER_PRIVACYPAGE_HPP
