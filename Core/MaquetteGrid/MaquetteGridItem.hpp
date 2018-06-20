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
#ifndef SIELOBROWSER_MOCKUPITEM_HPP
#define SIELOBROWSER_MOCKUPITEM_HPP

#include <QIcon>

#include <QUrl>
#include <QString>

#include <QVector>

namespace Sn
{
class MaquetteGridItem {
public:
	struct TabsSpace;

	struct Tab {
		QIcon icon{};
		QString title{};
		QUrl url{};

		bool selected{false};

		TabsSpace* parent{nullptr};
	};

	struct TabsSpace {
		~TabsSpace() { qDeleteAll(tabs); }

		QVector<MaquetteGridItem::Tab*> tabs{};
		int verticalIndex{0};

		MaquetteGridItem* parent{nullptr};
	};

	MaquetteGridItem(const QString& name, bool loadDefault = false);
	~MaquetteGridItem();

	const QString &name() const { return m_name; }
	void setName(const QString& name, bool isDefaultName = false);

	void clear();

	void addTabsSpace(TabsSpace* tabsSpace);
	QList<TabsSpace*> tabsSpaces() const { return m_tabsSpaces; }

	void saveMaquetteGrid();
private:
	void loadMaquetteGrid(bool loadDefault);
	void loadMaquetteGridFromMap(const QVariantMap& map);

	bool sortTabsIndex(TabsSpace* first, TabsSpace* second);


	QList<TabsSpace*> m_tabsSpaces{};

	QString m_name{};
};
}

#endif //SIELOBROWSER_MOCKUPITEM_HPP
