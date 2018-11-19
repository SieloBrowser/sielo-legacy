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
#ifndef SIELOBROWSER_SITEICON_HPP
#define SIELOBROWSER_SITEICON_HPP

#include "SharedDefines.hpp"

#include <QContextMenuEvent>
#include <QMouseEvent>

#include <QTimer>

#include <QIcon>

#include "Utils/ToolButton.hpp"

namespace Sn {

class TabWidget;

class WebView;

class AddressBar;

class SIELO_SHAREDLIB SiteIcon : public ToolButton {
Q_OBJECT

public:
	SiteIcon(TabWidget* tabWidget, AddressBar* parent);
	~SiteIcon();

	void setWebView(WebView* view);
	void updateIcon(bool secure);

private slots:
	void popupClosed();

private:
	void contextMenuEvent(QContextMenuEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	bool showPopup();

	TabWidget* m_tabWidget{nullptr};
	AddressBar* m_addressBar{nullptr};
	WebView* m_view{nullptr};
};
}


#endif //SIELOBROWSER_SITEICON_HPP
