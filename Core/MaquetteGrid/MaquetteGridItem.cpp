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

#include "MaquetteGridItem.hpp"

#include <QJsonParseError>
#include <QJsonDocument>

#include <QSaveFile>
#include <QFileInfo>

#include "Utils/IconProvider.hpp"
#include "Utils/DataPaths.hpp"

#include "Application.hpp"

namespace Sn
{
MaquetteGridItem::MaquetteGridItem(const QString& name, bool loadDefault) :
	m_name(name)
{
	QString maquetteGridFile{DataPaths::currentProfilePath() + QLatin1String("/maquette-grid/") + m_name + QLatin1String(".dat")};
	const QString backupFile{maquetteGridFile + QLatin1String(".old")};
	
	m_valid = RestoreManager::validateFile(maquetteGridFile);

	if (loadDefault || !m_valid) {
		if (!m_valid) {
			qWarning() << "MaquetteGridItem::loadMaquetteGrid() Error parsing maquetteGrid! Using default maquetteGrid!";
			qWarning() << "MaquetteGridItem::loadMaquetteGrid() Your maquetteGrid have been backed up in" << backupFile;

			QFile::remove(backupFile);
			QFile::copy(maquetteGridFile, backupFile);
		}

		maquetteGridFile = DataPaths::currentProfilePath() + QLatin1String("/maquette-grid/default.dat");
	}

	RestoreManager::createFromFile(maquetteGridFile, m_data);

	foreach (TabsSpaceSplitter::SavedTabsSpace tabsSpace, m_data.windows[0].tabsSpaces)
		addTabsSpace(tabsSpace);
}

MaquetteGridItem::~MaquetteGridItem()
{
	// Empty
}

void MaquetteGridItem::setName(const QString& name, bool isDefaultName)
{
	if (m_name == "default")
		return;

	QString oldFile{DataPaths::currentProfilePath() + QLatin1String("/maquette-grid/") + m_name + QLatin1String(".dat")};
	QString newFile{
		Application::ensureUniqueFilename(DataPaths::currentProfilePath() + QLatin1String("/maquette-grid/") + name + QLatin1String(".dat"))
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

void MaquetteGridItem::clear()
{
	m_tabsSpaces.clear();
}

void MaquetteGridItem::addTabsSpace(TabsSpaceSplitter::SavedTabsSpace tabsSpace)
{
	m_tabsSpaces.append(tabsSpace);
}

void MaquetteGridItem::saveMaquetteGrid()
{
	QByteArray data{};
	QDataStream stream{&data, QIODevice::WriteOnly};

	RestoreData restoreData{};
	restoreData.windows.reserve(1);

	restoreData.windows.append(BrowserWindow::SavedWindow(this));

	stream << 1;
	stream << restoreData;

	// Save data to a file
	QFile file{};
	file.setFileName(DataPaths::currentProfilePath() + QLatin1String("/maquette-grid/") + m_name + QLatin1String(".dat"));

	file.open(QIODevice::WriteOnly);
	file.write(data);
	file.close();
}
}
