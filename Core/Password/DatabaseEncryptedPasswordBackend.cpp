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

#include "DatabaseEncryptedPasswordBackend.hpp"

#include <QMessageBox>

#include "Password/PasswordManager.hpp"
#include "Password/MasterPasswordDialog.hpp"
#include "Password/AutoFill/AutoFill.hpp"

#include "Utils/AesInterface.hpp"
#include "Database/SqlDatabase.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

#include <ndb/query.hpp>
#include <ndb/function.hpp>
#include <ndb/preprocessor.hpp>

constexpr auto& autofill_encrypted = ndb::models::password.autofill_encrypted;

const QString INTERNAL_SERVER_ID = QLatin1String("sielo.internal");

namespace Sn {

DatabaseEncryptedPasswordBackend::DatabaseEncryptedPasswordBackend() :
		PasswordBackend(),
		m_stateOfMasterPassword(UnknownState),
		m_askPasswordDialogVisible(false),
		m_askMasterPassword(false)
{
	// Empty
}

DatabaseEncryptedPasswordBackend::~DatabaseEncryptedPasswordBackend()
{
	// Empty
}

QVector<PasswordEntry> DatabaseEncryptedPasswordBackend::getEntries(const QUrl& url)
{
	QVector<PasswordEntry> list;
	AesInterface aesDecryptor{};
	const QString host{PasswordManager::createHost(url)};


	if (hasPermission()) {
		for (auto& data : ndb::oquery<dbs::password>() << (autofill_encrypted.server == host)) {
			PasswordEntry entry{ data };
			if (decryptPasswordEntry(entry, &aesDecryptor))
				list.append(entry);
		}
	}

	return list;
}

QVector<PasswordEntry> DatabaseEncryptedPasswordBackend::getAllEntries()
{
	QVector<PasswordEntry> list;
	AesInterface aesDecryptor{};

	if (hasPermission()) {
		for (auto& data : ndb::oquery<dbs::password>() << autofill_encrypted) {
			if (data.server == INTERNAL_SERVER_ID)
				continue;

			PasswordEntry entry{ data };
			if (decryptPasswordEntry(entry, &aesDecryptor))
				list.append(entry);
		}
	}

	return list;
}

void DatabaseEncryptedPasswordBackend::setActive(bool active)
{
	if (active == isActive())
		return;

	PasswordBackend::setActive(active);

	if (active) {
		setAskMasterPasswordState(isMasterPasswordSetted());
		if (!isMasterPasswordSetted())
			showMasterPasswordDialog();
	}
	else {
		m_masterPassword.clear();
		setAskMasterPasswordState(isMasterPasswordSetted());
	}
}

void DatabaseEncryptedPasswordBackend::addEntry(const PasswordEntry& entry)
{
	if (entry.data.isEmpty()) {
		auto data = ndb::query<dbs::password>() << ((autofill_encrypted.username_encrypted) << (autofill_encrypted.server == entry.host));

		if (data.has_result())
			return;
	}

	PasswordEntry encryptedEntry = entry;
	AesInterface aesEncryptor{};

	if (hasPermission() && encryptPasswordEntry(encryptedEntry, &aesEncryptor)) {
		ndb::query<dbs::password>() + (autofill_encrypted.server = encryptedEntry.host,
									   autofill_encrypted.data_encrypted = QString::fromUtf8(encryptedEntry.data),
									   autofill_encrypted.password_encrypted = encryptedEntry.password,
									   autofill_encrypted.username_encrypted = encryptedEntry.username,
									   autofill_encrypted.last_used = ndb::now());
	}
}

bool DatabaseEncryptedPasswordBackend::updateEntry(const PasswordEntry& entry)
{
	PasswordEntry encryptedEntry = entry;
	AesInterface aesEncryptor{};

	if (hasPermission() && encryptPasswordEntry(encryptedEntry, &aesEncryptor)) {
		if (entry.data.isEmpty()) {
			ndb::query<dbs::password>() >> ((autofill_encrypted.data_encrypted = QString::fromUtf8(encryptedEntry.data),
					autofill_encrypted.username_encrypted = encryptedEntry.username)
					<< (autofill_encrypted.server == encryptedEntry.host));
		}
		else {
			ndb::query<dbs::password>() >> ((autofill_encrypted.data_encrypted = QString::fromUtf8(encryptedEntry.data),
											 autofill_encrypted.username_encrypted = encryptedEntry.username,
											 autofill_encrypted.password_encrypted = encryptedEntry.password)
					<< (autofill_encrypted.id == encryptedEntry.id.toLongLong()));
		}

		//return query.exec();
		return true;
	}

	return false;
}

void DatabaseEncryptedPasswordBackend::updateLastUsed(PasswordEntry& entry)
{
	ndb::query<dbs::password>() >> ((autofill_encrypted.last_used = ndb::now())
			<< (autofill_encrypted.id == entry.id.toLongLong()));
}

void DatabaseEncryptedPasswordBackend::removeEntry(const PasswordEntry& entry)
{
	if (!hasPermission())
		return;

	ndb::query<dbs::password>() - (autofill_encrypted.id == entry.id.toLongLong());

	m_stateOfMasterPassword = UnknownState;

	if (someDataFromDatabase().isEmpty())
		updateSampleData(m_masterPassword);
}

void DatabaseEncryptedPasswordBackend::removeAll()
{
	if (!hasPermission())
		return;

	ndb::clear<dbs::password>(autofill_encrypted);

	m_stateOfMasterPassword = PasswordIsSetted;

	updateSampleData(m_masterPassword);
}

QString DatabaseEncryptedPasswordBackend::name() const
{
	return AutoFill::tr("Database (encrypted)");
}

void DatabaseEncryptedPasswordBackend::showSettings(QWidget* parent)
{
	MasterPasswordDialog* masterPasswordDialog{new MasterPasswordDialog(this, parent)};
	masterPasswordDialog->showSettingsPage();
}

bool DatabaseEncryptedPasswordBackend::isMasterPasswordSetted()
{
	if (m_stateOfMasterPassword == UnknownState)
		m_stateOfMasterPassword = someDataFromDatabase().isEmpty() ? PasswordIsNotSetted : PasswordIsSetted;

	return m_stateOfMasterPassword == PasswordIsSetted;
}

bool DatabaseEncryptedPasswordBackend::hasPermission()
{
	if (!m_askMasterPassword)
		return true;

	if (m_askPasswordDialogVisible)
		return false;

	m_askPasswordDialogVisible = true;

	AskMasterPasswordDialog* askMasterPasswordDialog{new AskMasterPasswordDialog(this)};

	bool authorized = askMasterPasswordDialog->exec() == QDialog::Accepted;

	m_askPasswordDialogVisible = false;

	return authorized;
}

bool DatabaseEncryptedPasswordBackend::isPasswordVerified(const QByteArray& password)
{
	if (password.isEmpty())
		return false;

	if (m_masterPassword == password)
		return true;
	else if (!m_masterPassword.isEmpty())
		return false;
	else {
		AesInterface aesDecryptor{};

		aesDecryptor.decrypt(someDataFromDatabase(), password);

		if (aesDecryptor.isOk()) {
			m_masterPassword = password;
			return true;
		}
	}

	return false;
}

bool DatabaseEncryptedPasswordBackend::decryptPasswordEntry(PasswordEntry& entry, AesInterface* aesInterface)
{
	entry.username = QString::fromUtf8(aesInterface->decrypt(entry.username.toUtf8(), m_masterPassword));
	entry.password = QString::fromUtf8(aesInterface->decrypt(entry.password.toUtf8(), m_masterPassword));
	entry.data = aesInterface->decrypt(entry.data, m_masterPassword);

	return aesInterface->isOk();
}

bool DatabaseEncryptedPasswordBackend::encryptPasswordEntry(PasswordEntry& entry, AesInterface* aesInterface)
{
	entry.username = QString::fromUtf8(aesInterface->encrypt(entry.username.toUtf8(), m_masterPassword));
	entry.password = QString::fromUtf8(aesInterface->encrypt(entry.password.toUtf8(), m_masterPassword));
	entry.data = aesInterface->encrypt(entry.data, m_masterPassword);

	return aesInterface->isOk();
}

void DatabaseEncryptedPasswordBackend::tryToChangeMasterPassword(const QByteArray& newPassword)
{
	if (m_masterPassword == newPassword)
		return;

	if (newPassword.isEmpty()) {
		removeMasterPassword();
		return;
	}

	encryptDatabaseTableOnFly(m_masterPassword, newPassword);

	m_masterPassword = newPassword;

	updateSampleData(m_masterPassword);
}

void DatabaseEncryptedPasswordBackend::removeMasterPassword()
{
	if (!m_masterPassword.isEmpty()) {
		encryptDatabaseTableOnFly(m_masterPassword, QByteArray());

		m_masterPassword.clear();
		updateSampleData(QByteArray());
	}
}

void DatabaseEncryptedPasswordBackend::setAskMasterPasswordState(bool ask)
{
	m_askMasterPassword = ask;
}

void DatabaseEncryptedPasswordBackend::encryptDatabaseTableOnFly(const QByteArray& decryptorPassword,
																 const QByteArray& encryptorPassword)
{
	if (encryptorPassword == decryptorPassword)
		return;

	AesInterface encryptor;
	AesInterface decryptor;

	for (auto& qdata : ndb::oquery<dbs::password>() << autofill_encrypted) {
		if (qdata.server == INTERNAL_SERVER_ID)
			continue;

		int id{static_cast<int>(qdata.id)};
		QByteArray data = qdata.data_encrypted.toUtf8();
		QByteArray password = qdata.password_encrypted.toUtf8();
		QByteArray username = qdata.username_encrypted.toUtf8();

		if (!decryptorPassword.isEmpty()) {
			data = decryptor.decrypt(data, decryptorPassword);
			password = decryptor.decrypt(password, decryptorPassword);
			username = decryptor.decrypt(username, decryptorPassword);
		}

		if (!encryptorPassword.isEmpty()) {
			data = encryptor.encrypt(data, encryptorPassword);
			password = encryptor.encrypt(password, encryptorPassword);
			username = encryptor.encrypt(username, encryptorPassword);
		}

		ndb::query<dbs::password>() >> ((autofill_encrypted.data_encrypted = QString::fromUtf8(data),
										 autofill_encrypted.password_encrypted = QString::fromUtf8(password),
										 autofill_encrypted.username_encrypted = QString::fromUtf8(username))
				<< (autofill_encrypted.id == id));
	}
}

void DatabaseEncryptedPasswordBackend::updateSampleData(const QByteArray& password)
{
	auto data = ndb::query<dbs::password>()
			<< ((autofill_encrypted.id) << (autofill_encrypted.server == INTERNAL_SERVER_ID));

		if (!password.isEmpty()) {
			AesInterface aesInterface{};

			m_someDataStoredOnDatabase = aesInterface.encrypt(AesInterface::createRandomData(16), password);

			if (data.has_result()) {
				ndb::query<dbs::password>()
					>> ((autofill_encrypted.password_encrypted = QString::fromUtf8(
						m_someDataStoredOnDatabase))
						<< (autofill_encrypted.server == INTERNAL_SERVER_ID));
			}
			else {
				ndb::query<dbs::password>() +
					(autofill_encrypted.data_encrypted = "",
						autofill_encrypted.password_encrypted = QString::fromUtf8(m_someDataStoredOnDatabase),
						autofill_encrypted.username_encrypted = "",
						autofill_encrypted.server = INTERNAL_SERVER_ID,
						autofill_encrypted.last_used = 0);
			}

			m_stateOfMasterPassword = PasswordIsSetted;
		}
		else if (data.has_result()) {
			ndb::query<dbs::password>() - (autofill_encrypted.server == INTERNAL_SERVER_ID);

			m_stateOfMasterPassword = PasswordIsNotSetted;
			m_someDataStoredOnDatabase.clear();
		}
}

void DatabaseEncryptedPasswordBackend::showMasterPasswordDialog()
{
	MasterPasswordDialog* masterPasswordDialog{new MasterPasswordDialog(this, Application::instance()->getWindow())};

	masterPasswordDialog->showSetMasterPage();
	masterPasswordDialog->delayedExec();
}

QByteArray DatabaseEncryptedPasswordBackend::someDataFromDatabase()
{
	if (m_stateOfMasterPassword != UnknownState && !m_someDataStoredOnDatabase.isEmpty())
		return m_someDataStoredOnDatabase;

	QByteArray someData{};
	auto query = ndb::query<dbs::password>() << (autofill_encrypted.password_encrypted,
			autofill_encrypted.data_encrypted,
			autofill_encrypted.username_encrypted);

	if (query.has_result()) {
		int i{0};
		int j{0};

		while (someData.isEmpty()) {
			if (i > 2) {
				if (query.size() > j) {
					i = 0;
					++j;
					continue;
				}
				else
					break;
			}

			someData = query[j][i].get<QString>().toUtf8();
			++i;
		}
	}

	m_someDataStoredOnDatabase = someData;
	return m_someDataStoredOnDatabase;
/*
	QSqlQuery query{};

	query.exec("SELECT password_encrypted, data_encrypted, username_encrypted FROM autofill_encrypted");

	if (query.next()) {
		int i{0};

		while (someData.isEmpty()) {
			if (i > 2) {
				if (query.next()) {
					i = 0;
					continue;
				}
				else
					break;
			}

			someData = query.value(i).toByteArray();
			++i;
		}
	}

	m_someDataStoredOnDatabase = someData;
	return m_someDataStoredOnDatabase;
*/
}

}