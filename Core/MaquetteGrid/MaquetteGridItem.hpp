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

#include "SharedDefines.hpp"

#include <QIcon>

#include <QUrl>
#include <QString>

#include <QVector>

#include "Widgets/Tab/TabsSpaceSplitter.hpp"
#include "Utils/RestoreManager.hpp"

namespace Sn
{
class SIELO_SHAREDLIB MaquetteGridItem {
public:
	MaquetteGridItem(const QString& name, bool loadDefault = false);
	~MaquetteGridItem();

	const QString &name() const { return m_name; }
	void setName(const QString& name, bool isDefaultName = false);

	void clear();

	void addTabsSpace(TabsSpaceSplitter::SavedTabsSpace tabsSpace);
	QList<TabsSpaceSplitter::SavedTabsSpace> tabsSpaces() const { return m_tabsSpaces; }

	RestoreData data() const { return m_data; }

	void saveMaquetteGrid();

private:
	bool sortTabsIndex(TabsSpaceSplitter::SavedTabsSpace* first, TabsSpaceSplitter::SavedTabsSpace* second);

	QList<TabsSpaceSplitter::SavedTabsSpace> m_tabsSpaces{};
	RestoreData m_data{};

	QString m_name{};
	bool m_valid{false};
};
}

#endif //SIELOBROWSER_MOCKUPITEM_HPP
