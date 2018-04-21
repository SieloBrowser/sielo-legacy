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

#include "MasterPasswordDialog.hpp"

#include <QTimer>

#include <QVector>

#include <QPalette>

#include <QMessageBox>

#include "Password/DatabaseEncryptedPasswordBackend.hpp"
#include "Password/AutoFill/AutoFill.hpp"

#include "Application.hpp"

namespace Sn {

MasterPasswordDialog::MasterPasswordDialog(DatabaseEncryptedPasswordBackend* backend, QWidget* parent) :
	QDialog(parent),
	m_backend(backend)
{
	setAttribute(Qt::WA_DeleteOnClose);

	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	setWindowFlags(flags);
	setWindowIcon(icon);

	setupUI();

	m_currentPasswordLabel->setVisible(m_backend->isMasterPasswordSetted());
	m_currentPassword->setVisible(m_backend->isMasterPasswordSetted());

	connect(m_setMasterPasswordButton, &QCommandLinkButton::clicked, this, &MasterPasswordDialog::showSetMasterPage);
	connect(m_clearMasterPasswordButton,
			&QCommandLinkButton::clicked,
			this,
			&MasterPasswordDialog::clearMasterPasswordAddConvert);
	connect(m_settingsButtonBox, &QDialogButtonBox::rejected, this, &MasterPasswordDialog::reject);
	connect(m_setMasterButtonBox, &QDialogButtonBox::rejected, this, &MasterPasswordDialog::reject);
	connect(m_setMasterButtonBox, &QDialogButtonBox::accepted, this, &MasterPasswordDialog::accept);
}

MasterPasswordDialog::~MasterPasswordDialog()
{
	// Empty
}

void MasterPasswordDialog::delayedExec()
{
	QTimer::singleShot(0, this, &MasterPasswordDialog::exec);
}

void MasterPasswordDialog::accept()
{
	if (m_pages->currentIndex() != 1) {
		QDialog::accept();
		return;
	}

	QByteArray currentPasswordField{PasswordManager::passwordToHash(m_currentPassword->text())};

	if (m_backend->isMasterPasswordSetted() && !m_backend->isPasswordVerified(currentPasswordField)) {
		QMessageBox::critical(this, tr("Error"), tr("You entered a wrong password!"));
		return;
	}

	if (m_newPassword->text() != m_confirmPassword->text()) {
		QMessageBox::critical(this, tr("Error"), tr("New/Confirm password fields do not match!"));
		return;
	}

	if (m_newPassword->text().isEmpty()) {
		if (!m_backend->isMasterPasswordSetted())
			return;
		clearMasterPasswordAddConvert(false);
	}
	else {
		QByteArray newPasswordField{PasswordManager::passwordToHash(m_newPassword->text())};

		if (m_backend->masterPassword() != newPasswordField)
			m_backend->tryToChangeMasterPassword(newPasswordField);
	}

	QDialog::accept();

}

void MasterPasswordDialog::reject()
{
	QDialog::reject();

	if (m_backend->isActive() && !m_backend->isMasterPasswordSetted()) {
		QMessageBox::warning(this,
							 tr("Warning!"),
							 tr("This backend needs a master password to be set! Sielo just switches to its default backend (plain text)"));

		Application::instance()->autoFill()->passwordManager()->switchBackend("database");
		return;
	}

}

void MasterPasswordDialog::showSettingsPage()
{
	m_pages->setCurrentIndex(0);
	delayedExec();
}

void MasterPasswordDialog::showSetMasterPage()
{
	m_pages->setCurrentIndex(1);
}

void MasterPasswordDialog::clearMasterPasswordAddConvert(bool forcedAskPass)
{
	if (QMessageBox::warning(this,
							 tr("Warning!"),
							 tr("Are you sure you want to clear master password ? All data will be decrypt!"),
							 QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
		reject();
		return;
	}

	if (forcedAskPass)
		m_backend->setAskMasterPasswordState(true);

	if (m_backend->hasPermission()) {
		QVector<PasswordEntry> list = m_backend->getAllEntries();
		PasswordBackend* databaseBackend
			{Application::instance()->autoFill()->passwordManager()->availableBackends().value("database")};

		if (!databaseBackend)
			return;

		QVector<PasswordEntry> databaseList{databaseBackend->getAllEntries()};
		bool allDataMoved{true};

			foreach (const PasswordEntry& entry, list) {
				bool sameEntry = false;

					foreach (const PasswordEntry& dbEntry, databaseList) {
						sameEntry = samePasswordEntry(dbEntry, entry);

						if (sameEntry) {
							allDataMoved = false;
							break;
						}
					}

				if (!sameEntry) {
					databaseBackend->addEntry(entry);
					m_backend->removeEntry(entry);
				}
			}

		if (allDataMoved) {
			m_backend->removeAll();
			m_backend->removeMasterPassword();
			m_backend->setAskMasterPasswordState(false);
		}
		else {
			QMessageBox::critical(this,
								  tr("Error"),
								  tr("Some data has not been decrypted. The master password was not cleared"));
		}

		Application::instance()->autoFill()->passwordManager()->switchBackend("database");
	}

	reject();
}

bool MasterPasswordDialog::samePasswordEntry(const PasswordEntry& entry1, const PasswordEntry& entry2)
{
	if ((entry1.data.isEmpty() || entry2.data.isEmpty()) && entry1.host == entry2.host)
		return true;

	if (entry1.host != entry2.host || entry1.username != entry2.username)
		return false;

	return true;
}

void MasterPasswordDialog::setupUI()
{
	resize(436, 230);
	m_layout = new QVBoxLayout(this);

	m_pages = new QStackedWidget(this);

	m_settingsPage = new QWidget(this);
	m_setMasterPage = new QWidget(this);

	m_settingsLayout = new QVBoxLayout(m_settingsPage);
	m_setMasterLayout = new QVBoxLayout(m_setMasterPage);
	m_formLayout = new QFormLayout();

	m_setMasterPasswordButton = new QCommandLinkButton(tr("This backend does not work without a master password."),
													   tr("Set/Change Master Password..."),
													   m_settingsPage);
	m_clearMasterPasswordButton = new QCommandLinkButton(tr(
		"This option clears the master password and moves all encrypted data to the \"DataBase (Plain Text)\" backend, and switches to it."),
														 tr("Clear Master Password..."),
														 m_settingsPage);
	m_settingsButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, m_settingsPage);

	m_settingsLayout->addWidget(m_setMasterPasswordButton);
	m_settingsLayout->addWidget(m_clearMasterPasswordButton);
	m_settingsLayout->addWidget(m_settingsButtonBox);

	m_descLabel = new QLabel(tr(
		"The Master Password is used to protect site passwords and form data. If you set a Master Password you will be asked to enter it once per session."),
							 m_setMasterPage);
	m_descLabel->setWordWrap(true);
	m_currentPasswordLabel = new QLabel(tr("Current Password"), m_setMasterPage);
	m_currentPassword = new QLineEdit();
	m_currentPassword->setEchoMode(QLineEdit::Password);
	m_newPasswordLabel = new QLabel(tr("New Password"), m_setMasterPage);
	m_newPassword = new QLineEdit();
	m_newPassword->setEchoMode(QLineEdit::Password);
	m_confirmPasswordLabel = new QLabel(tr("Confirm Password"), m_setMasterPage);
	m_confirmPassword = new QLineEdit();
	m_confirmPassword->setEchoMode(QLineEdit::Password);
	m_noteLabel = new QLabel(tr("<b>Note:</b> The Master Password is not resettable. Do not forget it, please."),
							 m_setMasterPage);
	m_setMasterButtonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, m_setMasterPage);

	m_formLayout->addRow(m_currentPasswordLabel, m_currentPassword);
	m_formLayout->addRow(m_newPasswordLabel, m_newPassword);
	m_formLayout->addRow(m_confirmPasswordLabel, m_confirmPassword);

	m_setMasterLayout->addWidget(m_descLabel);
	m_setMasterLayout->addLayout(m_formLayout);
	m_setMasterLayout->addWidget(m_noteLabel);
	m_setMasterLayout->addWidget(m_setMasterButtonBox);

	m_pages->addWidget(m_settingsPage);
	m_pages->addWidget(m_setMasterPage);
	m_pages->setCurrentIndex(0);

	m_layout->addWidget(m_pages);

}

AskMasterPasswordDialog::AskMasterPasswordDialog(DatabaseEncryptedPasswordBackend* backend, QWidget* parent) :
	QDialog(parent),
	m_backend(backend)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);
	setWindowModality(Qt::ApplicationModal);

	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	setWindowFlags(flags);
	setWindowIcon(icon);

	setupUI();

	connect(m_password, &QLineEdit::returnPressed, this, &AskMasterPasswordDialog::verifyPassword);
	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &AskMasterPasswordDialog::verifyPassword);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &AskMasterPasswordDialog::reject);
}

AskMasterPasswordDialog::~AskMasterPasswordDialog()
{
	// Empty
}

void AskMasterPasswordDialog::verifyPassword()
{
	QByteArray enteredPassword{PasswordManager::passwordToHash(m_password->text())};

	if (!m_backend->isPasswordVerified(enteredPassword)) {
		m_backend->setAskMasterPasswordState(true);
		m_warningLabel->show();
		m_password->clear();
		m_password->setFocus();
	}
	else {
		m_backend->setAskMasterPasswordState(false);
		accept();
	}
}

void AskMasterPasswordDialog::setupUI()
{
	m_layout = new QVBoxLayout(this);

	m_desc = new QLabel(tr("Permission is required, please enter Master Password:"));
	m_password = new QLineEdit(this);
	m_password->setEchoMode(QLineEdit::Password);
	m_warningLabel = new QLabel(tr("Entered password is wrong"), this);

	QPalette palette{m_warningLabel->palette()};
	palette.setBrush(QPalette::WindowText, Qt::red);

	m_warningLabel->setPalette(palette);
	m_warningLabel->hide();

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);

	m_layout->addWidget(m_desc);
	m_layout->addWidget(m_password);
	m_layout->addWidget(m_warningLabel);
	m_layout->addWidget(m_buttonBox);
}

}