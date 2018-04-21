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

#include "Widgets/Tab/AddTabButton.hpp"

#include <QMimeData>

#include "Widgets/Tab/MainTabBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"

namespace Sn {

AddTabButton::AddTabButton(TabWidget* tabWidget, MainTabBar* tabBar) :
	ToolButton(tabBar),
	m_tabBar(tabBar),
	m_tabWidget(tabWidget)
{
	setObjectName(QLatin1String("tabwidget-button-addtab"));
	setAutoRaise(true);
	setFocusPolicy(Qt::NoFocus);
	setAcceptDrops(true);
	setToolTip(TabWidget::tr("New Tab"));
}

void AddTabButton::wheelEvent(QWheelEvent* event)
{
	m_tabBar->wheelEvent(event);
}

void AddTabButton::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MiddleButton && rect().contains(event->pos()))
		m_tabWidget->addTabFromClipboard();

	ToolButton::mouseReleaseEvent(event);
}

void AddTabButton::dragEnterEvent(QDragEnterEvent* event)
{
	const QMimeData* mime{event->mimeData()};

	if (mime->hasUrls()) {
		event->acceptProposedAction();
		return;
	}

	ToolButton::dragEnterEvent(event);
}

void AddTabButton::dropEvent(QDropEvent* event)
{
	const QMimeData* mime{event->mimeData()};

	if (!mime->hasUrls()) {
		ToolButton::dropEvent(event);
		return;
	}

		foreach (const QUrl& url, mime->urls()) m_tabWidget->addView(url, Application::NTT_SelectedNewEmptyTab);
}

}