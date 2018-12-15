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

#include "MaquetteGrid.hpp"

#include <QDir>

#include "MaquetteGrid/MaquetteGridItem.hpp"

#include "Utils/AutoSaver.hpp"
#include "Utils/DataPaths.hpp"

#include "Application.hpp"

namespace Sn
{
MaquetteGrid::MaquetteGrid(QObject* parent) :
	QObject(parent),
	m_saver(new AutoSaver(this))
{
	loadMaquetteGrid();
}

MaquetteGrid::~MaquetteGrid()
{
	m_saver->saveIfNeccessary();
	qDeleteAll(m_maquetteGrid);
}

void MaquetteGrid::addMaquetteGrid(MaquetteGridItem* maquetteGrid)
{
	m_maquetteGrid.append(maquetteGrid);

	emit maquetteGridAdded(maquetteGrid);

	m_saver->changeOccurred();
}

void MaquetteGrid::removeMaquetteGrid(MaquetteGridItem* maquetteGrid)
{
	m_maquetteGrid.removeOne(maquetteGrid);
	QFile::remove(DataPaths::currentProfilePath() + QLatin1String("/maquette-grid/") + maquetteGrid->name() + QLatin1String(".dat"));

	emit maquetteGridRemoved(maquetteGrid);

	m_saver->changeOccurred();
}

void MaquetteGrid::changeMaquetteGrid(MaquetteGridItem* maquetteGrid)
{
	emit maquetteGridChanged(maquetteGrid);

	m_saver->changeOccurred();
}

void MaquetteGrid::loadMaquetteGrid()
{
	QDir directory{DataPaths::currentProfilePath() + QLatin1String("/maquette-grid/")};
	QFileInfoList files = directory.entryInfoList(QStringList("*.dat"));

	foreach(const QFileInfo& info, files) {
		MaquetteGridItem* maquetteGrid{new MaquetteGridItem(info.baseName())};
		m_maquetteGrid.append(maquetteGrid);
	}

	if (m_maquetteGrid.isEmpty())
		m_maquetteGrid.append(new MaquetteGridItem("default", true));
}

void MaquetteGrid::save()
{
	foreach(MaquetteGridItem* maquetteGrid, m_maquetteGrid)
		maquetteGrid->saveMaquetteGrid();
}
}
