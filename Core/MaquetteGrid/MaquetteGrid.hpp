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
#ifndef SIELOBROWSER_MOCKUP_HPP
#define SIELOBROWSER_MOCKUP_HPP

#include "SharedDefines.hpp"

#include <QObject>

#include <QIcon>

#include <QUrl>
#include <QString>

namespace Sn
{
class AutoSaver;

class MaquetteGridItem;

class SIELO_SHAREDLIB MaquetteGrid: public QObject {
Q_OBJECT

public:
	MaquetteGrid(QObject* parent = nullptr);
	~MaquetteGrid();

	void addMaquetteGrid(MaquetteGridItem* maquetteGrid);
	void removeMaquetteGrid(MaquetteGridItem* maquetteGrid);
	void changeMaquetteGrid(MaquetteGridItem* maquetteGrid);

	void loadMaquetteGrid();
	void save();

	QList<MaquetteGridItem*> maquetteGrid() { return m_maquetteGrid; }

signals:
	void maquetteGridAdded(MaquetteGridItem* maquetteGrid);
	void maquetteGridRemoved(MaquetteGridItem* maquetteGrid);
	void maquetteGridChanged(MaquetteGridItem* maquetteGrid);

private:
	QList<MaquetteGridItem*> m_maquetteGrid{};

	AutoSaver* m_saver{nullptr};
};
}

#endif //SIELOBROWSER_MOCKUP_HPP
