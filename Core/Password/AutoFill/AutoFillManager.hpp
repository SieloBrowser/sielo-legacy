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
#ifndef SIELO_BROWSER_AUTOFILLMANAGER_HPP
#define SIELO_BROWSER_AUTOFILLMANAGER_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTabWidget>
#include <QTreeWidget>
#include <QSpacerItem>

#include "Password/PasswordManager.hpp"

namespace Sn {

class SIELO_SHAREDLIB AutoFillManager: public QWidget {
Q_OBJECT

public:
	AutoFillManager(QWidget* parent = nullptr);
	~AutoFillManager();

	void showExceptions();

private slots:
	void loadPasswords();
	void changePasswordBackend();
	void showBackendOption();

	void removePassword();
	void removeAllPasswords();
	void editPassword();
	void showPasswords();

	void copyPassword();
	void copyUsername();

	void removeException();
	void removeAllExceptions();

	void importPasswords();
	void exportPasswords();

	void sImportPassword();
	void sExportPassword();

	void currentPasswordBackendChanged();
	void passwordContextMenu(const QPoint& pos);

private:
	void setupUI();

	QVBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_storePasswordsLayout{nullptr};

	QGridLayout* m_passwordsPageLayout{nullptr};
	QVBoxLayout* m_editAndRemoveAllLayout{nullptr};
	QHBoxLayout* m_importExportLayout{nullptr};

	QHBoxLayout* m_exceptionsLayout{nullptr};
	QVBoxLayout* m_removeAllLayout{nullptr};

	QTabWidget* m_tabWidget{nullptr};

	QWidget* m_passwordsPage{nullptr};
	QWidget* m_exceptionsPage{nullptr};

	QLabel* m_descPasswordsAreStored{nullptr};
	QLabel* m_currentBackendLabel{nullptr};
	QPushButton* m_buttonChangeBackend{nullptr};
	QPushButton* m_buttonBackendOption{nullptr};

	QLineEdit* m_searchLineEdit{nullptr};
	QTreeWidget* m_passwordsTree{nullptr};
	QPushButton* m_buttonEditPasswordsPage{nullptr};
	QPushButton* m_buttonRemovePasswordsPage{nullptr};
	QPushButton* m_buttonRemoveAllPasswordsPage{nullptr};
	QPushButton* m_buttonImportExport{nullptr};
	QLabel* m_importExportLabel{nullptr};
	QPushButton* m_buttonShowPasswords{nullptr};

	QTreeWidget* m_exceptionsTree{nullptr};
	QPushButton* m_buttonRemoveExceptionsPage{nullptr};
	QPushButton* m_buttonRemoveAllExceptionsPage{nullptr};

	QSpacerItem* m_storePasswordsSpacer{nullptr};
	QSpacerItem* m_importAndShowSpacer{nullptr};
	QSpacerItem* m_editRemoveAllSpacer{nullptr};
	QSpacerItem* m_removeAllSpacer{nullptr};

	PasswordManager* m_passwordManager{nullptr};
	QString m_fileName{};
	bool m_passwordsShown{false};
};

}

#endif //SIELO_BROWSER_AUTOFILLMANAGER_HPP
