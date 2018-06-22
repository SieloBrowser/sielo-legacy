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

#include "AddressBarCompleterModel.hpp"

#include <QPixmap>

#include "History/History.hpp"

#include "Web/Tab/WebTab.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
AddressBarCompleterModel::AddressBarCompleterModel(QObject* parent) :
	QStandardItemModel(parent)
{
	// Empty
}

void AddressBarCompleterModel::setCompletions(const QList<QStandardItem*>& items)
{
	clear();
	addCompletions(items);
}

void AddressBarCompleterModel::addCompletions(const QList<QStandardItem*>& items)
{
	foreach (QStandardItem* item, items) {
		item->setIcon(QPixmap::fromImage(item->data(ImageRole).value<QImage>()));
		setTabPosition(item);

		if (item->icon().isNull())
			item->setIcon(Application::getAppIcon("webpage"));

		appendRow(QList<QStandardItem*>{item});
	}
}

QList<QStandardItem*> AddressBarCompleterModel::suggestionItems() const
{
	QList<QStandardItem*> items{};

	for (int i{0}; i < rowCount(); ++i) {
		QStandardItem* it{item(i)};

		if (it->data(SearchSuggestionRole).toBool())
			items.append(it);
	}

	return items;
}

void AddressBarCompleterModel::setTabPosition(QStandardItem* item) const
{
	const QUrl url{item->data(UrlRole).toUrl()};
	const QList<BrowserWindow*> windows = Application::instance()->windows();

	foreach (BrowserWindow* window, windows) {
		QVector<TabWidget*> tabWidgets = window->tabWidgets();
		foreach (TabWidget* tabWidget, tabWidgets) {
			QList<WebTab*> tabs = tabWidget->allTabs();
			for (int i{0}; i < tabs.count(); ++i) {
				WebTab* tab{tabs[i]};

				if (tab->url() == url) {
					item->setData(QVariant::fromValue<void*>(static_cast<void*>(tabWidget)), TabPositionTabsSpaceRole);
					item->setData(i, TabPositionTabRole);
				}
			}
		}
	}

	// Tab wasn't found
	item->setData(QVariant::fromValue<void*>(static_cast<void*>(0)), TabPositionTabsSpaceRole);
	item->setData(-1, TabPositionTabRole);
}

void AddressBarCompleterModel::refreshTabPositions() const
{
	for (int row{0}; row < rowCount(); ++row) {
		QStandardItem* itm{item(row)};
		if (itm)
			setTabPosition(itm);
	}
}
}
