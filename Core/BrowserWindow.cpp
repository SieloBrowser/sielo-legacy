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

#include "BrowserWindow.hpp"

#include <QToolTip>

#include <QSettings>

#include <QAction>

#include <QTimer>

#include "Web/LoadRequest.hpp"
#include "Web/WebPage.hpp"
#include "Web/Tab/WebTab.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/AddressBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"

namespace Sn {

BrowserWindow::BrowserWindow(Application::WindowType type, const QUrl& url) :
	QMainWindow(nullptr),
	m_startUrl(url),
	m_windowType(type)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_DontCreateNativeAncestors);

	setObjectName(QLatin1String("mainwindow"));
	setWindowTitle(tr("Sielo"));
	setProperty("private", Application::instance()->privateBrowsing());

	setupUi();
	loadSettings();

	QTimer::singleShot(0, this, &BrowserWindow::postLaunch);

}

BrowserWindow::~BrowserWindow()
{
	//TODO: emit window deleted to plugins
}

void BrowserWindow::loadSettings()
{
	QSettings settings{};

	m_homePage = settings.value(QLatin1String("Web-Settings/homePage"), QUrl("https://www.ecosia.org")).toUrl();

	settings.endGroup();

	m_tabWidget->loadSettings();

}

void BrowserWindow::setStartTab(WebTab* tab)
{
	m_startTab = tab;
}

void BrowserWindow::setStartPage(WebPage* page)
{
	m_startPage = page;
}

void BrowserWindow::restoreWindowState(const RestoreManager::WindowData& data)
{
	restoreState(data.windowState);
	m_tabWidget->restoreState(data.tabsState, data.currentTab);
}

void BrowserWindow::currentTabChanged()
{
	TabbedWebView* view{webView()};
	if (!view)
		return;

	setWindowTitle(tr("%1 - Sielo").arg(view->webTab()->title()));

	view->setFocus();
}


TabbedWebView* BrowserWindow::webView() const
{
	return webView(m_tabWidget->currentIndex());
}

TabbedWebView* BrowserWindow::webView(int index) const
{
	WebTab* webTab = qobject_cast<WebTab*>(m_tabWidget->widget(index));
	if (!webTab)
		return 0;

	return webTab->webView();
}

void BrowserWindow::enterHtmlFullScreen()
{
	// Empty
}

void BrowserWindow::bookmarkAllTabs()
{
	// Empty
}

void BrowserWindow::addTab()
{
	m_tabWidget->addView(QUrl(), Application::NTT_SelectedNewEmptyTab, true);
	m_tabWidget->setCurrentTabFresh(true);
}

void BrowserWindow::setupUi()
{
	QWidget* widget{new QWidget(this)};
	widget->setCursor(Qt::ArrowCursor);

	m_layout = new QVBoxLayout(widget);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_tabWidget = new TabWidget(this, this);

	m_mainSplitter = new QSplitter(this);
	m_mainSplitter->addWidget(m_tabWidget);

	m_layout->addWidget(m_tabWidget->tabBar());
	m_layout->addWidget(m_mainSplitter);

	m_tabWidget->tabBar()->show();

	QPalette palette{QToolTip::palette()};
	QColor color{palette.window().color()};

	color.setAlpha(0);
	palette.setColor(QPalette::Window, color);

	QToolTip::setPalette(palette);

	setMinimumWidth(300);
	//TODO: delete this line when settings will be implements
	resize(1200, 720);
	setCentralWidget(widget);

}

void BrowserWindow::postLaunch()
{
	bool addTab{true};
	QUrl startUrl{};

	switch (Application::instance()->afterLaunch()) {
	case Application::OpenBlankPage:
		startUrl = QUrl();
		break;
	case Application::OpenHomePage:
	case Application::RestoreSession:
		startUrl = m_homePage;
		break;

	default:
		break;
	}

	switch (m_windowType) {
	case Application::WT_FirstAppWindow:
		if (Application::instance()->afterLaunch() == Application::RestoreSession
			&& Application::instance()->restoreManager()) {
			addTab = !Application::instance()
				->restoreSession(this, Application::instance()->restoreManager()->restoreData());
		}
		break;
	case Application::WT_NewWindow:
		addTab = true;
		break;
	case Application::WT_OtherRestoredWindow:
		addTab = false;
		break;

	}

	show();

	if (!m_startUrl.isEmpty()) {
		startUrl = m_startUrl;
		addTab = true;
	}
	if (m_startTab) {
		addTab = false;
		m_tabWidget->addView(m_startTab);
	}
	if (m_startPage) {
		addTab = false;
		m_tabWidget->addView(QUrl());
		webView()->setPage(m_startPage);
	}

	if (addTab) {
		m_tabWidget->addView(startUrl, Application::NTT_CleanSelectedTabAtEnd);

		if (startUrl.isEmpty())
			webView()->webTab()->addressBar()->setFocus();
	}

	if (m_tabWidget->tabBar()->normalTabsCount() <= 0)
		m_tabWidget->addView(m_homePage, Application::NTT_SelectedTabAtEnd);

	//TODO: emit main window created to plugins

	QAction* action{new QAction(tr("Restore Closed Tab"), this)};
	action->setShortcut(QKeySequence("Ctrl+Shift+T"));
	this->addAction(action);
	connect(action, SIGNAL(triggered()), m_tabWidget, SLOT(restoreClosedTab()));

	tabWidget()->tabBar()->ensureVisible();
}


}