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

#include "MockupItem.hpp"

#include <QJsonParseError>
#include <QJsonDocument>

#include <QSaveFile>
#include <QFileInfo>

#include "Utils/IconProvider.hpp"

#include "Application.hpp"

namespace Sn
{
MockupItem::MockupItem(const QString& name, bool loadDefault) :
	m_name(name)
{
	loadMockup(loadDefault);
}

MockupItem::~MockupItem()
{
	// Empty
}

void MockupItem::setName(const QString& name, bool isDefaultName)
{
	QString oldFile{Application::paths()[Application::P_Mockups] + QLatin1Char('/') + m_name + QLatin1String(".json")};
	QString newFile{
		Application::ensureUniqueFilename(
			Application::paths()[Application::P_Mockups] + QLatin1Char('/') + name + QLatin1String(".json"))
	};

	//std::string strOldFile = oldFile.toStdString();
	//std::string strNewFile = newFile.toStdString();
	//std::string strName = name.toStdString();
	//std::string strBase = QFileInfo(newFile).baseName().toStdString();

	if (!isDefaultName && QFile::rename(oldFile, newFile))
		m_name = QFileInfo(newFile).baseName();
	else if (isDefaultName)
		m_name = QFileInfo(newFile).baseName();
}

void MockupItem::clear()
{
	m_tabsSpaces.clear();
}

void MockupItem::addTabsSpace(TabsSpace* tabsSpace)
{
	m_tabsSpaces.append(tabsSpace);
}

void MockupItem::saveMockup()
{
	std::sort(m_tabsSpaces.begin(), m_tabsSpaces.end(), [](MockupItem::TabsSpace* first, MockupItem::TabsSpace* second) {
		return first->verticalIndex < second->verticalIndex;
	});

	QVariantMap map{};
	QVariantList tabsSpacesList{};

	foreach(MockupItem::TabsSpace* tabsSpace, m_tabsSpaces) {
		QVariantMap tabsSpaceMap{};
		QVariantList tabsList{};

		foreach(MockupItem::Tab* tab, tabsSpace->tabs) {
			QVariantMap tabMap{};
			tabMap.insert("title", tab->title);
			tabMap.insert("url", tab->url);
			tabMap.insert("seleted", tab->selected);

			tabsList.append(tabMap);
		}

		tabsSpaceMap.insert("tabs", tabsList);
		tabsSpaceMap.insert("vertical_index", tabsSpace->verticalIndex);

		tabsSpacesList.append(tabsSpaceMap);
	}

	map.insert("version", 1);
	map.insert("tabs_spaces", tabsSpacesList);

	const QJsonDocument json{QJsonDocument::fromVariant(map)};
	const QByteArray data{json.toJson()};

	if (data.isEmpty()) {
		qWarning() << "MockupItem::saveMockup() Error serializing mockup!";
		return;
	}

	QSaveFile file{
		Application::paths()[Application::P_Mockups] + QLatin1Char('/') + m_name + QLatin1String(".json")
	};

	if (!file.open(QFile::WriteOnly)) {
		qWarning() << "MockupItem::saveMockup() Error opening mockup file for writing!";
		return;
	}

	file.write(data);
	file.commit();
}

void MockupItem::loadMockup(bool loadDefault)
{
	const QString mockupFile{
		Application::paths()[Application::P_Mockups] + QLatin1Char('/') + m_name + QLatin1String(".json")
	};
	const QString backupFile{mockupFile + QLatin1String(".old")};

	QJsonParseError err;
	QJsonDocument json = QJsonDocument::fromJson(Application::readAllFileByteContents(mockupFile), &err);
	const QVariant res = json.toVariant();

	if (loadDefault || err.error != QJsonParseError::NoError || res.type() != QVariant::Map) {
		if (QFile(mockupFile).exists() && !loadDefault) {
			qWarning() << "MockupItem::loadMockup() Error parsing mockup! Using default mockup!";
			qWarning() << "MockupItem::loadMockup() Your mockup have been backed up in" << backupFile;

			QFile::remove(backupFile);
			QFile::copy(mockupFile, backupFile);
		}

		json = QJsonDocument::fromJson(
			Application::readAllFileByteContents(QStringLiteral(":data/default-mockup.json")), &err);
		const QVariant data = json.toVariant();

		Q_ASSERT(err.error == QJsonParseError::NoError);
		Q_ASSERT(data.type() == QVariant::Map);

		loadMockupFromMap(data.toMap());

		setName(m_name, true);
		saveMockup();
	}
	else {
		loadMockupFromMap(res.toMap());
	}
}

void MockupItem::loadMockupFromMap(const QVariantMap& map)
{
	foreach (QVariant tabsSpaceData, map.value("tabs_spaces").toList()) {
		QVariantMap tabsSpaceMap{tabsSpaceData.toMap()};
		MockupItem::TabsSpace* tabsSpace{new MockupItem::TabsSpace()};

		foreach (QVariant tabData, tabsSpaceMap.value("tabs").toList()) {
			QVariantMap tabMap{tabData.toMap()};
			MockupItem::Tab* tab{new MockupItem::Tab()};

			tab->icon = IconProvider::iconForDomain(tabMap.value("url").toUrl());;
			tab->title = tabMap.value("title").toString();
			tab->url = tabMap.value("url").toUrl();
			tab->selected = tabMap.value("selected").toBool();
			tab->parent = tabsSpace;

			tabsSpace->tabs.append(tab);
		}

		tabsSpace->verticalIndex = tabsSpaceMap.value("vertical_index").toInt();
		tabsSpace->parent = this;

		m_tabsSpaces.append(tabsSpace);
	}
}
}
