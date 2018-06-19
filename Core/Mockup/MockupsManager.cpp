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

#include "MockupsManager.hpp"

#include "Mockup/Mockups.hpp"
#include "Mockup/MockupItem.hpp"
#include "Mockup/MockupsTabsList.hpp"

#include "Utils/AutoSaver.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
MockupsManager::MockupsManager(BrowserWindow* window) :
	QDialog(window),
	m_saver(new AutoSaver(this)),
	m_window(window),
	m_mockups(Application::instance()->mockups())
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName(QLatin1String("mockups-manager"));
	setupUI();

	setupMockupsList();
	m_mockupsListWidget->setCurrentRow(0);
	refresh(m_mockups->mockups()[0]);

	connect(m_mockupName, &QLineEdit::textEdited, this, &MockupsManager::changeMockupName);
	connect(m_newMockup, &QPushButton::clicked, this, &MockupsManager::newMockup);
	connect(m_newMockupFromCurrentSession, &QPushButton::clicked, this, &MockupsManager::newMockupFromCurrentSession);
	connect(m_deleteMockup, &QPushButton::clicked, this, &MockupsManager::deleteMockup);

	connect(m_title, SIGNAL(textEdited(QString)), this, SLOT(tabChanged()));
	connect(m_url, SIGNAL(textEdited(QString)), this, SLOT(tabChanged()));

	connect(m_mockupsListWidget, &QListWidget::itemClicked, this, &MockupsManager::mockupChanged);
}

MockupsManager::~MockupsManager()
{
	m_saver->saveIfNeccessary();
}

MockupsTabsList *MockupsManager::createNewTabsList()
{
	MockupsTabsList* list{new MockupsTabsList(this, m_tabsSpacesWidget)};

	QListWidgetItem* item{
		new QListWidgetItem(Application::getAppIcon("webpage"), QString("%1 (%2)").arg("New Tab").arg("https://"))
	};
	item->setData(MockupsManager::TitleRole, "New Tab");
	item->setData(MockupsManager::UrlRole, QUrl("https://"));

	list->addItem(item);

	m_tabsLists.append(list);

	connect(list, &MockupsTabsList::itemClicked, this, &MockupsManager::itemClicked);

	return list;
}

MockupsTabsList *MockupsManager::createTabsList(MockupItem::TabsSpace* tabsSpace)
{
	MockupsTabsList* list{new MockupsTabsList(this, m_tabsSpacesWidget)};

	for (int j{0}; j < tabsSpace->tabs.count(); ++j) {
		MockupItem::Tab* tab{tabsSpace->tabs[j]};
		QListWidgetItem* item{
			new QListWidgetItem(tab->icon, QString("%1 (%2)").arg(tab->title).arg(tab->url.toString()))
		};

		//item->setFlags(item->flags() & ~(Qt::ItemIsDropEnabled));
		item->setData(MockupsManager::TitleRole, tab->title);
		item->setData(MockupsManager::UrlRole, tab->url);
		/*item->setFlags(
		static_cast<Qt::ItemFlags>(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::
		ItemIsDropEnabled));*/
		list->addItem(item);
	}

	m_tabsLists.append(list);

	connect(list, &MockupsTabsList::itemClicked, this, &MockupsManager::itemClicked);

	return list;
}

QVBoxLayout *MockupsManager::createNewTabsSpaceLayout(MockupsTabsList* list)
{
	QVBoxLayout* tabsSpaceLayout{new QVBoxLayout()};

	AddButton* addTopButton{new AddButton(tabsSpaceLayout, m_tabsSpacesWidget)};
	AddButton* addBottomButton{new AddButton(tabsSpaceLayout, m_tabsSpacesWidget)};

	connect(addTopButton, &AddButton::clicked, this, &MockupsManager::createVerticalTabsSpace);
	connect(addBottomButton, &AddButton::clicked, this, &MockupsManager::createVerticalTabsSpace);

	tabsSpaceLayout->addWidget(addTopButton);
	tabsSpaceLayout->addWidget(list);
	tabsSpaceLayout->addWidget(addBottomButton);

	list->setParentLayout(tabsSpaceLayout);

	return tabsSpaceLayout;
}

void MockupsManager::save()
{
	if (!m_workingItem)
		return;

	QListWidgetItem* mockupListWidget{m_mockupsListWidget->currentItem()};

	if (mockupListWidget->text() != m_workingItem->name())
		m_workingItem->setName(mockupListWidget->text());

	mockupListWidget->setText(m_workingItem->name());
	m_mockupName->setText(m_workingItem->name());

	m_workingItem->clear();

	foreach(MockupsTabsList* list, m_tabsLists) {
		MockupItem::TabsSpace* tabsSpace{list->tabsSpace()};

		// "- 1" is a workaroud to be sure to have a 0 index for first column of tabs spaces
		tabsSpace->verticalIndex = m_verticalLayouts.indexOf(list->parentLayout()) - 1;
		tabsSpace->parent = m_workingItem;

		m_workingItem->addTabsSpace(tabsSpace);
	}

	m_workingItem->saveMockup();
}

void MockupsManager::newMockup()
{
	MockupItem* item{new MockupItem("mockup", true)};

	m_mockups->addMockup(item);

	m_mockupsListWidget->addItem(item->name());
	m_mockupsListWidget->setCurrentRow(m_mockupsListWidget->count() - 1);
	refresh(m_mockups->mockups().last());

	save();
}

void MockupsManager::newMockupFromCurrentSession()
{
	MockupItem* item{ m_window->mockupItem() };

	m_mockups->addMockup(item);

	m_mockupsListWidget->addItem(item->name());
	m_mockupsListWidget->setCurrentRow(m_mockupsListWidget->count() - 1);
	refresh(m_mockups->mockups().last());

	save();
}

void MockupsManager::deleteMockup()
{
	if (!m_workingItem)
		return;

	QListWidgetItem* itemToDelet{m_mockupsListWidget->currentItem()};
	delete itemToDelet;
	m_mockups->removeMockup(m_workingItem);

	if (m_mockupsListWidget->count() <= 0)
		newMockup();

	m_mockupsListWidget->setCurrentRow(0);
	refresh(m_mockups->mockups()[0]);
}

void MockupsManager::createVerticalTabsSpace()
{
	AddButton* button{qobject_cast<AddButton*>(sender())};

	if (!button)
		return;

	QVBoxLayout* layout{qobject_cast<QVBoxLayout*>(button->parentLayout())};
	MockupsTabsList* list{createNewTabsList()};
	AddButton* addBottomTabsSpace{new AddButton(layout, m_tabsSpacesWidget)};

	int index{layout->indexOf(button)};

	connect(addBottomTabsSpace, &AddButton::clicked, this, &MockupsManager::createVerticalTabsSpace);

	layout->insertWidget(index + 1, list);
	layout->insertWidget(index + 2, addBottomTabsSpace);

	list->setParentLayout(layout);

	m_saver->changeOccurred();
}

void MockupsManager::createHorizontalTabsSpace()
{
	AddButton* button{qobject_cast<AddButton*>(sender())};

	if (!button)
		return;

	QVBoxLayout* layout{qobject_cast<QVBoxLayout*>(button->parentLayout())};
	MockupsTabsList* list{createNewTabsList()};
	AddButton* addRightTabsSpace{new AddButton(layout, m_tabsSpacesWidget)};
	QVBoxLayout* tabsSpaceLayout{createNewTabsSpaceLayout(list)};
	QVBoxLayout* addRightLayout{new QVBoxLayout()};

	connect(addRightTabsSpace, &AddButton::clicked, this, &MockupsManager::createHorizontalTabsSpace);

	addRightLayout->addWidget(addRightTabsSpace);

	int index{m_verticalLayouts.indexOf(layout)};

	m_verticalLayouts.insert(index + 1, tabsSpaceLayout);
	m_verticalLayouts.insert(index + 2, addRightLayout);

	m_tabsSpacesLayout->insertLayout(index + 1, tabsSpaceLayout);
	m_tabsSpacesLayout->insertLayout(index + 2, addRightLayout);

	m_saver->changeOccurred();
}

void MockupsManager::removeTabsSpace(MockupsTabsList* tabsSpace)
{
	Q_ASSERT(tabsSpace->parentLayout());

	QVBoxLayout* layout{tabsSpace->parentLayout()};
	int index{layout->indexOf(tabsSpace)};
	AddButton* button{qobject_cast<AddButton*>(layout->itemAt(index - 1)->widget())};

	m_tabsLists.removeOne(tabsSpace);

	tabsSpace->deleteLater();
	button->deleteLater();

	if (layout->count() == 3) {
		AddButton* lastButton{qobject_cast<AddButton*>(layout->itemAt(2)->widget())};
		QVBoxLayout* addRightLayout{m_verticalLayouts[m_verticalLayouts.indexOf(layout) + 1]};
		AddButton* addRightButton{qobject_cast<AddButton*>(addRightLayout->itemAt(0)->widget())};

		m_verticalLayouts.removeOne(layout);
		m_verticalLayouts.removeOne(addRightLayout);

		lastButton->deleteLater();
		addRightLayout->deleteLater();
		addRightButton->deleteLater();
		layout->deleteLater();
	}

	m_activeItem = nullptr;
	updateEditInputs();

	m_saver->changeOccurred();
}

void MockupsManager::itemClicked(QListWidgetItem* item)
{
	if (item == m_activeItem)
		return;

	m_activeItem = item;
	updateEditInputs();
}

void MockupsManager::mockupChanged(QListWidgetItem* item)
{
	int row{m_mockupsListWidget->row(item)};

	m_saver->saveIfNeccessary();
	refresh(m_mockups->mockups()[row]);
}

void MockupsManager::changeMockupName(const QString& newName)
{
	if (!m_workingItem)
		return;

	m_mockupsListWidget->currentItem()->setText(newName);
	m_mockups->changeMockup(m_workingItem);

	m_saver->changeOccurred();
}

void MockupsManager::tabChanged()
{
	Q_ASSERT(m_activeItem);

	m_activeItem->setText(QString("%1 (%2)").arg(m_title->text()).arg(m_url->text()));
	m_activeItem->setData(TitleRole, m_title->text());
	m_activeItem->setData(UrlRole, m_url->text());

	m_saver->changeOccurred();
}

void MockupsManager::updateEditInputs()
{
	if (!m_activeItem) {
		m_url->setText("");
		m_title->setText("");
	}
	else {
		m_url->setText(m_activeItem->data(MockupsManager::UrlRole).toString());
		m_title->setText(m_activeItem->data(MockupsManager::TitleRole).toString());
	}

	m_url->setEnabled(m_activeItem);
	m_title->setEnabled(m_activeItem);
}

void MockupsManager::refresh(MockupItem* item)
{
	if (item == m_workingItem)
		return;

	m_workingItem = item;
	m_mockupName->setText(item->name());

	m_verticalLayouts.clear();
	m_tabsLists.clear();

	m_tabsSpacesWidget->deleteLater();
	m_tabsSpacesLayout->deleteLater();

	m_tabsSpacesWidget = new QWidget(m_scrollArea);
	m_tabsSpacesLayout = new QHBoxLayout(m_tabsSpacesWidget);
	m_scrollArea->setWidget(m_tabsSpacesWidget);

	setupTabsSpaces();
}

void MockupsManager::setupMockupsList()
{
	foreach (MockupItem* mitem, m_mockups->mockups()) {
		QListWidgetItem* item{new QListWidgetItem(mitem->name())};
		m_mockupsListWidget->addItem(item);
	}
}

void MockupsManager::setupFirstTabsSpace()
{
	MockupItem::TabsSpace* firstTabsSpace{m_workingItem->tabsSpaces()[0]};
	MockupsTabsList* firstList{createTabsList(firstTabsSpace)};

	QVBoxLayout* firstAddLeftLayout{new QVBoxLayout()};
	QVBoxLayout* firstTabsSpaceLayout{createNewTabsSpaceLayout(firstList)};
	QVBoxLayout* firstAddRightLayout{new QVBoxLayout()};

	AddButton* firstAddLeftButton{new AddButton(firstAddLeftLayout, m_tabsSpacesWidget)};
	AddButton* firstAddRightButton{new AddButton(firstAddRightLayout, m_tabsSpacesWidget)};

	connect(firstAddLeftButton, &AddButton::clicked, this, &MockupsManager::createHorizontalTabsSpace);
	connect(firstAddRightButton, &AddButton::clicked, this, &MockupsManager::createHorizontalTabsSpace);

	firstAddLeftLayout->addWidget(firstAddLeftButton);
	firstAddRightLayout->addWidget(firstAddRightButton);

	m_verticalLayouts.append(firstAddLeftLayout);
	m_verticalLayouts.append(firstTabsSpaceLayout);
	m_verticalLayouts.append(firstAddRightLayout);

	m_tabsSpacesLayout->addLayout(firstAddLeftLayout);
	m_tabsSpacesLayout->addLayout(firstTabsSpaceLayout);
	m_tabsSpacesLayout->addLayout(firstAddRightLayout);
}

void MockupsManager::setupTabsSpaces()
{
	Q_ASSERT(m_workingItem->tabsSpaces().count() >= 1);

	m_workingItem = m_workingItem;
	setupFirstTabsSpace();

	int workingColumn{0};
	for (int i{1}; i < m_workingItem->tabsSpaces().count(); ++i) {
		MockupItem::TabsSpace* tabsSpace{m_workingItem->tabsSpaces()[i]};
		MockupsTabsList* list{createTabsList(tabsSpace)};

		if (tabsSpace->verticalIndex == workingColumn) {
			QVBoxLayout* layout{m_verticalLayouts[m_verticalLayouts.count() - 2]};
			AddButton* addBottomTabsSpace{new AddButton(layout, m_tabsSpacesWidget)};

			connect(addBottomTabsSpace, &QPushButton::clicked, this, &MockupsManager::createVerticalTabsSpace);

			layout->addWidget(list);
			layout->addWidget(addBottomTabsSpace);

			list->setParentLayout(layout);
		}
		else {
			QVBoxLayout* tabsSpaceLayout{createNewTabsSpaceLayout(list)};
			QVBoxLayout* addRightLayout{new QVBoxLayout()};

			AddButton* addRightButton{new AddButton(addRightLayout, m_tabsSpacesWidget)};

			connect(addRightButton, &AddButton::clicked, this, &MockupsManager::createHorizontalTabsSpace);

			addRightLayout->addWidget(addRightButton);

			m_verticalLayouts.append(tabsSpaceLayout);
			m_verticalLayouts.append(addRightLayout);

			m_tabsSpacesLayout->addLayout(tabsSpaceLayout);
			m_tabsSpacesLayout->addLayout(addRightLayout);

			workingColumn = tabsSpace->verticalIndex;
		}
	}
}

void MockupsManager::setupUI()
{
	int maxListWidth{ 220 };
	resize(1082, 764);

	m_editBox = new QGroupBox(this);

	m_layout = new QGridLayout(this);
	m_editLayout = new QVBoxLayout(m_editBox);

	m_mockupsListWidget = new QListWidget(this);
	m_mockupsListWidget->setMaximumWidth(maxListWidth);

	m_mockupName = new QLineEdit(this);
	m_newMockup = new QPushButton(tr("Add Mockup"), this);
	m_newMockupFromCurrentSession = new QPushButton(tr("Add Mockup From Current Session"), this);
	m_deleteMockup = new QPushButton(tr("Delete Mockup"), this);

	m_mockupName->setMaximumWidth(maxListWidth);
	m_newMockup->setMaximumWidth(maxListWidth);
	m_newMockupFromCurrentSession->setMaximumWidth(maxListWidth);
	m_deleteMockup->setMaximumWidth(maxListWidth);

	m_scrollArea = new QScrollArea(this);
	m_tabsSpacesWidget = new QWidget(m_scrollArea);
	m_tabsSpacesLayout = new QHBoxLayout(m_tabsSpacesWidget);

	m_tabsSpacesWidget->setObjectName(QLatin1String("mockups-area-tabsspaces"));

	m_scrollArea->setObjectName(QLatin1String("mockups-area-tabsspaces"));
	m_scrollArea->setFrameShape(QFrame::NoFrame);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_scrollArea->setWidget(m_tabsSpacesWidget);

	m_titleDesc = new QLabel(tr("Title:"), this);
	m_title = new QLineEdit(this);
	m_urlDesc = new QLabel(tr("Url:"), this);
	m_url = new QLineEdit(this);

	m_title->setEnabled(false);
	m_url->setEnabled(false);

	m_editLayout->addWidget(m_titleDesc);
	m_editLayout->addWidget(m_title);
	m_editLayout->addWidget(m_urlDesc);
	m_editLayout->addWidget(m_url);

	m_layout->addWidget(m_mockupsListWidget, 0, 0, 1, 1);
	m_layout->addWidget(m_mockupName, 1, 0, 1, 1);
	m_layout->addWidget(m_newMockup, 2, 0, 1, 1);
	m_layout->addWidget(m_newMockupFromCurrentSession, 3, 0, 1, 1);
	m_layout->addWidget(m_deleteMockup, 4, 0, 1, 1);
	m_layout->addWidget(m_scrollArea, 0, 1, 1, 1);
	m_layout->addWidget(m_editBox, 1, 1, 4, 1);
}
}
