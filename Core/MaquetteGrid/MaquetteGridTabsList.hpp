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
#ifndef SIELOBROWSER_MOCKUPSTABSLIST_HPP
#define SIELOBROWSER_MOCKUPSTABSLIST_HPP

#include "SharedDefines.hpp"

#include <QListWidget>

#include <QDropEvent>

#include <QVBoxLayout>

#include <QPushButton>

#include "MaquetteGrid/MaquetteGridItem.hpp"

namespace Sn
{
class MaquetteGridManager;

class SIELO_SHAREDLIB MaquetteGridTabsList: public QListWidget {
Q_OBJECT

public:
	MaquetteGridTabsList(MaquetteGridManager* manager, QWidget* parent = nullptr);
	~MaquetteGridTabsList();

	QVBoxLayout *parentLayout() const { return m_parentLayout; }
	void setParentLayout(QVBoxLayout* layout);

	TabsSpaceSplitter::SavedTabsSpace tabsSpace();

	MaquetteGridManager* manager() const { return m_maquetteGridManager; }

private slots:
	void deleteItem();
	void addTab();

protected:
	void dropEvent(QDropEvent* event);
	void enterEvent(QEvent* event);
	void leaveEvent(QEvent* event);
	void keyPressEvent(QKeyEvent* event);

private:
	MaquetteGridManager* m_maquetteGridManager{nullptr};
	QVBoxLayout* m_parentLayout{nullptr};

	QPushButton* m_deleteButton{nullptr};
	QPushButton* m_addTabButton{nullptr};

};
}

#endif //SIELOBROWSER_MOCKUPSTABSLIST_HPP
