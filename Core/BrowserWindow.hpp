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
#ifndef SIELOBROWSER_BROWSERWINDOW_HPP
#define SIELOBROWSER_BROWSERWINDOW_HPP

#include <QMainWindow>

#include <QVBoxLayout>
#include <QSplitter>

#include <QUrl>

#include "Application.hpp"

namespace Sn {
class WebTab; //TODO: do this class
class TabbedWebView;

class TabWidget;

class BrowserWindow: public QMainWindow {
Q_OBJECT

public:
	BrowserWindow(Application::WindowType type, const QUrl& url = QUrl());
	~BrowserWindow();

	TabbedWebView* webView() const;
	TabbedWebView* webView(int index) const;
public slots:
	void enterHtmlFullScreen();

	void bookmarkAllTabs();

private slots:
	void addTab();

private:
	QUrl m_startUrl{};
	QUrl m_homePage{};
	Application::WindowType m_windowType{};
	WebTab* m_startTab{nullptr};
	WebTab* m_startPage{nullptr};

	QVBoxLayout* m_layout{nullptr};
	QSplitter* m_mainSplitter{nullptr};

	TabWidget* m_tabWidget{nullptr};
};

}

#endif //SIELOBROWSER_BROWSERWINDOW_HPP
