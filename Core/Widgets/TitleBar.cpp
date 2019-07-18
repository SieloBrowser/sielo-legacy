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

#include "TitleBar.hpp"

#include <QDesktopWidget>
#include <QScreen>

#include "Bookmarks/BookmarksUtils.hpp"
#include "Bookmarks/BookmarksToolbar.hpp"

#include "Utils/Settings.hpp"

#include "Widgets/NavigationBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"

#include "Application.hpp"
#include "BrowserWindow.hpp"

namespace Sn {
TitleBar::TitleBar(BrowserWindow* window, bool showBookmarks) :
		QWidget(window),
		m_window(window)
{
	setObjectName("title-bar");

	setupUI();
	loadSettings();

	connect(m_window, &BrowserWindow::maximizeChanged, this, &TitleBar::maximizeChanged);

#ifdef Q_OS_WIN
	m_window->addCaption(m_moveControlWidget);
	m_window->addCaption(m_moveTopControlWidget);

	connect(m_closeButton, &QToolButton::clicked, this, &TitleBar::closeWindow);
	connect(m_toggleMaximize, &QToolButton::clicked, this, &TitleBar::toggleMaximize);
	connect(m_minimize, &QToolButton::clicked, this, &TitleBar::minimize);
#endif
}

void TitleBar::loadSettings()
{
	m_navigationToolBar->loadSettings();
}

void TitleBar::closeWindow()
{
	m_window->close();
}

void TitleBar::toggleMaximize()
{
	if (m_isMaximized) {
		if (m_window->isFullScreen())
			m_window->showNormal();

		m_isMaximized = false;
		m_window->setGeometry(m_geometry);
	}
	else {
		m_window->setGeometry(Application::desktop()->availableGeometry(m_window));
		m_window->showNormal();
	}

//	if (isWindowMaximized() && !forceMaximize) {
//		if (m_window->isFullScreen())
//			m_window->showNormal();
//
//		m_window->setGeometry(m_geometry);
//		m_isMaximized = false;
//
//#ifdef Q_OS_WIN
//		m_toggleMaximize->setObjectName(QLatin1String("titlebar-button-maximize"));
//		m_toggleMaximize->setIcon(Application::getAppIcon("tb-maximize", "titlebar"));
//		m_moveTopControlWidget->show();
//#endif // Q_OS_WIN
//	}
//	else {
//		if (!m_isOnSide)
//			m_geometry = m_window->geometry();
//
//		m_window->setGeometry(Application::desktop()->availableGeometry(m_window));
//		m_isMaximized = true;
//
//		m_window->showNormal();
//
//#ifdef Q_OS_WIN
//		m_toggleMaximize->setObjectName(QLatin1String("titlebar-button-reverse-maximize"));
//		m_toggleMaximize->setIcon(Application::getAppIcon("tb-revert-maximize", "titlebar"));
//		m_moveTopControlWidget->hide();
//#endif // Q_OS_WIN
//	}
}

void TitleBar::minimize()
{
	m_window->showMinimized();
}

void TitleBar::maximizeChanged(bool maximized, QSize oldSize)
{
	if (maximized) {
		if (!m_isMaximized)
			m_geometry = QRect(m_window->geometry().x(), m_window->geometry().y(), oldSize.width(), oldSize.height());

		m_isMaximized = true;

#ifdef Q_OS_WIN
		m_moveTopControlWidget->hide();

		m_toggleMaximize->setObjectName(QLatin1String("titlebar-button-reverse-maximize"));
		m_toggleMaximize->setIcon(Application::getAppIcon("tb-revert-maximize", "titlebar"));
#endif // Q_OS_WIN
	}
	else {
		if (m_isMaximized) {
			m_isMaximized = false;
			m_window->setGeometry(m_geometry);
		}

#ifdef Q_OS_WIN
		m_moveTopControlWidget->show();

		m_toggleMaximize->setObjectName(QLatin1String("titlebar-button-maximize"));
		m_toggleMaximize->setIcon(Application::getAppIcon("tb-maximize", "titlebar"));
#endif // Q_OS_WIN
	}
}

void TitleBar::setupUI()
{
	m_main_layout = new QVBoxLayout(this);
	m_main_layout->setSpacing(0);
	m_main_layout->setContentsMargins(0, 0, 0, 0);

	m_layout = new QHBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_addressBars = new QStackedWidget(this);
	m_navigationToolBar = new NavigationToolBar(m_window, m_addressBars);
	m_navigationToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_layout->addWidget(m_navigationToolBar);

#ifdef Q_OS_WIN
	m_moveTopControlWidget = new QWidget(this);
	m_moveTopControlWidget->setObjectName("titlebar-movecontrol-top");
	m_moveTopControlWidget->setFixedHeight(8);

	m_moveControlWidget = new QWidget(this);
	m_moveControlWidget->setObjectName("titlebar-movecontrol");
	m_moveControlWidget->setFixedWidth(48);

	m_closeButton = new QToolButton(this);
	m_toggleMaximize = new QToolButton(this);
	m_minimize = new QToolButton(this);

	m_closeButton->setObjectName(QLatin1String("titlebar-button-close"));
	m_closeButton->setIcon(Application::getAppIcon("tb-close", "titlebar"));

	m_toggleMaximize->setObjectName(
		QLatin1String(m_isMaximized ? "titlebar-button-reverse-maximize" : "titlebar-button-maximize"));
	m_toggleMaximize->setIcon(m_isMaximized ? Application::getAppIcon("tb-revert-maximize", "titlebar") :
							  Application::getAppIcon("tb-maximize", "titlebar"));

	m_minimize->setObjectName(QLatin1String("titlebar-button-minimize"));
	m_minimize->setIcon(Application::getAppIcon("tb-minimize", "titlebar"));

	m_layout->addWidget(m_moveControlWidget);
	m_layout->addWidget(m_minimize);
	m_layout->addWidget(m_toggleMaximize);
	m_layout->addWidget(m_closeButton);

	m_main_layout->addWidget(m_moveTopControlWidget);
#endif

	m_main_layout->addLayout(m_layout);

	resizeEvent(nullptr);
}
}
