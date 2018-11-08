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

#include "SiteInfoWidget.hpp"

#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"

namespace Sn {

SiteInfoWidget::SiteInfoWidget(TabWidget* tabWidget, QWidget* parent) :
		AddressBarPopup(parent),
		m_tabWidget(tabWidget)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUI();
	setPopupAlignment(Qt::AlignLeft);

	WebView* view = m_tabWidget->webTab()->webView();

	if (view->url().scheme() == QLatin1String("https"))
		m_secure->setText(tr("Your connection to this site is <b>secured</b>."));
	else
		m_secure->setText(tr("Your connection to this site is <b>unsecured</b>."));

	//TODO: Count number of visite (waiting for new database based history system

	connect(m_moreButton, &QPushButton::clicked, m_tabWidget->action(QStringLiteral("ShowSiteInfo")), &QAction::trigger);
}

SiteInfoWidget::~SiteInfoWidget()
{
	// Empty
}

void SiteInfoWidget::setupUI()
{
	m_layout = new QGridLayout(this);

	m_history = new QLabel(this);
	m_secure = new QLabel(this);
	m_moreButton = new QPushButton(tr("More..."), this);
	m_spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_layout->addWidget(m_secure, 0, 0, 1, 2);
	m_layout->addWidget(m_history, 1, 0, 1, 2);
	m_layout->addWidget(m_moreButton, 2, 0, 1, 1);
	m_layout->addItem(m_spacer, 2, 1, 1, 1);
}

}