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

#include "StatusBarMessage.hpp"

#include <QStatusBar>

#include "Web/Tab/TabbedWebView.hpp"
#include "Web/WebView.hpp"

#include "Widgets/TipLabel.hpp"
#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
StatusBarMessage::StatusBarMessage(TabWidget* tabWidget) :
	m_tabWidget(tabWidget),
	m_statusBarText(new TipLabel(tabWidget))
{
	connect(m_tabWidget, SIGNAL(mouseOver), this, SLOT(sMouseOver));
}

void StatusBarMessage::showMessage(const QString& message)
{
	if (m_tabWidget->window()->statusBar()->isVisible()) {
		m_tabWidget->window()->statusBar()->showMessage(message.isRightToLeft() ? message : (QChar(0x202a) + message));
	}
	else if (Application::instance()->activeWindow() == m_tabWidget->window()) {
		WebView* view = m_tabWidget->webTab()->webView();

		m_statusBarText->setText(message);
		m_statusBarText->setMaximumWidth(view->width());
		m_statusBarText->resize(m_statusBarText->sizeHint());

		QPoint position{0, view->height() - m_statusBarText->height()};
		const QRect statusRect{QRect(view->mapToGlobal(QPoint(0, position.y())), m_statusBarText->size())};

		if (statusRect.contains(QCursor::pos()))
			position.setY(position.y() - m_statusBarText->height());

		m_statusBarText->move(view->mapToGlobal(position));
		m_statusBarText->show(view);
	}
}

void StatusBarMessage::clearMessage()
{
	if (m_tabWidget->window()->statusBar()->isVisible())
		m_tabWidget->window()->statusBar()->showMessage(QString());
	else
		m_statusBarText->hideDelayed();
}

void StatusBarMessage::sMouseOver(bool arg)
{
	if (!arg) clearMessage();
}

}