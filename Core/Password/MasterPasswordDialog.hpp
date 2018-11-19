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
#ifndef SIELO_BROWSER_MASTERPASSWORD_HPP
#define SIELO_BROWSER_MASTERPASSWORD_HPP

#include "SharedDefines.hpp"

#include <QDialog>
#include <QWidget>

#include <QVBoxLayout>
#include <QFormLayout>
#include <QStackedWidget>

#include <QCommandLinkButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>

#include "Password/PasswordManager.hpp"

namespace Sn {
class DatabaseEncryptedPasswordBackend;

class SIELO_SHAREDLIB MasterPasswordDialog: public QDialog {
Q_OBJECT

public:
	MasterPasswordDialog(DatabaseEncryptedPasswordBackend* backend, QWidget* parent = nullptr);
	~MasterPasswordDialog();

	void delayedExec();

public slots:
	void accept();
	void reject();

	void showSettingsPage();
	void showSetMasterPage();

	void clearMasterPasswordAddConvert(bool forcedAskPass = true);
	bool samePasswordEntry(const PasswordEntry& entry1, const PasswordEntry& entry2);

private:
	void setupUI();

	QVBoxLayout* m_layout{nullptr};
	QVBoxLayout* m_settingsLayout{nullptr};
	QVBoxLayout* m_setMasterLayout{nullptr};
	QFormLayout* m_formLayout{nullptr};

	QStackedWidget* m_pages{nullptr};

	QWidget* m_settingsPage{nullptr};
	QWidget* m_setMasterPage{nullptr};

	QCommandLinkButton* m_setMasterPasswordButton{nullptr};
	QCommandLinkButton* m_clearMasterPasswordButton{nullptr};
	QDialogButtonBox* m_settingsButtonBox{nullptr};

	QLabel* m_descLabel{nullptr};
	QLabel* m_currentPasswordLabel{nullptr};
	QLineEdit* m_currentPassword{nullptr};
	QLabel* m_newPasswordLabel{nullptr};
	QLineEdit* m_newPassword{nullptr};
	QLabel* m_confirmPasswordLabel{nullptr};
	QLineEdit* m_confirmPassword{nullptr};
	QLabel* m_noteLabel{nullptr};
	QDialogButtonBox* m_setMasterButtonBox{nullptr};

	DatabaseEncryptedPasswordBackend* m_backend{nullptr};
};

class SIELO_SHAREDLIB AskMasterPasswordDialog: public QDialog {
Q_OBJECT

public:
	AskMasterPasswordDialog(DatabaseEncryptedPasswordBackend* backend, QWidget* parent = nullptr);
	~AskMasterPasswordDialog();

private slots:
	void verifyPassword();

private:
	void setupUI();

	QVBoxLayout* m_layout{nullptr};
	QLabel* m_desc{nullptr};
	QLineEdit* m_password{nullptr};
	QLabel* m_warningLabel{nullptr};
	QDialogButtonBox* m_buttonBox{nullptr};

	DatabaseEncryptedPasswordBackend* m_backend{nullptr};
};

}


#endif //SIELO_BROWSER_MASTERPASSWORD_HPP
