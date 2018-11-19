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
#ifndef SIELOBROWSER_SITEINFOWIDGET_HPP
#define SIELOBROWSER_SITEINFOWIDGET_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QGridLayout>

#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

#include "Widgets/AddressBarPopup.hpp"

namespace Sn {
class TabWidget;

class SIELO_SHAREDLIB SiteInfoWidget : public AddressBarPopup {
Q_OBJECT

public:
	SiteInfoWidget(TabWidget* tabWidget, QWidget* parent = nullptr);
	~SiteInfoWidget();

private:
	void setupUI();

	TabWidget* m_tabWidget{nullptr};

	// UI
	QGridLayout* m_layout{nullptr};
	QLabel* m_history{nullptr};
	QLabel* m_secure{nullptr};
	QPushButton* m_moreButton{nullptr};
	QSpacerItem* m_spacer{nullptr};

};
}


#endif //SIELOBROWSER_SITEINFOWIDGET_HPP
