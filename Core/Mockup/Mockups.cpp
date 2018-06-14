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

#include "Mockups.hpp"

#include <QDir>

#include "Mockup/MockupItem.hpp"

#include "Utils/AutoSaver.hpp"

#include "Application.hpp"

namespace Sn
{
Mockups::Mockups(QObject* parent) :
	QObject(parent),
	m_saver(new AutoSaver(this))
{
	loadMockups();
}

Mockups::~Mockups()
{
	m_saver->saveIfNeccessary();
	qDeleteAll(m_mockups);
}

void Mockups::addMockup(MockupItem* mockup)
{
	m_mockups.append(mockup);

	emit mockupAdded(mockup);

	m_saver->changeOccurred();
}

void Mockups::removeMockup(MockupItem* mockup)
{
	m_mockups.removeOne(mockup);

	emit mockupRemoved(mockup);

	m_saver->changeOccurred();
}

void Mockups::changeMockup(MockupItem* mockup)
{
	emit mockupChanged(mockup);

	m_saver->changeOccurred();
}

void Mockups::loadMockups()
{
	QDir directory{Application::paths()[Application::P_Mockups]};
	QFileInfoList files = directory.entryInfoList(QStringList("*.json"));

	foreach(const QFileInfo& info, files) {
		MockupItem* mockup{new MockupItem(info.baseName())};
		m_mockups.append(mockup);
	}
}

void Mockups::save()
{
	foreach(MockupItem* mockup, m_mockups)
		mockup->saveMockup();
}
}
