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

#include "TabsSpaceSplitter.hpp"

#include <QFileInfo>

#include "Utils/Settings.hpp"
#include "Utils/DataPaths.hpp"

#include "Web/WebView.hpp"

#include "Widgets/TitleBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"
#include "Widgets/Preferences/Appearance.hpp"

#include "MaquetteGrid/MaquetteGridItem.hpp"
#include "MaquetteGrid/MaquetteGridManager.hpp"
#include "MaquetteGrid/MaquetteGridTabsList.hpp"

#include "Application.hpp"
#include "BrowserWindow.hpp"

namespace Sn
{
TabsSpaceSplitter::SavedTabsSpace::SavedTabsSpace()
{
	// Empty
}

TabsSpaceSplitter::SavedTabsSpace::SavedTabsSpace(TabsSpaceSplitter* splitter, TabWidget* tabWidget)
{
	QWidget* columnWidget{nullptr};
	
	TabsSpaceInfo info = splitter->tabsSpaceInfo(tabWidget);

	x = info.x;
	y = info.y;
	columnWidget = info.column;
	homeUrl = tabWidget->homeUrl().toString();

	tabs.reserve(tabWidget->count());

	for (int i{0}; i < tabWidget->count(); ++i) {
		WebTab* webTab{tabWidget->webTab(i)};

		if (!webTab)
			continue;

		WebTab::SavedTab tab{webTab};

		if (webTab->application()) {
			tab.title = "Home Page";
			tab.url = tabWidget->window()->homePageUrl();
		}

		if (!tab.isValide())
			continue;

		if (webTab->isCurrentTab())
			currentTab = i;

		tabs.append(tab);
	}
}

TabsSpaceSplitter::SavedTabsSpace::SavedTabsSpace(MaquetteGridTabsList* maquetteGridTabsList)
{
	Settings settings{};
	int defaultZoomLevel{settings.value("Web-Settings/defaultZoomLevel", WebView::zoomLevels().indexOf(100)).toInt()};

	homeUrl = maquetteGridTabsList->manager()->window()->homePageUrl().toString();
	currentTab = 0;

	tabs.reserve(maquetteGridTabsList->count());

	for (int i{0}; i < maquetteGridTabsList->count(); ++i) {
		WebTab::SavedTab tab{};
		QListWidgetItem* itm{maquetteGridTabsList->item(i)};

		tab.icon = itm->icon();
		tab.title = itm->data(MaquetteGridManager::TitleRole).toString();
		tab.url = itm->data(MaquetteGridManager::UrlRole).toUrl();
		tab.history = QByteArray();
		tab.isPinned = false;
		tab.zoomLevel = defaultZoomLevel;

		tabs.append(tab);
	}
}

bool TabsSpaceSplitter::SavedTabsSpace::isValid() const
{
	for (const WebTab::SavedTab& tab : tabs) {
		if (!tab.isValide())
			return false;
	}

	return currentTab > -1;
}

void TabsSpaceSplitter::SavedTabsSpace::clear()
{
	x = 0;
	y = 0;
	currentTab = -1;
	tabs.clear();
}

QDataStream &operator<<(QDataStream &stream, const TabsSpaceSplitter::SavedTabsSpace &tabsSpace)
{
	stream << 1;
	stream << tabsSpace.x;
	stream << tabsSpace.y;
	stream << tabsSpace.homeUrl;
	stream << tabsSpace.tabs.count();

	for (int i{0}; i < tabsSpace.tabs.count(); ++i)
		stream << tabsSpace.tabs[i];

	stream << tabsSpace.currentTab;

	return stream;
}

QDataStream &operator>>(QDataStream &stream, TabsSpaceSplitter::SavedTabsSpace &tabsSpace)
{
	int version{0};
	stream >> version;

	if (version < 1)
		return stream;

	stream >> tabsSpace.x;
	stream >> tabsSpace.y;
	stream >> tabsSpace.homeUrl;

	int tabsCount{0};
	stream >> tabsCount;

	for(int i{0}; i < tabsCount; ++i) {
		WebTab::SavedTab tab{};
		stream >> tab;
		tabsSpace.tabs.append(tab);
	}

	stream >> tabsSpace.currentTab;

	return stream;
}

TabsSpaceSplitter::TabsSpaceSplitter(BrowserWindow* window) :
	QWidget(window),
	m_window(window)
{
	m_horizontalSplitter = new QSplitter(this);
	m_horizontalSplitter->setContentsMargins(0, 0, 0, 0);

	m_layout = new QVBoxLayout(this);
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setSpacing(0);
	m_layout->addWidget(m_horizontalSplitter);

	loadSettings();
}

void TabsSpaceSplitter::loadSettings()
{
	Settings settings{};

	m_tabsSpacePadding = settings.value(QLatin1String("Settings/tabsSpacesPadding"), 7).toInt();
	const bool showBookmarksToolBar{settings.value(QLatin1String("ShowBookmarksToolBar"), true).toBool()};
	const bool showActiveTabsSpace = settings.value("Settings/showActiveTabsSpace", true).toBool();

	// We can apply a padding between tabs space, exactly like i3 gaps
	foreach(TabWidget* tabWidget, m_tabWidgets)
	{
		tabWidget->loadSettings();
		tabWidget->updateShowBookmarksBarText(showBookmarksToolBar);

		tabWidget->parentWidget()->setStyleSheet("");

		if (tabWidget->parentWidget())
			tabWidget->parentWidget()->setContentsMargins(m_tabsSpacePadding, m_tabsSpacePadding,
														  m_tabsSpacePadding, m_tabsSpacePadding);

		if (tabWidget == m_currentTabWidget && showActiveTabsSpace)
			tabWidget->parentWidget()->setStyleSheet("#tabwidget-stack { border: 4px solid rgba(" + AppearancePage::colorString("accentnormal") + "); }");
	}
}

bool TabsSpaceSplitter::restoreTabsSpace(const SavedTabsSpace& tabsSpace)
{
	if (tabsSpace.tabs.isEmpty())
		return false;

	TabWidget* tabWidget{new TabWidget(m_window)};

	QVector<QPair<WebTab*, QVector<int>>> childTabs{};

	for (int i{0}; i < tabsSpace.tabs.count(); ++i) {
		WebTab::SavedTab tab = tabsSpace.tabs[i];
		WebTab *webTab = tabWidget->webTab(tabWidget->addView(QUrl(), Application::NTT_CleanSelectedTab, false, tab.isPinned));
		webTab->restoreTab(tab);
		
		if (!tab.childTabs.isEmpty()) 
			childTabs.append({webTab, tab.childTabs});
	}

	for (const auto p : childTabs) {
		const auto indices = p.second;
		
		for (int index : indices) {
			WebTab *t = tabWidget->webTab(index);
			if (t) 
				p.first->addChildTab(t);
		}
	}

	tabWidget->setCurrentIndex(tabsSpace.currentTab);
	tabWidget->setHomeUrl(tabsSpace.homeUrl);
	
	QTimer::singleShot(0, this, [tabWidget]() {
		tabWidget->tabBar()->ensureVisible();
	});

	tabWidget->webTab()->tabActivated();

	insertTabsSpace(tabsSpace.x, tabsSpace.y, tabWidget);

	return true;
}

void TabsSpaceSplitter::createNewTabsSpace(int x, int y)
{
	TabWidget* tabWidget = new TabWidget(m_window);

	insertTabsSpace(x, y, tabWidget);
}

void TabsSpaceSplitter::createNewTabsSpace(TabWidget* tabWidget)
{
	Q_ASSERT(!m_tabWidgets.contains(tabWidget));

	if (!tabWidget)
		tabWidget = new TabWidget(m_window);

	insertTabsSpace(m_horizontalSplitter->count(), 0, tabWidget);
}

void TabsSpaceSplitter::createNewTabsSpace(WebTab* startTab)
{
	TabWidget* tabWidget{new TabWidget(m_window)};
	tabWidget->addView(startTab, Application::NTT_SelectedTabAtEnd);

	insertTabsSpace(m_horizontalSplitter->count(), 0, tabWidget);
}

TabWidget* TabsSpaceSplitter::createNewTabsSpace(TabsSpacePosition position, TabWidget* from)
{
	TabWidget* tabWidget = new TabWidget(m_window);

	insertTabsSpace(position, tabWidget, from);

	return tabWidget;
}

void TabsSpaceSplitter::insertTabsSpace(TabsSpacePosition position, TabWidget* tabWidget, TabWidget* from)
{
	Q_ASSERT(m_tabWidgets.contains(from));

	QWidget* container = tabWidgetContainer(tabWidget);

	TabsSpaceInfo info = tabsSpaceInfo(from);

	if (position == TabsSpacePosition::TSP_Left) {
		QSplitter* newColumn{createColumn()};
		m_horizontalSplitter->insertWidget(info.x, newColumn->parentWidget());

		newColumn->addWidget(container);
	}
	if (position == TabsSpacePosition::TSP_Right) {
		QSplitter* newColumn{createColumn()};
		m_horizontalSplitter->insertWidget(info.x + 1, newColumn->parentWidget());

		newColumn->addWidget(container);
	}

	if (position == TabsSpacePosition::TSP_Top)
		m_verticalSplitter[info.column]->insertWidget(info.y, container);
	if (position == TabsSpacePosition::TSP_Bottom)
		m_verticalSplitter[info.column]->insertWidget(info.y + 1, container);
}

void TabsSpaceSplitter::insertTabsSpace(int x, int y, TabWidget* tabWidget)
{
	QWidget* container = tabWidgetContainer(tabWidget);
	int count = m_horizontalSplitter->count();

	if (m_horizontalSplitter->count() <= x) {
		QSplitter* newColumn{createColumn()};
		m_horizontalSplitter->addWidget(newColumn->parentWidget());

		newColumn->addWidget(container);
	}
	else {
		m_verticalSplitter[m_horizontalSplitter->widget(x)]->insertWidget(y, container);
	}
}

void TabsSpaceSplitter::removeTabsSpace(TabWidget* tabWidget)
{
	int index = m_tabWidgets.indexOf(tabWidget);

	if (index > 0)
		m_currentTabWidget = m_tabWidgets[0];
	else if (m_tabWidgets.count() > 1)
		m_currentTabWidget = m_tabWidgets[1];

	TabsSpaceInfo info = tabsSpaceInfo(tabWidget);

	QSplitter* column = m_verticalSplitter[info.column];

	m_tabWidgets.removeOne(tabWidget);

	// We have to wait drag operation to be done on unix system
#ifdef Q_OS_UNIX
	if (column->count() <= 1) {
		m_verticalSplitter.remove(info.column);
		QTimer::singleShot(200, info.column, &QWidget::deleteLater);
	}
	else {
		QTimer::singleShot(200, tabWidget->parentWidget(), &QWidget::deleteLater);
	}
#else
	if (column->count() <= 1) {
		m_verticalSplitter.remove(info.column);
		info.column->deleteLater();
	}
	else {
		tabWidget->parentWidget()->deleteLater();
	}
#endif
}

void TabsSpaceSplitter::autoResize()
{
	QList<int> mainSizes; // Vertical splitter size
	QList<int> verticalSizes; // All sizes of all vertical splitter

	for (int i{0}; i < m_horizontalSplitter->count(); ++i)
		mainSizes.append(m_horizontalSplitter->width() / m_horizontalSplitter->count());

	m_horizontalSplitter->setSizes(mainSizes);

	for (int i{0}; i < m_horizontalSplitter->count(); ++i) {
		verticalSizes.clear(); // We don't want to apply size of previous vertical splitter

		QSplitter* column = m_verticalSplitter[m_horizontalSplitter->widget(i)];

		for (int j{0}; j < column->count(); ++j)
			verticalSizes.append(column->height() / column->count());

		column->setSizes(verticalSizes);
	}
}

void TabsSpaceSplitter::tabsSpaceInFullView(TabWidget* tabWidget)
{
	TabsSpaceInfo info = tabsSpaceInfo(tabWidget);

	QList<int> sizes{};

	for (int i{0}; i < m_horizontalSplitter->count(); ++i) {
		QSplitter* column = m_verticalSplitter[m_horizontalSplitter->widget(i)];
		QList<int> verticalSizes{};

		if (info.x == i) {
			for (int j{0}; j < column->count(); ++j) {
				if (info.y == j)
					verticalSizes.append(height());
				else
					verticalSizes.append(0);
			}

			column->setSizes(verticalSizes);
			sizes.append(width());
		}
		else {
			sizes.append(0);
		}
	}

	m_horizontalSplitter->setSizes(sizes);
}

int TabsSpaceSplitter::count() const
{
	return m_tabWidgets.count();
}

int TabsSpaceSplitter::horizontalCount() const
{
	return m_horizontalSplitter->count();
}

void TabsSpaceSplitter::currentTabWidgetChanged(TabWidget* current)
{
	if (Settings().value("Settings/showActiveTabsSpace", true).toBool()) {
		if (m_currentTabWidget)
			m_currentTabWidget->parentWidget()->setStyleSheet("");

		current->parentWidget()->setStyleSheet("#tabwidget-stack { border: 4px solid rgba(" + AppearancePage::colorString("accentnormal") + "); }");
	}

	m_currentTabWidget = current;
}

TabWidget* TabsSpaceSplitter::tabWidget(int index) const
{
	return index >= 0 ? m_tabWidgets[index] : m_currentTabWidget;
}

TabWidget* TabsSpaceSplitter::tabWidget(QPoint position) const
{
	// Check on wich tabs space the floating button is to call the right slots
	foreach(TabWidget* tabWidget, m_tabWidgets)
	{
		QRect tabWidgetRect = tabWidget->geometry();

		if (tabWidgetRect.contains(tabWidget->mapFromGlobal(mapToGlobal(position)))) {
			return tabWidget;
		}
	}

	return m_currentTabWidget;
}

MaquetteGridItem* TabsSpaceSplitter::maquetteGridItem()
{
	QString name{Application::ensureUniqueFilename(DataPaths::currentProfilePath() + QLatin1String("/maquette-grid/") + tr("Session") + QLatin1String(".dat"))};
	name = QFileInfo(name).baseName();

	MaquetteGridItem* item{new MaquetteGridItem(name, true)};
	item->clear(); // We don't want default maquetteGrid.

	// Loop throug each tabs space
	foreach(TabWidget* tabWidget, m_tabWidgets)
	{
		SavedTabsSpace tabsSpace(this, tabWidget);

		item->addTabsSpace(tabsSpace);
	}

	return item;
}

TabsSpaceSplitter::TabsSpaceInfo TabsSpaceSplitter::tabsSpaceInfo(TabWidget* from) const
{
	bool found{false};

	for (int i{0}; i < m_horizontalSplitter->count(); ++i) {
		QSplitter* column = m_verticalSplitter[m_horizontalSplitter->widget(i)];

		for (int j{0}; j < column->count(); ++j) {
			if (column->widget(j) == from->parentWidget()) {
				TabsSpaceInfo info{};
				info.column = m_horizontalSplitter->widget(i);
				info.x = i;
				info.y = j;

				return info;
			}
		}
	}

	return TabsSpaceInfo();
}

bool TabsSpaceSplitter::valideCoordinates(int x, int y) const
{
	if (x >= m_horizontalSplitter->count())
		return false;

	if (y >= m_verticalSplitter.values(m_horizontalSplitter->widget(x)).count())
		return false;

	return true;
}

QWidget* TabsSpaceSplitter::tabWidgetContainer(TabWidget* tabWidget)
{
	Q_ASSERT(!m_tabWidgets.contains(tabWidget));

	QWidget* widget{new QWidget(this)};
	QVBoxLayout* layout{new QVBoxLayout(widget)};

	widget->setObjectName("tabsspace-container");
	widget->setContentsMargins(m_tabsSpacePadding, m_tabsSpacePadding, m_tabsSpacePadding, m_tabsSpacePadding);

	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

	m_tabWidgets.append(tabWidget);
	m_currentTabWidget = tabWidget;

	tabWidget->setParent(widget);
	tabWidget->tabBar()->show();

	layout->addWidget(tabWidget->tabBar());
	layout->addWidget(tabWidget);

	connect(tabWidget, &TabWidget::focusIn, m_window, &BrowserWindow::tabWidgetIndexChanged);
	connect(m_window->titleBar(), &TitleBar::toggleBookmarksBar, tabWidget,
			&TabWidget::updateShowBookmarksBarText);

	return widget;
}

QSplitter* TabsSpaceSplitter::createColumn()
{
	QWidget *column{new QWidget(this)};
	QSplitter* splitter{new QSplitter(Qt::Vertical, this)};
	QVBoxLayout* layout{new QVBoxLayout(column)};

	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(splitter);

	m_verticalSplitter[column] = splitter;

	return splitter;
}
}
