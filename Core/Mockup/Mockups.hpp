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

#include <QObject>

#include <QIcon>

#include <QUrl>
#include <QString>

namespace Sn
{
class AutoSaver;

class MockupItem;

class Mockups: public QObject {
Q_OBJECT

public:
	Mockups(QObject* parent = nullptr);
	~Mockups();

	void addMockup(MockupItem* mockup);
	void removeMockup(MockupItem* mockup);
	void changeMockup(MockupItem* mockup);

	void loadMockups();
	void save();

	QList<MockupItem*> mockups() { return m_mockups; }

signals:
	void mockupAdded(MockupItem* mockup);
	void mockupRemoved(MockupItem* mockup);
	void mockupChanged(MockupItem* mockup);

private:
	QList<MockupItem*> m_mockups{};

	AutoSaver* m_saver{nullptr};
};
}

#endif //SIELOBROWSER_MOCKUP_HPP
