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
#ifndef SIELOBROWSER_MOCKUPSMANAGER_HPP
#define SIELOBROWSER_MOCKUPSMANAGER_HPP

#include "SharedDefines.hpp"

#include <QDialog>
#include <QScrollArea>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QGroupBox>

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>

#include <QListWidgetItem>

#include "MaquetteGrid/MaquetteGridItem.hpp"

namespace Sn
{
class AutoSaver;

class BrowserWindow;

class MaquetteGrid;
class MaquetteGridTabsList;
class MaquetteGridModel;

class SIELO_SHAREDLIB AddButton: public QPushButton {
Q_OBJECT

public:
	AddButton(QVBoxLayout* parentLayout, QWidget* parent = nullptr) :
		QPushButton("+", parent),
		m_parentLayout(parentLayout)
	{
		setObjectName(QLatin1String("maquetteGrid-addtabsspace-buttons"));
		setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}

	QVBoxLayout *parentLayout() const { return m_parentLayout; }

private:
	QVBoxLayout* m_parentLayout{nullptr};
};

class SIELO_SHAREDLIB MaquetteGridManager: public QDialog {
Q_OBJECT

public:
	enum Roles {
		TitleRole = Qt::UserRole + 1,
		UrlRole = Qt::UserRole + 2
	};

	MaquetteGridManager(BrowserWindow* window);
	~MaquetteGridManager();

	MaquetteGridTabsList *createNewTabsList();
	MaquetteGridTabsList *createTabsList(TabsSpaceSplitter::SavedTabsSpace tabsSpace);
	QVBoxLayout *createNewTabsSpaceLayout(MaquetteGridTabsList* list);

	BrowserWindow* window() const { return m_window; }
	AutoSaver *saver() const { return m_saver; }

public slots:
	void save();

	void newMaquetteGrid();
	void newMaquetteGridFromCurrentSession();
	void deleteMaquetteGrid();

	void createVerticalTabsSpace();
	void createHorizontalTabsSpace();
	void removeTabsSpace(MaquetteGridTabsList* tabsSpace);

private slots:
	void itemClicked(QListWidgetItem* item);
	void maquetteGridChanged(QListWidgetItem* item);

	void changeMaquetteGridName(const QString& newName);

	void tabChanged();

private:
	void updateEditInputs();

	void refresh(MaquetteGridItem* item);

	void setupMaquetteGridList();
	void setupFirstTabsSpace();
	void setupTabsSpaces();
	void setupUI();

	AutoSaver* m_saver{nullptr};
	BrowserWindow* m_window{nullptr};

	QList<QVBoxLayout*> m_verticalLayouts{};
	QList<MaquetteGridTabsList*> m_tabsLists{};

	QGridLayout* m_layout{nullptr};
	QVBoxLayout* m_editLayout{nullptr};

	QListWidget* m_maquetteGridListWidget{nullptr};
	QLineEdit* m_maquetteGridName{nullptr};
	QPushButton* m_newMaquetteGrid{nullptr};
	QPushButton* m_newMaquetteGridFromCurrentSession{nullptr};
	QPushButton* m_deleteMaquetteGrid{nullptr};

	QGroupBox* m_editBox{nullptr};

	QWidget* m_tabsSpacesWidget{nullptr};
	QHBoxLayout* m_tabsSpacesLayout{nullptr};
	QScrollArea* m_scrollArea{nullptr};

	QLabel* m_titleDesc{nullptr};
	QLineEdit* m_title{nullptr};
	QLabel* m_urlDesc{nullptr};
	QLineEdit* m_url{nullptr};

	QListWidgetItem* m_activeItem{nullptr};
	MaquetteGridItem* m_workingItem{nullptr};
	MaquetteGrid* m_maquetteGrid{nullptr};
};
}

#endif //SIELOBROWSER_MOCKUPSMANAGER_HPP
