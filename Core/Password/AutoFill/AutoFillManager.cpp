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

#include "AutoFillManager.hpp"

#include <QHeaderView>
#include <QMenu>

#include <QInputDialog>
#include <QMessageBox>

#include <QClipboard>

#include <QTimer>

#include <QSqlQuery>

#include "Password/DatabasePasswordBackend.hpp"
#include "Password/DatabaseEncryptedPasswordBackend.hpp"
#include "Password/AutoFill/AutoFill.hpp"

#include "Application.hpp"

namespace Sn
{

AutoFillManager::AutoFillManager(QWidget* parent) :
	QWidget(parent),
	m_passwordManager(Application::instance()->autoFill()->passwordManager()),
	m_passwordsShown(false)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName("autofill_manager");

	setupUI();

	m_passwordsTree->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(m_buttonEditPasswordsPage, &QPushButton::clicked, this, &AutoFillManager::editPassword);
	connect(m_buttonRemovePasswordsPage, &QPushButton::clicked, this, &AutoFillManager::removePassword);
	connect(m_buttonRemoveAllPasswordsPage, &QPushButton::clicked, this, &AutoFillManager::removeAllPasswords);

	connect(m_buttonRemoveExceptionsPage, &QPushButton::clicked, this, &AutoFillManager::removeException);
	connect(m_buttonRemoveAllExceptionsPage, &QPushButton::clicked, this, &AutoFillManager::removeAllExceptions);

	connect(m_buttonShowPasswords, &QPushButton::clicked, this, &AutoFillManager::showPasswords);
	//TODO: Search line edit connection
	connect(m_buttonChangeBackend, &QPushButton::clicked, this, &AutoFillManager::changePasswordBackend);
	connect(m_buttonBackendOption, &QPushButton::clicked, this, &AutoFillManager::showBackendOption);

	connect(m_passwordManager,
			&PasswordManager::passwordBackendChanged,
			this,
			&AutoFillManager::currentPasswordBackendChanged);
	connect(m_passwordsTree, &QTreeWidget::customContextMenuRequested, this, &AutoFillManager::passwordContextMenu);

	m_currentBackendLabel->setText(QString("<b>%1</b>").arg(m_passwordManager->activeBackend()->name()));
	m_buttonBackendOption->setVisible(m_passwordManager->activeBackend()->hasSettings());

	QTimer::singleShot(0, this, &AutoFillManager::loadPasswords);
}

AutoFillManager::~AutoFillManager()
{
	// Empty
}

void AutoFillManager::showExceptions()
{
	m_tabWidget->setCurrentIndex(1);
}

void AutoFillManager::loadPasswords()
{
	m_buttonShowPasswords->setText(tr("Show Passwords"));
	m_passwordsShown = false;

	QVector<PasswordEntry> allEntries = Application::instance()->autoFill()->getAllFormData();

	m_passwordsTree->clear();

	foreach(const PasswordEntry& entry, allEntries)
	{
		QTreeWidgetItem* item{new QTreeWidgetItem(m_passwordsTree)};
		item->setText(0, entry.host);
		item->setText(1, entry.username);
		item->setText(2, "********");

		QVariant variant{};

		variant.setValue<PasswordEntry>(entry);
		item->setData(0, Qt::UserRole + 10, variant);
		m_passwordsTree->addTopLevelItem(item);
	}

	m_exceptionsTree->clear();

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.exec("SELECT server, id FROM autofill_exceptions");

	while (query.next()) {
		QTreeWidgetItem* item{new QTreeWidgetItem(m_exceptionsTree)};
		item->setText(0, query.value(0).toString());
		item->setData(0, Qt::UserRole + 10, query.value(1).toString());

		m_exceptionsTree->addTopLevelItem(item);
	}

	m_passwordsTree->sortByColumn(-1);
	m_exceptionsTree->sortByColumn(-1);
}

void AutoFillManager::changePasswordBackend()
{
	QHash<QString, PasswordBackend*> backends = m_passwordManager->availableBackends();
	QStringList items;
	int current{0};
	QHashIterator<QString, PasswordBackend*> iterator{backends};

	while (iterator.hasNext()) {
		iterator.next();

		if (iterator.value() == m_passwordManager->activeBackend())
			current = items.size();
		items << iterator.value()->name();
	}

	QString item{QInputDialog::getItem(this, tr("Change backend..."), tr("Change backend:"), items, current, false)};

	if (!item.isEmpty()) {
		PasswordBackend* backend{nullptr};
		QHashIterator<QString, PasswordBackend*> i{backends};

		while (i.hasNext()) {
			i.next();

			if (i.value()->name() == item) {
				backend = i.value();
				break;
			}
		}

		if (backend)
			m_passwordManager->switchBackend(backends.key(backend));
	}
}

void AutoFillManager::showBackendOption()
{
	PasswordBackend* backend = m_passwordManager->activeBackend();

	if (backend->hasSettings())
		backend->showSettings(this);
}

void AutoFillManager::removePassword()
{
	QTreeWidgetItem* currentItem{m_passwordsTree->currentItem()};

	if (!currentItem)
		return;

	PasswordEntry entry = currentItem->data(0, Qt::UserRole + 10).value<PasswordEntry>();

	Application::instance()->autoFill()->removeEntry(entry);

	delete currentItem;
}

void AutoFillManager::removeAllPasswords()
{
	QMessageBox::StandardButton button{QMessageBox::warning(this,
															tr("Confirmation"),
															tr("Are you sure you want to delete all passwords on your computer?"),
															QMessageBox::Yes | QMessageBox::No)};

	if (button != QMessageBox::Yes)
		return;

	Application::instance()->autoFill()->removeAllEntries();
	m_passwordsTree->clear();
}

void AutoFillManager::editPassword()
{
	QTreeWidgetItem* currentItem{m_passwordsTree->currentItem()};

	if (!currentItem)
		return;

	PasswordEntry entry = currentItem->data(0, Qt::UserRole + 10).value<PasswordEntry>();
	bool ok{false};
	QString text{QInputDialog::getText(this,
									   tr("Edit password"),
									   tr("Change password:"),
									   QLineEdit::Normal,
									   entry.password,
									   &ok)};

	if (ok && !text.isEmpty() && text != entry.password) {
		QByteArray oldPassword{"=" + PasswordManager::urlEncodePassword(entry.password)};

		entry.data.replace(oldPassword, "=" + PasswordManager::urlEncodePassword(text));
		entry.password = text;

		if (Application::instance()->autoFill()->updateEntry(entry)) {
			QVariant variant{};

			variant.setValue<PasswordEntry>(entry);
			currentItem->setData(0, Qt::UserRole + 10, variant);

			if (m_passwordsShown)
				currentItem->setText(2, text);
		}
	}
}

void AutoFillManager::showPasswords()
{
	if (m_passwordsShown) {
		for (int i{0}; i < m_passwordsTree->topLevelItemCount(); ++i) {
			QTreeWidgetItem* item{m_passwordsTree->topLevelItem(i)};

			if (!item)
				continue;

			item->setText(2, "********");
		}

		m_buttonShowPasswords->setText(tr("Show Passwords"));
		m_passwordsShown = false;

		return;
	}

	m_passwordsShown = true;

	int result{QMessageBox::question(this,
									 tr("Show Passwords"),
									 tr("Are you sure that you want to show all passwords?"),
									 QMessageBox::Yes | QMessageBox::No,
									 QMessageBox::No)};

	if (result != QMessageBox::Yes)
		return;

	for (int i{0}; i < m_passwordsTree->topLevelItemCount(); ++i) {
		QTreeWidgetItem* item{m_passwordsTree->topLevelItem(i)};

		item->setText(2, item->data(0, Qt::UserRole + 10).value<PasswordEntry>().password);
	}

	m_buttonShowPasswords->setText(tr("Hide Passwords"));
}

void AutoFillManager::copyPassword()
{
	QTreeWidgetItem* currentItem{m_passwordsTree->currentItem()};

	if (!currentItem)
		return;

	PasswordEntry entry = currentItem->data(0, Qt::UserRole + 10).value<PasswordEntry>();
	Application::clipboard()->setText(entry.password);
}

void AutoFillManager::copyUsername()
{
	QTreeWidgetItem* currentItem{m_passwordsTree->currentItem()};

	if (!currentItem)
		return;

	PasswordEntry entry = currentItem->data(0, Qt::UserRole + 10).value<PasswordEntry>();
	Application::clipboard()->setText(entry.username);
}

void AutoFillManager::removeException()
{
	QTreeWidgetItem* currentItem{m_exceptionsTree->currentItem()};

	if (!currentItem)
		return;

	const QString id{currentItem->data(0, Qt::UserRole + 10).toString()};
	
	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("DELETE FROM autofill_exceptions WHERE id=?");
	query.addBindValue(id);
	query.exec();

	delete currentItem;
}

void AutoFillManager::removeAllExceptions()
{
	QSqlQuery query{SqlDatabase::instance()->database()};
	query.exec("DELETE FROM autofill_exceptions");

	m_exceptionsTree->clear();
}

void AutoFillManager::importPasswords()
{
	QMessageBox::critical(nullptr, tr("No"), tr("You can't import password yet"));
}

void AutoFillManager::exportPasswords()
{
	QMessageBox::critical(nullptr, tr("No"), tr("You can't export password yet"));
}

void AutoFillManager::sImportPassword()
{
	// Empty
}

void AutoFillManager::sExportPassword()
{
	// Empty
}

void AutoFillManager::currentPasswordBackendChanged()
{
	m_currentBackendLabel->setText(QString("<b>%1</b>").arg(m_passwordManager->activeBackend()->name()));
	m_buttonBackendOption->setVisible(m_passwordManager->activeBackend()->hasSettings());

	QTimer::singleShot(0, this, &AutoFillManager::loadPasswords);
}

void AutoFillManager::passwordContextMenu(const QPoint& pos)
{
	QMenu* menu{new QMenu(this)};

	menu->setAttribute(Qt::WA_DeleteOnClose);
	menu->addAction(tr("Copy Username"), this, &AutoFillManager::copyUsername);
	menu->addAction(tr("Copy Password"), this, &AutoFillManager::copyPassword);
	menu->addSeparator();
	menu->addAction(tr("Edit Password"), this, &AutoFillManager::editPassword);

	menu->popup(m_passwordsTree->viewport()->mapToGlobal(pos));
}

void AutoFillManager::setupUI()
{
	m_layout = new QVBoxLayout(this);
	m_storePasswordsLayout = new QHBoxLayout();

	m_tabWidget = new QTabWidget(this);

	m_passwordsPage = new QWidget(this);
	m_exceptionsPage = new QWidget(this);

	m_passwordsPageLayout = new QGridLayout(m_passwordsPage);
	m_editAndRemoveAllLayout = new QVBoxLayout();
	m_importExportLayout = new QHBoxLayout();
	m_exceptionsLayout = new QHBoxLayout(m_exceptionsPage);
	m_removeAllLayout = new QVBoxLayout();

	m_descPasswordsAreStored = new QLabel(tr("Passwords are stored in: "), this);
	m_currentBackendLabel = new QLabel(this);
	m_buttonChangeBackend = new QPushButton(tr("Change backend"), this);
	m_buttonBackendOption = new QPushButton(tr("Backend options"), this);

	m_storePasswordsSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_storePasswordsLayout->addWidget(m_descPasswordsAreStored);
	m_storePasswordsLayout->addWidget(m_currentBackendLabel);
	m_storePasswordsLayout->addWidget(m_buttonChangeBackend);
	m_storePasswordsLayout->addWidget(m_buttonBackendOption);
	m_storePasswordsLayout->addItem(m_storePasswordsSpacer);

	m_searchLineEdit = new QLineEdit(m_passwordsPage);
	m_searchLineEdit->setPlaceholderText(tr("Search (not available yet)"));

	m_passwordsTree = new QTreeWidget(m_passwordsPage);
	m_passwordsTree->setIndentation(0);
	m_passwordsTree->setSortingEnabled(true);
	m_passwordsTree->header()->setMinimumSectionSize(100);

	QTreeWidgetItem* passwordsTreeHeader = m_passwordsTree->headerItem();
	passwordsTreeHeader->setText(0, tr("Server"));
	passwordsTreeHeader->setText(1, tr("Username"));
	passwordsTreeHeader->setText(2, tr("Passwords"));

	m_buttonEditPasswordsPage = new QPushButton(tr("Edit"), m_passwordsPage);
	m_buttonRemovePasswordsPage = new QPushButton(tr("Remove"), m_passwordsPage);
	m_buttonRemoveAllPasswordsPage = new QPushButton(tr("Remove all"), m_passwordsPage);
	m_buttonImportExport = new QPushButton(tr("Import and Export"), m_passwordsPage);
	m_importExportLabel = new QLabel(m_passwordsPage);
	m_buttonShowPasswords = new QPushButton(tr("Show Passwords"), m_passwordsPage);

	m_importAndShowSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_editRemoveAllSpacer = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding);

	m_editAndRemoveAllLayout->addWidget(m_buttonEditPasswordsPage);
	m_editAndRemoveAllLayout->addWidget(m_buttonRemovePasswordsPage);
	m_editAndRemoveAllLayout->addWidget(m_buttonRemoveAllPasswordsPage);
	m_editAndRemoveAllLayout->addItem(m_editRemoveAllSpacer);

	m_importExportLayout->addWidget(m_buttonImportExport);
	m_importExportLayout->addWidget(m_importExportLabel);
	m_importExportLayout->addItem(m_importAndShowSpacer);
	m_importExportLayout->addWidget(m_buttonShowPasswords);

	QMenu* menu{new QMenu(this)};
	menu->addAction(tr("Import Passwords from File..."), this, &AutoFillManager::importPasswords);
	menu->addAction(tr("Export Passwords to File..."), this, &AutoFillManager::exportPasswords);

	m_buttonImportExport->setMenu(menu);
	m_passwordsPageLayout->addWidget(m_searchLineEdit, 0, 0, 1, 1);
	m_passwordsPageLayout->addWidget(m_passwordsTree, 1, 0, 1, 1);
	m_passwordsPageLayout->addLayout(m_editAndRemoveAllLayout, 1, 1, 1, 1);
	m_passwordsPageLayout->addLayout(m_importExportLayout, 2, 0, 1, 2);

	m_exceptionsTree = new QTreeWidget(m_exceptionsPage);

	QTreeWidgetItem* exceptionsTreeHeader = m_exceptionsTree->headerItem();
	exceptionsTreeHeader->setText(0, tr("Server"));

	m_buttonRemoveExceptionsPage = new QPushButton(tr("Remove"), m_exceptionsPage);
	m_buttonRemoveAllExceptionsPage = new QPushButton(tr("Remove All"), m_exceptionsPage);

	m_removeAllSpacer = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding);

	m_removeAllLayout->addWidget(m_buttonRemoveExceptionsPage);
	m_removeAllLayout->addWidget(m_buttonRemoveAllExceptionsPage);
	m_removeAllLayout->addItem(m_removeAllSpacer);

	m_exceptionsLayout->addWidget(m_exceptionsTree);
	m_exceptionsLayout->addLayout(m_removeAllLayout);

	m_tabWidget->addTab(m_passwordsPage, tr("Passwords"));
	m_tabWidget->addTab(m_exceptionsPage, tr("Exceptions"));

	m_layout->addLayout(m_storePasswordsLayout);
	m_layout->addWidget(m_tabWidget);

	if (isRightToLeft()) {
		m_passwordsTree->headerItem()->setTextAlignment(0, Qt::AlignRight | Qt::AlignVCenter);
		m_passwordsTree->headerItem()->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
		m_passwordsTree->headerItem()->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
		m_passwordsTree->setLayoutDirection(Qt::LeftToRight);

		m_exceptionsTree->setLayoutDirection(Qt::LeftToRight);
	}
}

}