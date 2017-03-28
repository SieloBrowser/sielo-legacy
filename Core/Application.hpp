/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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
#ifndef CORE_APPLICATION_HPP
#define CORE_APPLICATION_HPP

#include <QApplication>
#include <QList>

#include <QWebEngineProfile>

namespace Sn {
class PluginProxy;
class HistoryManager;
class BookmarksManager;

class Application: public QApplication {
public:
	enum ObjectName {
		ON_WebView,
		ON_TabBar,
		ON_BrowserWindow
	};

	enum NewTabType {
		NTT_SelectedTab = 1,
		NTT_NotSelectedTab = 2,
		NTT_CleanTab = 4,
		NTT_TabAtEnd = 8,
		NTT_NewEmptyTab = 16,
		/* ------------------------- */
			NTT_SelectedNewEmptyTab = NTT_SelectedTab | NTT_TabAtEnd | NTT_NewEmptyTab,
		NTT_SelectedTabAtEnd = NTT_SelectedTab | NTT_TabAtEnd,
		NTT_NotSelectedTabAtEnd = NTT_NotSelectedTab | NTT_TabAtEnd,
		NTT_CleanSelectedTabAtEnd = NTT_SelectedTab | NTT_TabAtEnd | NTT_CleanTab,
		NTT_CleanSelectedTab = NTT_CleanTab | NTT_SelectedTab,
		NTT_CleanNotSelectedTab = NTT_CleanTab | NTT_NotSelectedTab
	};

	enum Path {
		P_Data = 0,
		P_Plugin = 1
	};

	Application(int& argc, char** argv);
	~Application();

	bool privateBrowsing() const { return m_privateBrowsing; }

	PluginProxy* plugins() { return m_plugins; }
	HistoryManager* historyManager();
	BookmarksManager* bookmarksManager();

	QWebEngineProfile* webProfile() const { return m_webProfile; }

	static QList<QString> paths();
	static Application* instance();

private:
	bool m_privateBrowsing{false};

	PluginProxy* m_plugins{nullptr};
	HistoryManager* m_historyManager{nullptr};
	BookmarksManager* m_bookmarksManager{nullptr};

	QWebEngineProfile* m_webProfile{nullptr};
};

}
#endif // CORE_APPLICATION_HPP
