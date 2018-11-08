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

#include "SiteIcon.hpp"

#include "Web/WebView.hpp"

#include "Widgets/AddressBar/AddressBar.hpp"
#include "Widgets/SiteInfo.hpp"
#include "Widgets/SiteInfoWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn {
SiteIcon::SiteIcon(TabWidget* tabWidget, Sn::AddressBar* parent) :
		ToolButton(parent),
		m_tabWidget(tabWidget),
		m_addressBar(parent)
{
	setObjectName(QLatin1String("addressbar-siteicon"));
	setToolButtonStyle(Qt::ToolButtonIconOnly);
	setCursor(Qt::ArrowCursor);
	setToolTip(AddressBar::tr("Show information about this page"));
	setFocusPolicy(Qt::ClickFocus);
}

SiteIcon::~SiteIcon()
{
	// Empty
}

void SiteIcon::setWebView(Sn::WebView* view)
{
	m_view = view;
}

void SiteIcon::updateIcon(bool secure)
{
	if (secure)
		ToolButton::setIcon(Application::getAppIcon("locked"));
	else
		ToolButton::setIcon(Application::getAppIcon("nonlocked"));
}

void SiteIcon::popupClosed()
{
	setDown(false);
}

void SiteIcon::contextMenuEvent(QContextMenuEvent* event)
{
	// It's just to prevent the propagation to the address bar
	event->accept();
}

void SiteIcon::mouseReleaseEvent(QMouseEvent* event)
{
	bool activated = false;

	if (event->button() == Qt::LeftButton && rect().contains(event->pos()))
		activated = showPopup();

	if (activated)
		setUpdatesEnabled(false);

	ToolButton::mouseReleaseEvent(event);

	if (activated) {
		setDown(true);
		setUpdatesEnabled(true);
	}
}

bool SiteIcon::showPopup()
{
	if (!m_view || !m_tabWidget)
		return false;

	QUrl url{m_view->url()};

	if (!SiteInfo::canShowSiteInfo(url))
		return false;

	setDown(true);

	SiteInfoWidget* info{new SiteInfoWidget(m_tabWidget)};
	info->showAt(parentWidget());

	connect(info, &SiteInfoWidget::destroyed, this, &SiteIcon::popupClosed);

	return true;
}

}