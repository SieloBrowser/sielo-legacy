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

#include "MaquetteGridManager.hpp"

#include <QFileInfo>

#include "Utils/DataPaths.hpp"

#include "MaquetteGrid/MaquetteGrid.hpp"
#include "MaquetteGrid/MaquetteGridItem.hpp"
#include "MaquetteGrid/MaquetteGridTabsList.hpp"

#include "Utils/AutoSaver.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
MaquetteGridManager::MaquetteGridManager(BrowserWindow* window) :
	QDialog(window),
	m_saver(new AutoSaver(this)),
	m_window(window),
	m_maquetteGrid(Application::instance()->maquetteGrid())
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName(QLatin1String("maquettegrid-manager"));
	setWindowTitle("Maquette Grid Manager");
	setupUI();

	setupMaquetteGridList();
	m_maquetteGridListWidget->setCurrentRow(0);
	refresh(m_maquetteGrid->maquetteGrid()[0]);

	connect(m_maquetteGridName, &QLineEdit::textEdited, this, &MaquetteGridManager::changeMaquetteGridName);
	connect(m_newMaquetteGrid, &QPushButton::clicked, this, &MaquetteGridManager::newMaquetteGrid);
	connect(m_newMaquetteGridFromCurrentSession, &QPushButton::clicked, this, &MaquetteGridManager::newMaquetteGridFromCurrentSession);
	connect(m_deleteMaquetteGrid, &QPushButton::clicked, this, &MaquetteGridManager::deleteMaquetteGrid);

	connect(m_title, SIGNAL(textEdited(QString)), this, SLOT(tabChanged()));
	connect(m_url, SIGNAL(textEdited(QString)), this, SLOT(tabChanged()));

	connect(m_maquetteGridListWidget, &QListWidget::itemClicked, this, &MaquetteGridManager::maquetteGridChanged);
}

MaquetteGridManager::~MaquetteGridManager()
{
	m_saver->saveIfNeccessary();
}

MaquetteGridTabsList *MaquetteGridManager::createNewTabsList()
{
	MaquetteGridTabsList* list{new MaquetteGridTabsList(this, m_tabsSpacesWidget)};

	QListWidgetItem* item{
		new QListWidgetItem(Application::getAppIcon("webpage"), QString("%1 (%2)").arg("New Tab").arg("https://"))
	};
	item->setData(MaquetteGridManager::TitleRole, "New Tab");
	item->setData(MaquetteGridManager::UrlRole, QUrl("https://"));

	list->addItem(item);

	m_tabsLists.append(list);

	connect(list, &MaquetteGridTabsList::itemClicked, this, &MaquetteGridManager::itemClicked);

	return list;
}

MaquetteGridTabsList *MaquetteGridManager::createTabsList(TabsSpaceSplitter::SavedTabsSpace tabsSpace)
{
	MaquetteGridTabsList* list{new MaquetteGridTabsList(this, m_tabsSpacesWidget)};

	for (int j{0}; j < tabsSpace.tabs.count(); ++j) {
		WebTab::SavedTab tab{tabsSpace.tabs[j]};
		QListWidgetItem* item{
			new QListWidgetItem(tab.icon, QString("%1 (%2)").arg(tab.title).arg(tab.url.toString()))
		};

		//item->setFlags(item->flags() & ~(Qt::ItemIsDropEnabled));
		item->setData(MaquetteGridManager::TitleRole, tab.title);
		item->setData(MaquetteGridManager::UrlRole, tab.url);
		/*item->setFlags(
		static_cast<Qt::ItemFlags>(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::
		ItemIsDropEnabled));*/
		list->addItem(item);
	}

	m_tabsLists.append(list);

	connect(list, &MaquetteGridTabsList::itemClicked, this, &MaquetteGridManager::itemClicked);

	return list;
}

QVBoxLayout *MaquetteGridManager::createNewTabsSpaceLayout(MaquetteGridTabsList* list)
{
	QVBoxLayout* tabsSpaceLayout{new QVBoxLayout()};

	AddButton* addTopButton{new AddButton(tabsSpaceLayout, m_tabsSpacesWidget)};
	AddButton* addBottomButton{new AddButton(tabsSpaceLayout, m_tabsSpacesWidget)};

	connect(addTopButton, &AddButton::clicked, this, &MaquetteGridManager::createVerticalTabsSpace);
	connect(addBottomButton, &AddButton::clicked, this, &MaquetteGridManager::createVerticalTabsSpace);

	tabsSpaceLayout->addWidget(addTopButton);
	tabsSpaceLayout->addWidget(list);
	tabsSpaceLayout->addWidget(addBottomButton);

	list->setParentLayout(tabsSpaceLayout);

	return tabsSpaceLayout;
}

void MaquetteGridManager::save()
{
	if (!m_workingItem)
		return;

	QListWidgetItem* maquetteGridListWidget{m_maquetteGridListWidget->currentItem()};

	if (maquetteGridListWidget->text() != m_workingItem->name())
		m_workingItem->setName(maquetteGridListWidget->text());

	maquetteGridListWidget->setText(m_workingItem->name());
	m_maquetteGridName->setText(m_workingItem->name());

	m_workingItem->clear();

	foreach(MaquetteGridTabsList* list, m_tabsLists) {
		TabsSpaceSplitter::SavedTabsSpace tabsSpace{list->tabsSpace()};

		// "- 1" is a workaroud to be sure to have a 0 index for first column of tabs spaces
		tabsSpace.x = m_verticalLayouts.indexOf(list->parentLayout()) - 1;
		tabsSpace.y = list->parentLayout()->indexOf(list); 

		m_workingItem->addTabsSpace(tabsSpace);
	}

	m_workingItem->saveMaquetteGrid();
}

void MaquetteGridManager::newMaquetteGrid()
{
	QString name{Application::ensureUniqueFilename(DataPaths::currentProfilePath() + QLatin1String("/maquette-grid/maquette-grid.dat"))};
	name = QFileInfo(name).baseName();

	MaquetteGridItem* item{new MaquetteGridItem(name, true)};

	m_maquetteGrid->addMaquetteGrid(item);

	m_maquetteGridListWidget->addItem(item->name());
	m_maquetteGridListWidget->setCurrentRow(m_maquetteGridListWidget->count() - 1);
	refresh(m_maquetteGrid->maquetteGrid().last());

	save();
}

void MaquetteGridManager::newMaquetteGridFromCurrentSession()
{
	MaquetteGridItem* item{ m_window->tabsSpaceSplitter()->maquetteGridItem() };

	m_maquetteGrid->addMaquetteGrid(item);

	m_maquetteGridListWidget->addItem(item->name());
	m_maquetteGridListWidget->setCurrentRow(m_maquetteGridListWidget->count() - 1);
	refresh(m_maquetteGrid->maquetteGrid().last());

	save();
}

void MaquetteGridManager::deleteMaquetteGrid()
{
	if (!m_workingItem || m_workingItem->name() == "default")
		return;

	QListWidgetItem* itemToDelet{m_maquetteGridListWidget->currentItem()};
	delete itemToDelet;
	m_maquetteGrid->removeMaquetteGrid(m_workingItem);

	if (m_maquetteGridListWidget->count() <= 0)
		newMaquetteGrid();

	m_maquetteGridListWidget->setCurrentRow(0);
	refresh(m_maquetteGrid->maquetteGrid()[0]);
}

void MaquetteGridManager::createVerticalTabsSpace()
{
	AddButton* button{qobject_cast<AddButton*>(sender())};

	if (!button)
		return;

	QVBoxLayout* layout{qobject_cast<QVBoxLayout*>(button->parentLayout())};
	MaquetteGridTabsList* list{createNewTabsList()};
	AddButton* addBottomTabsSpace{new AddButton(layout, m_tabsSpacesWidget)};

	int index{layout->indexOf(button)};

	connect(addBottomTabsSpace, &AddButton::clicked, this, &MaquetteGridManager::createVerticalTabsSpace);

	layout->insertWidget(index + 1, list);
	layout->insertWidget(index + 2, addBottomTabsSpace);

	list->setParentLayout(layout);

	m_saver->changeOccurred();
}

void MaquetteGridManager::createHorizontalTabsSpace()
{
	AddButton* button{qobject_cast<AddButton*>(sender())};

	if (!button)
		return;

	QVBoxLayout* layout{qobject_cast<QVBoxLayout*>(button->parentLayout())};
	MaquetteGridTabsList* list{createNewTabsList()};
	AddButton* addRightTabsSpace{new AddButton(layout, m_tabsSpacesWidget)};
	QVBoxLayout* tabsSpaceLayout{createNewTabsSpaceLayout(list)};
	QVBoxLayout* addRightLayout{new QVBoxLayout()};

	connect(addRightTabsSpace, &AddButton::clicked, this, &MaquetteGridManager::createHorizontalTabsSpace);

	addRightLayout->addWidget(addRightTabsSpace);

	int index{m_verticalLayouts.indexOf(layout)};

	m_verticalLayouts.insert(index + 1, tabsSpaceLayout);
	m_verticalLayouts.insert(index + 2, addRightLayout);

	m_tabsSpacesLayout->insertLayout(index + 1, tabsSpaceLayout);
	m_tabsSpacesLayout->insertLayout(index + 2, addRightLayout);

	m_saver->changeOccurred();
}

void MaquetteGridManager::removeTabsSpace(MaquetteGridTabsList* tabsSpace)
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

void MaquetteGridManager::itemClicked(QListWidgetItem* item)
{
	if (item == m_activeItem)
		return;

	m_activeItem = item;
	updateEditInputs();
}

void MaquetteGridManager::maquetteGridChanged(QListWidgetItem* item)
{
	int row{m_maquetteGridListWidget->row(item)};

	m_saver->saveIfNeccessary();
	refresh(m_maquetteGrid->maquetteGrid()[row]);
}

void MaquetteGridManager::changeMaquetteGridName(const QString& newName)
{
	if (!m_workingItem || m_workingItem->name() == "default")
		return;

	m_maquetteGridListWidget->currentItem()->setText(newName);
	m_maquetteGrid->changeMaquetteGrid(m_workingItem);

	m_saver->changeOccurred();
}

void MaquetteGridManager::tabChanged()
{
	Q_ASSERT(m_activeItem);

	m_activeItem->setText(QString("%1 (%2)").arg(m_title->text()).arg(m_url->text()));
	m_activeItem->setData(TitleRole, m_title->text());
	m_activeItem->setData(UrlRole, m_url->text());

	m_saver->changeOccurred();
}

void MaquetteGridManager::updateEditInputs()
{
	if (!m_activeItem) {
		m_url->setText("");
		m_title->setText("");
	}
	else {
		m_url->setText(m_activeItem->data(MaquetteGridManager::UrlRole).toString());
		m_title->setText(m_activeItem->data(MaquetteGridManager::TitleRole).toString());
	}

	m_url->setEnabled(m_activeItem);
	m_title->setEnabled(m_activeItem);
}

void MaquetteGridManager::refresh(MaquetteGridItem* item)
{
	if (item == m_workingItem)
		return;

	m_workingItem = item;
	m_maquetteGridName->setText(item->name());

	m_verticalLayouts.clear();
	m_tabsLists.clear();

	m_tabsSpacesWidget->deleteLater();
	m_tabsSpacesLayout->deleteLater();

	m_tabsSpacesWidget = new QWidget(m_scrollArea);
	m_tabsSpacesLayout = new QHBoxLayout(m_tabsSpacesWidget);
	m_scrollArea->setWidget(m_tabsSpacesWidget);

	m_deleteMaquetteGrid->setEnabled(m_workingItem->name() != "default");
	m_maquetteGridName->setEnabled(m_workingItem->name() != "default");

	setupTabsSpaces();
}

void MaquetteGridManager::setupMaquetteGridList()
{
	foreach (MaquetteGridItem* mitem, m_maquetteGrid->maquetteGrid()) {
		QListWidgetItem* item{new QListWidgetItem(mitem->name())};
		m_maquetteGridListWidget->addItem(item);
	}
}

void MaquetteGridManager::setupFirstTabsSpace()
{
	TabsSpaceSplitter::SavedTabsSpace firstTabsSpace{m_workingItem->tabsSpaces()[0]};
	MaquetteGridTabsList* firstList{createTabsList(firstTabsSpace)};

	QVBoxLayout* firstAddLeftLayout{new QVBoxLayout()};
	QVBoxLayout* firstTabsSpaceLayout{createNewTabsSpaceLayout(firstList)};
	QVBoxLayout* firstAddRightLayout{new QVBoxLayout()};

	AddButton* firstAddLeftButton{new AddButton(firstAddLeftLayout, m_tabsSpacesWidget)};
	AddButton* firstAddRightButton{new AddButton(firstAddRightLayout, m_tabsSpacesWidget)};

	connect(firstAddLeftButton, &AddButton::clicked, this, &MaquetteGridManager::createHorizontalTabsSpace);
	connect(firstAddRightButton, &AddButton::clicked, this, &MaquetteGridManager::createHorizontalTabsSpace);

	firstAddLeftLayout->addWidget(firstAddLeftButton);
	firstAddRightLayout->addWidget(firstAddRightButton);

	m_verticalLayouts.append(firstAddLeftLayout);
	m_verticalLayouts.append(firstTabsSpaceLayout);
	m_verticalLayouts.append(firstAddRightLayout);

	m_tabsSpacesLayout->addLayout(firstAddLeftLayout);
	m_tabsSpacesLayout->addLayout(firstTabsSpaceLayout);
	m_tabsSpacesLayout->addLayout(firstAddRightLayout);
}

void MaquetteGridManager::setupTabsSpaces()
{
	Q_ASSERT(m_workingItem->tabsSpaces().count() >= 1);

	setupFirstTabsSpace();

	int workingColumn{0};
	for (int i{1}; i < m_workingItem->tabsSpaces().count(); ++i) {
		TabsSpaceSplitter::SavedTabsSpace tabsSpace{m_workingItem->tabsSpaces()[i]};
		MaquetteGridTabsList* list{createTabsList(tabsSpace)};

		if (tabsSpace.x == workingColumn) {
			QVBoxLayout* layout{m_verticalLayouts[m_verticalLayouts.count() - 2]};
			AddButton* addBottomTabsSpace{new AddButton(layout, m_tabsSpacesWidget)};

			connect(addBottomTabsSpace, &QPushButton::clicked, this, &MaquetteGridManager::createVerticalTabsSpace);

			layout->addWidget(list);
			layout->addWidget(addBottomTabsSpace);

			list->setParentLayout(layout);
		}
		else {
			QVBoxLayout* tabsSpaceLayout{createNewTabsSpaceLayout(list)};
			QVBoxLayout* addRightLayout{new QVBoxLayout()};

			AddButton* addRightButton{new AddButton(addRightLayout, m_tabsSpacesWidget)};

			connect(addRightButton, &AddButton::clicked, this, &MaquetteGridManager::createHorizontalTabsSpace);

			addRightLayout->addWidget(addRightButton);

			m_verticalLayouts.append(tabsSpaceLayout);
			m_verticalLayouts.append(addRightLayout);

			m_tabsSpacesLayout->addLayout(tabsSpaceLayout);
			m_tabsSpacesLayout->addLayout(addRightLayout);

			workingColumn = tabsSpace.x;
		}
	}
}

void MaquetteGridManager::setupUI()
{
	int maxListWidth{ 320 };
	resize(1082, 764);

	m_editBox = new QGroupBox(this);

	m_layout = new QGridLayout(this);
	m_editLayout = new QVBoxLayout(m_editBox);

	m_maquetteGridListWidget = new QListWidget(this);
	m_maquetteGridListWidget->setMaximumWidth(maxListWidth);

	m_maquetteGridName = new QLineEdit(this);
	m_newMaquetteGrid = new QPushButton(tr("Add Maquette Grid"), this);
	m_newMaquetteGridFromCurrentSession = new QPushButton(tr("Add Maquette Grid From Current Session"), this);
	m_deleteMaquetteGrid = new QPushButton(tr("Delete MaquetteGrid"), this);

	m_maquetteGridName->setMaximumWidth(maxListWidth);
	m_newMaquetteGrid->setMaximumWidth(maxListWidth);
	m_newMaquetteGridFromCurrentSession->setMaximumWidth(maxListWidth);
	m_deleteMaquetteGrid->setMaximumWidth(maxListWidth);

	m_scrollArea = new QScrollArea(this);
	m_tabsSpacesWidget = new QWidget(m_scrollArea);
	m_tabsSpacesLayout = new QHBoxLayout(m_tabsSpacesWidget);

	m_tabsSpacesWidget->setObjectName(QLatin1String("maquettegrid-area-tabsspaces"));

	m_scrollArea->setObjectName(QLatin1String("maquettegrid-area-tabsspaces"));
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

	m_layout->addWidget(m_maquetteGridListWidget, 0, 0, 1, 1);
	m_layout->addWidget(m_maquetteGridName, 1, 0, 1, 1);
	m_layout->addWidget(m_newMaquetteGrid, 2, 0, 1, 1);
	m_layout->addWidget(m_newMaquetteGridFromCurrentSession, 3, 0, 1, 1);
	m_layout->addWidget(m_deleteMaquetteGrid, 4, 0, 1, 1);
	m_layout->addWidget(m_scrollArea, 0, 1, 1, 1);
	m_layout->addWidget(m_editBox, 1, 1, 4, 1);
}
}
