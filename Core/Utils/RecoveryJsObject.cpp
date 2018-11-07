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

#include "Utils/RecoveryJsObject.hpp"

#include <QJsonObject>

#include <QBuffer>

#include "Utils/RestoreManager.hpp"

#include "Web/WebPage.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"

namespace Sn {

RecoveryJsObject::RecoveryJsObject(RestoreManager* manager) :
	QObject(),
	m_manager(manager)
{
	// Empty
}

void RecoveryJsObject::setPage(WebPage* page)
{
	Q_ASSERT(page);

	m_page = page;
}

QJsonArray RecoveryJsObject::restoreData() const
{
	QJsonArray out{};

	/*
		foreach (const RestoreManager::WindowData& window, m_manager->restoreData()) {
			int j{0};
			QJsonArray tabs;
				foreach (const WebTab::SavedTab& tab, window.tabsState) {
					QJsonObject jsnTab;

					jsnTab[QStringLiteral("tab")] = j;

					QPixmap iconPixmap{tab.icon.pixmap(16)};
					QUrl pixmapDataUrl{};
					QByteArray bytes;
					QBuffer buffer{&bytes};

					buffer.open(QIODevice::WriteOnly);

					if (iconPixmap.save(&buffer), "PNG")
						bytes = buffer.buffer().toBase64();
					else
						bytes = QByteArray();

					const QString data{bytes};
					data.isEmpty() ? pixmapDataUrl = QUrl() : pixmapDataUrl =
																  QUrl(QStringLiteral("data:image/png;base64,") + data);

					jsnTab[QStringLiteral("icon")] = pixmapDataUrl.toString();
					jsnTab[QStringLiteral("title")] = tab.title;
					jsnTab[QStringLiteral("url")] = tab.url.toString();
					jsnTab[QStringLiteral("pinned")] = tab.isPinned;
					jsnTab[QStringLiteral("current")] = window.currentTab == j;

					tabs.append(jsnTab);

					++j;
				}

			QJsonObject jsnWindow{};

			jsnWindow[QStringLiteral("window")] = ++i;
			jsnWindow[QStringLiteral("tabs")] = tabs;

			out.append(jsnWindow);
		}
*/
	return out;
}

void RecoveryJsObject::startNewSession()
{
	BrowserWindow* window{getBrowserWindow()};

	if (!window)
		return;

	m_page->load(window->homePageUrl());

	Application::instance()->destroyRestoreManager();
}

void RecoveryJsObject::restoreSession(const QStringList& excludeWindows, const QStringList& excluedTabs)
{
	Q_ASSERT(excludeWindows.size() == excluedTabs.size());

/*	RestoreData data = m_manager->restoreData();

	for (int i{0}; i < excludeWindows.size(); ++i) {
		int win{excludeWindows[i].toInt()};
		int tab{excluedTabs[i].toInt()};

		if (!(win >= 0 && data.count() > win) || !(tab >= 0 && data[win].tabsState.count() > tab))
			continue;

		RestoreManager::WindowData& windowData{data[win]};

		windowData.tabsState.remove(tab);

		if (windowData.currentTab >= tab)
			--windowData.currentTab;

		if (windowData.tabsState.isEmpty()) {
			data.remove(win);
			continue;
		}

		if (windowData.currentTab < 0)
			windowData.currentTab = windowData.tabsState.size() - 1;
	}

	BrowserWindow* window{getBrowserWindow()};

	if (!window)
		return;

	if (!Application::instance()->restoreSession(window, data))
		startNewSession();
		*/
}

BrowserWindow* RecoveryJsObject::getBrowserWindow() const
{
	TabbedWebView* view = qobject_cast<TabbedWebView*>(m_page->view());

	return view ? view->webTab()->tabWidget()->window() : nullptr;
}

}