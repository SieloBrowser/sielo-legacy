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
#ifndef SIELO_BROWSER_WEBCONFIGPAGE_HPP
#define SIELO_BROWSER_WEBCONFIGPAGE_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QFrame>

#include <QSpacerItem>

namespace Sn {

class SIELO_SHAREDLIB WebConfigPage: public QWidget {
Q_OBJECT

public:
	WebConfigPage(QWidget* parent = nullptr);
	~WebConfigPage();

	void loadSettings();
	void save();

private:
	void setupUI();

	QVBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_wheelLayout{nullptr};
	QHBoxLayout* m_defaultZoomLayout{nullptr};

	QCheckBox* m_allowPeeper{nullptr};
	QCheckBox* m_allowJavaScript{nullptr};
	QCheckBox* m_includeLinksInFocusChain{nullptr};
	QCheckBox* m_enableXSS{nullptr};
	QCheckBox* m_animatedScrolling{nullptr};
	QCheckBox* m_enableSpacialAnimation{nullptr};

	QFrame* m_line1{nullptr};

	QLabel* m_descWheel{nullptr};
	QSpinBox* m_wheelSpin{nullptr};
	QLabel* m_lineLabel{nullptr};

	QFrame* m_line2{nullptr};

	QLabel* m_descDefaultZoom{nullptr};
	QComboBox* m_defaultZoom{nullptr};
	QSpacerItem* m_defaultZoomSpacer{nullptr};

	QSpacerItem* m_layoutSpacer{nullptr};
};

}

#endif //SIELO_BROWSER_WEBCONFIGPAGE_HPP
