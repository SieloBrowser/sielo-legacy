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

#include "Mockup/MockupItem.hpp"

namespace Sn
{
class AutoSaver;

class Mockups;
class MockupsTabsList;
class MockupsModel;

class AddButton: public QPushButton {
Q_OBJECT

public:
	AddButton(QVBoxLayout* parentLayout, QWidget* parent = nullptr) :
		QPushButton("+", parent),
		m_parentLayout(parentLayout)
	{
		setObjectName(QLatin1String("mockups-addtabsspace-buttons"));
		setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}

	QVBoxLayout *parentLayout() const { return m_parentLayout; }

private:
	QVBoxLayout* m_parentLayout{nullptr};
};

class MockupsManager: public QDialog {
Q_OBJECT

public:
	enum Roles {
		TitleRole = Qt::UserRole + 1,
		UrlRole = Qt::UserRole + 2
	};

	MockupsManager(QWidget* parent = nullptr);
	~MockupsManager();

	MockupsTabsList *createNewTabsList();
	MockupsTabsList *createTabsList(MockupItem::TabsSpace* tabsSpace);
	QVBoxLayout *createNewTabsSpaceLayout(MockupsTabsList* list);

	AutoSaver *saver() const { return m_saver; }

public slots:
	void save();

	void newMockup();
	void newMockupFromCurrentSession();
	void deleteMockup();

	void createVerticalTabsSpace();
	void createHorizontalTabsSpace();
	void removeTabsSpace(MockupsTabsList* tabsSpace);

private slots:
	void itemClicked(QListWidgetItem* item);
	void mockupChanged(QListWidgetItem* item);

	void changeMockupName(const QString& newName);

	void tabChanged();

private:
	void updateEditInputs();

	void refresh(MockupItem* item);

	void setupMockupsList();
	void setupFirstTabsSpace();
	void setupTabsSpaces();
	void setupUI();

	AutoSaver* m_saver{nullptr};

	QList<QVBoxLayout*> m_verticalLayouts{};
	QList<MockupsTabsList*> m_tabsLists{};

	QGridLayout* m_layout{nullptr};
	QVBoxLayout* m_editLayout{nullptr};

	QListWidget* m_mockupsListWidget{nullptr};
	QLineEdit* m_mockupName{nullptr};
	QPushButton* m_newMockup{ nullptr };
	QPushButton* m_newMockupFromCurrentSession{ nullptr };
	QPushButton* m_deleteMockup{ nullptr };

	QGroupBox* m_editBox{nullptr};

	QWidget* m_tabsSpacesWidget{nullptr};
	QHBoxLayout* m_tabsSpacesLayout{nullptr};
	QScrollArea* m_scrollArea{nullptr};

	QLabel* m_titleDesc{nullptr};
	QLineEdit* m_title{nullptr};
	QLabel* m_urlDesc{nullptr};
	QLineEdit* m_url{nullptr};

	QListWidgetItem* m_activeItem{nullptr};
	MockupItem* m_workingItem{nullptr};
	Mockups* m_mockups{nullptr};
};
}

#endif //SIELOBROWSER_MOCKUPSMANAGER_HPP
