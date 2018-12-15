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
#ifndef SIELOBROWSER_TABSSPACESPLITTER_HPP
#define SIELOBROWSER_TABSSPACESPLITTER_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QSplitter>
#include <QVBoxLayout>

#include "Web/Tab/WebTab.hpp"

namespace Sn
{

class WebTab;

class MaquetteGridItem;
class MaquetteGridTabsList;

class BrowserWindow;
class TabWidget;

class SIELO_SHAREDLIB TabsSpaceSplitter: public QWidget {
	Q_OBJECT

public:
	struct SavedTabsSpace{
		int x{0};
		int y{0};
		int currentTab{-1};
		QString homeUrl{"https://doosearch.sielo.app"};
		QVector<WebTab::SavedTab> tabs{};

		SavedTabsSpace();
		SavedTabsSpace(TabsSpaceSplitter* splitter, TabWidget* tabWidget);
		SavedTabsSpace(MaquetteGridTabsList* maquetteGridTabsList);

		bool isValid() const;
		void clear();

		friend QDataStream &operator<<(QDataStream &stream, const SavedTabsSpace &tabsSpace);
		friend QDataStream &operator>>(QDataStream &stream, SavedTabsSpace &tabsSpace);
	};

	struct TabsSpaceInfo {

		TabsSpaceInfo() {};

		int x{0};
		int y{0};
		QWidget* column{nullptr};
	};

	//! Tabs Space Position.
	/* Position where a tabs space can be create. */
	enum TabsSpacePosition {
		TSP_Left, /*!< Create tabs space to the left. */
		TSP_Right, /*!< Create tabs space to the right. */
		TSP_Top, /*!< Create tabs space to the top. */
		TSP_Bottom /*!< Create tabs space to the bottom. */
	};

	TabsSpaceSplitter(BrowserWindow* window);
	~TabsSpaceSplitter() = default;

	void loadSettings();

	bool restoreTabsSpace(const SavedTabsSpace& tabsSpace);

	void createNewTabsSpace(TabWidget* tabWidget = nullptr);
	void createNewTabsSpace(WebTab* startTab);
	void createNewTabsSpace(int x, int y);
	TabWidget* createNewTabsSpace(TabsSpacePosition position, TabWidget* from);
	
	void insertTabsSpace(TabsSpacePosition position, TabWidget* tabWidget, TabWidget* from);
	void insertTabsSpace(int x, int y, TabWidget* tabWidget);

	void removeTabsSpace(TabWidget* tabWidget);

	int count() const;
	int horizontalCount() const;

	void currentTabWidgetChanged(TabWidget* current);
	TabWidget* tabWidget(int index = -1) const;
	TabWidget* tabWidget(QPoint position) const;
	QVector<TabWidget*> tabWidgets() const { return m_tabWidgets; }

	MaquetteGridItem* maquetteGridItem();

	TabsSpaceInfo tabsSpaceInfo(TabWidget* from) const;

public slots:
	void autoResize();
	void tabsSpaceInFullView(TabWidget* tabWidget);

private:
	bool valideCoordinates(int x, int y) const;
	QWidget* tabWidgetContainer(TabWidget* tabWidget);
	QSplitter* createColumn();

	BrowserWindow* m_window{nullptr};
	TabWidget* m_currentTabWidget{nullptr};
	QVector<TabWidget*> m_tabWidgets{};

	QVBoxLayout* m_layout{nullptr};
	QSplitter* m_horizontalSplitter{nullptr};
	QHash<QWidget*, QSplitter*> m_verticalSplitter{};

	int m_tabsSpacePadding{7};
};
}

#endif //SIELOBROWSER_TABSSPACESPLITTER_HPP