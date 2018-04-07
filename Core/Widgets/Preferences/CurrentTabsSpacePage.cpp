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

#include "CurrentTabsSpacePage.hpp"


#include "Widgets/Tab/TabWidget.hpp"

namespace Sn {
CurrentTabsSpacePage::CurrentTabsSpacePage(TabWidget* tabWidget, QWidget* parent) :
	QWidget(parent),
	m_tabWidget(tabWidget)
{
	setupUI();
	loadSettings();
}

CurrentTabsSpacePage::~CurrentTabsSpacePage()
{
	// Empty
}

void CurrentTabsSpacePage::loadSettings()
{
	m_lineHomePageUrl->setText(m_tabWidget->homeUrl().toString());
}

void CurrentTabsSpacePage::save()
{
	QString newUrl{m_lineHomePageUrl->text()};
	if ((newUrl.left(7) != "http://") && (newUrl.left(8) != "https://"))
		newUrl.insert(0, "http://");

	m_tabWidget->setHomeUrl(newUrl);
}

void CurrentTabsSpacePage::setupUI()
{
	m_layout = new QVBoxLayout(this);

	m_descHomePageUrl = new QLabel(tr("Home page url for current tabs space"), this);

	m_lineHomePageUrl = new QLineEdit(this);
	m_lineHomePageUrl->setPlaceholderText(tr("Enter url"));

	m_spacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_layout->addWidget(m_descHomePageUrl);
	m_layout->addWidget(m_lineHomePageUrl);
	m_layout->addItem(m_spacer);
}
}