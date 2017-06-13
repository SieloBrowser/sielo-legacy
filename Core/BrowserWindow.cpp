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

		foreach (TabWidget* tabWidget, m_tabWidgets) tabWidget->loadSettings();

}

QByteArray BrowserWindow::saveTabs()
{
	QByteArray data{};
	QDataStream stream{&data, QIODevice::WriteOnly};

	int count = m_mainSplitter->count();
	stream << m_mainSplitter->count();

	for (int i{0}; i < m_mainSplitter->count(); ++i) {
		QSplitter* verticalSplitter = static_cast<QSplitter*>(m_mainSplitter->widget(i));

		stream << verticalSplitter->count();

		for (int j{0}; j < verticalSplitter->count(); ++j)
			stream << static_cast<TabWidget*>(verticalSplitter->widget(j)->findChild<TabWidget*>(QString("tabwidget")))
				->saveState();

	}

	return data;
}

void BrowserWindow::restoreTabs()
{

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
	int mainSplitterCount{data.spaceTabsCount[0]};
	int tabWidgetToRestore{0};

	for (int i{0}; i < mainSplitterCount; ++i) {
		int verticalSplitterCount{data.spaceTabsCount[i + 1]};
		QSplitter* verticalSplitter{new QSplitter(Qt::Vertical, this)};

			for (int j{0}; j < verticalSplitterCount; ++j) {
				if (i == 0 && j == 0) {
					m_tabWidgets[0]->restoreState(data.tabsState[0], data.currentTabs[0]);
					verticalSplitter->addWidget(m_tabWidgets[0]->parentWidget());
					m_mainSplitter->widget(0)->deleteLater();
					++tabWidgetToRestore;
				}
				else {
					QWidget* widget{new QWidget(this)};
					QVBoxLayout* layout{new QVBoxLayout(widget)};
					TabWidget* tabWidget{new TabWidget(this, widget)};

					layout->setSpacing(0);
					layout->setContentsMargins(0, 0, 0, 0);

					m_tabWidgets.append(tabWidget);
					m_currentTabWidget = tabWidgetToRestore;

					tabWidget->restoreState(data.tabsState[tabWidgetToRestore], data.currentTabs[tabWidgetToRestore]);
					tabWidget->tabBar()->show();

					layout->addWidget(tabWidget->tabBar());
					layout->addWidget(tabWidget);

					connect(tabWidget, &TabWidget::focusIn, this, &BrowserWindow::tabWidgetIndexChanged);

					verticalSplitter->addWidget(widget);

					++tabWidgetToRestore;
				}


		}
		m_mainSplitter->addWidget(verticalSplitter);
	}

	autoResizeTabsSpace();
}

void BrowserWindow::currentTabChanged()
{
	TabbedWebView* view{webView()};
	if (!view)
		return;

	setWindowTitle(tr("%1 - Sielo").arg(view->webTab()->title()));

	view->setFocus();
}

void BrowserWindow::createNewTabsSpace(TabsSpacePosition position, WebTab* tab)
{
	QWidget* widgetTabWidget{createWidgetTabWidget(tab)};

	if (position == BrowserWindow::TSP_Left || position == BrowserWindow::TSP_Right) {
		QSplitter* verticalSplitter{new QSplitter(Qt::Vertical, this)};
		if (position == BrowserWindow::TSP_Left) {
			verticalSplitter->addWidget(widgetTabWidget);

			m_mainSplitter->insertWidget(m_mainSplitter
											 ->indexOf(static_cast<QSplitter*>(m_tabWidgets[m_currentTabWidget]
												 ->parent()->parent())), verticalSplitter);
		}
		else {
			verticalSplitter->addWidget(widgetTabWidget);

			m_mainSplitter->insertWidget(
				m_mainSplitter->indexOf(static_cast<QSplitter*>(m_tabWidgets[m_currentTabWidget]->parent()->parent()))
				+ 1, verticalSplitter);
		}

		QList<int> size;

		for (int i{0}; i < m_mainSplitter->count(); ++i)
			size.append(m_mainSplitter->width() / m_mainSplitter->count());

		m_mainSplitter->setSizes(size);
	}
	else {
		QSplitter* verticalSplitter = static_cast<QSplitter*>(tabWidget()->parent()->parent());
		if (position == BrowserWindow::TSP_Top)
			verticalSplitter->insertWidget(verticalSplitter->indexOf(static_cast<QWidget*>(tabWidget()->parent())),
										   widgetTabWidget);
		else if (position == BrowserWindow::TSP_Bottom)
			verticalSplitter->insertWidget(verticalSplitter->indexOf(static_cast<QWidget*>(tabWidget()->parent())) + 1,
										   widgetTabWidget);

		QList<int> size;

		for (int i{0}; i < verticalSplitter->count(); ++i)
			size.append(verticalSplitter->height() / verticalSplitter->count());

		verticalSplitter->setSizes(size);
	}

}

void BrowserWindow::closeTabsSpace(TabWidget* tabWidget)
{
	QWidget* widgetTabWidget = static_cast<QWidget*>(tabWidget->parent());
	QSplitter* verticalSplitter = static_cast<QSplitter*>(tabWidget->parent()->parent());

	m_tabWidgets.removeOne(tabWidget);

	if (verticalSplitter->count() <= 1)
		verticalSplitter->deleteLater();
	else
		widgetTabWidget->deleteLater();

	m_currentTabWidget = 0;
}

void BrowserWindow::autoResizeTabsSpace()
{
	QList<int> mainSizes;
	QList<int> verticalSizes;
	for (int i{0}; i < m_mainSplitter->count(); ++i)
		mainSizes.append(m_mainSplitter->width() / m_mainSplitter->count());

	m_mainSplitter->setSizes(mainSizes);

	for (int i{0}; i < m_mainSplitter->count(); ++i) {
		mainSizes.clear();

		QSplitter* verticalSplitter = static_cast<QSplitter*>(m_mainSplitter->widget(i));

		for (int j{0}; j < verticalSplitter->count(); ++j)
			verticalSizes.append(verticalSplitter->height() / verticalSplitter->count());

		verticalSplitter->setSizes(verticalSizes);
	}
}

TabbedWebView* BrowserWindow::webView() const
{
	return webView(tabWidget()->currentIndex());
}

TabbedWebView* BrowserWindow::webView(int index) const
{
	WebTab* webTab = qobject_cast<WebTab*>(tabWidget()->widget(index));
	if (!webTab)
		return 0;

	return webTab->webView();
}

TabWidget* BrowserWindow::tabWidget() const
{
	return m_tabWidgets[m_currentTabWidget];
}

TabWidget* BrowserWindow::tabWidget(int index) const
{
	return m_tabWidgets[index];
}

int BrowserWindow::tabWidgetsCount() const
{
	return m_tabWidgets.count();
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
	tabWidget()->addView(QUrl(), Application::NTT_SelectedNewEmptyTab, true);
	tabWidget()->setCurrentTabFresh(true);
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
		tabWidget()->addView(m_startTab);
	}
	if (m_startPage) {
		addTab = false;
		tabWidget()->addView(QUrl());
		webView()->setPage(m_startPage);
	}

	if (addTab) {
		tabWidget()->addView(startUrl, Application::NTT_CleanSelectedTabAtEnd);

		if (startUrl.isEmpty())
			webView()->webTab()->addressBar()->setFocus();
	}

	if (tabWidget()->tabBar()->normalTabsCount() <= 0)
		tabWidget()->addView(m_homePage, Application::NTT_SelectedTabAtEnd);

	//TODO: emit main window created to plugins

	m_restoreAction = new QAction(tr("Restore Closed Tab"), this);
	m_restoreAction->setShortcut(QKeySequence("Ctrl+Shift+T"));
	this->addAction(m_restoreAction);
	connect(m_restoreAction, SIGNAL(triggered()), tabWidget(), SLOT(restoreClosedTab()));

	tabWidget()->tabBar()->ensureVisible();
}

void BrowserWindow::tabWidgetIndexChanged(TabWidget* tabWidget)
{
	if (m_currentTabWidget == m_tabWidgets.indexOf(tabWidget))
		return;

	disconnect(m_restoreAction, SIGNAL(triggered()), m_tabWidgets[m_currentTabWidget], SLOT(restoreClosedTab()));

	m_currentTabWidget = m_tabWidgets.indexOf(tabWidget);

	connect(m_restoreAction, SIGNAL(triggered()), m_tabWidgets[m_currentTabWidget], SLOT(restoreClosedTab()));

}

void BrowserWindow::setupUi()
{
	QWidget* widget{new QWidget(this)};

	m_layout = new QVBoxLayout(widget);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_mainSplitter = new QSplitter(this);

	QWidget* widgetTabWidget{createWidgetTabWidget()};
	QSplitter* verticalSplitter{new QSplitter(Qt::Vertical, this)};

	widget->setCursor(Qt::ArrowCursor);

	verticalSplitter->addWidget(widgetTabWidget);

	m_mainSplitter->addWidget(verticalSplitter);

	m_layout->addWidget(m_mainSplitter);

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

QWidget* BrowserWindow::createWidgetTabWidget(WebTab* tab)
{
	QWidget* widget{new QWidget(this)};
	QVBoxLayout* layout{new QVBoxLayout(widget)};
	TabWidget* tabWidget{new TabWidget(this, widget)};

	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

	m_tabWidgets.append(tabWidget);

	if (tab) {
		int previousCurrentTabWidget{m_currentTabWidget};
		m_currentTabWidget = m_tabWidgets.count() - 1;

		tab->detach();
		tabWidget->addView(tab);

		m_currentTabWidget = previousCurrentTabWidget;
	}

	tabWidget->tabBar()->show();

	layout->addWidget(tabWidget->tabBar());
	layout->addWidget(tabWidget);

	connect(tabWidget, &TabWidget::focusIn, this, &BrowserWindow::tabWidgetIndexChanged);

	return widget;

}

}