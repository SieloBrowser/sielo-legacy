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

#include <QSqlQuery>

#include "Password/PasswordManager.hpp"
#include "Password/MasterPasswordDialog.hpp"
#include "Password/AutoFill/AutoFill.hpp"

#include "Utils/AesInterface.hpp"
#include "Database/SqlDatabase.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

const QString INTERNAL_SERVER_ID = QLatin1String("sielo.internal");

namespace Sn
{

DatabaseEncryptedPasswordBackend::DatabaseEncryptedPasswordBackend() :
	PasswordBackend(),
	m_stateOfMasterPassword(UnknownState),
	m_askPasswordDialogVisible(false),
	m_askMasterPassword(false)
{
	QSqlDatabase db{SqlDatabase::instance()->database()};

	if (!db.tables().contains(QLatin1String("autofill_encrypted"))) {
		db.exec("CREATE TABLE autofill_encrypted (data_encrypted TEXT, id INTEGER PRIMARY KEY,"
				"password_encrypted TEXT, server TEXT, username_encrypted TEXT, last_used NUMERIC)");
		db.exec("CREATE INDEX autofillEncryptedServer ON autofill_encrypted(server ASC)");
	}
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


	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT id, username_encrypted, password_encrypted, data_encrypted FROM autofill_encrypted "
				  "WHERE server=? ORDER BY last_used DESC");
	query.addBindValue(host);
	query.exec();

	if (query.next() && hasPermission()) {
		do {
			PasswordEntry data{};
			data.id = query.value(0);
			data.host = host;
			data.username = query.value(1).toString();
			data.password = query.value(2).toString();
			data.data = query.value(3).toByteArray();

			if (decryptPasswordEntry(data, &aesDecryptor))
				list.append(data);

		} while (query.next());
	}

	return list;
}

QVector<PasswordEntry> DatabaseEncryptedPasswordBackend::getAllEntries()
{
	QVector<PasswordEntry> list;
	AesInterface aesDecryptor{};

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT id, server, username_encrypted, password_encrypted, data_encrypted FROM autofill_encrypted");
	query.exec();

	if (query.next() && hasPermission()) {
		do {
			PasswordEntry data{};
			data.id = query.value(0);
			data.host = query.value(1).toString();
			if (data.host == INTERNAL_SERVER_ID)
				continue;

			data.username = query.value(2).toString();
			data.password = query.value(3).toString();
			data.data = query.value(4).toByteArray();

			if (decryptPasswordEntry(data, &aesDecryptor))
				list.append(data);

		} while (query.next());
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
	// Data is empty only for HTTP/FTP authorization
	if (entry.data.isEmpty()) {
		// Multiple-usernames for HTTP/FTP authorization not supported
		QSqlQuery query{SqlDatabase::instance()->database()};
		query.prepare("SELECT username_encrypted FROM autofill_encrypted WHERE server=?");
		query.addBindValue(entry.host);
		query.exec();

		if (query.next())
			return;
	}

	PasswordEntry encryptedEntry{entry};
	AesInterface aesEncryptor{};

	if (hasPermission() && encryptPasswordEntry(encryptedEntry, &aesEncryptor)) {
		QSqlQuery query{SqlDatabase::instance()->database()};
		query.prepare("INSERT INTO autofill_encrypted (server, data_encrypted, username_encrypted, password_encrypted, last_used) "
					  "VALUES (?,?,?,?,strftime('%s', 'now'))");
		query.bindValue(0, encryptedEntry.host);
		query.bindValue(1, encryptedEntry.data);
		query.bindValue(2, encryptedEntry.username);
		query.bindValue(3, encryptedEntry.password);
		query.exec();
	}
}

bool DatabaseEncryptedPasswordBackend::updateEntry(const PasswordEntry& entry)
{
	PasswordEntry encryptedEntry = entry;
	AesInterface aesEncryptor{};

	if (hasPermission() && encryptPasswordEntry(encryptedEntry, &aesEncryptor)) {
		QSqlQuery query{SqlDatabase::instance()->database()};

		// Data is empty only for HTTP/FTP authorization
		if (entry.data.isEmpty()) {
			query.prepare("UPDATE autofill_encrypted SET username_encrypted=?, password_encrypted=? WHERE server=?");
			query.bindValue(0, encryptedEntry.username);
			query.bindValue(1, encryptedEntry.password);
			query.bindValue(2, encryptedEntry.host);
		}
		else {
			query.prepare("UPDATE autofill_encrypted SET data_encrypted=?, username_encrypted=?, password_encrypted=? WHERE id=?");
			query.addBindValue(encryptedEntry.data);
			query.addBindValue(encryptedEntry.username);
			query.addBindValue(encryptedEntry.password);
			query.addBindValue(encryptedEntry.id);
		}

		return query.exec();
	}

	return false;
}

void DatabaseEncryptedPasswordBackend::updateLastUsed(PasswordEntry& entry)
{
	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("UPDATE autofill_encrypted SET last_used=strftime('%s', 'now') WHERE id=?");
	query.addBindValue(entry.id);
	query.exec();
}

void DatabaseEncryptedPasswordBackend::removeEntry(const PasswordEntry& entry)
{
	if (!hasPermission())
		return;

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("DELETE FROM autofill_encrypted WHERE id=?");
	query.addBindValue(entry.id);
	query.exec();

	m_stateOfMasterPassword = UnknownState;

	if (someDataFromDatabase().isEmpty())
		updateSampleData(m_masterPassword);
}

void DatabaseEncryptedPasswordBackend::removeAll()
{
	if (!hasPermission())
		return;

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("DELETE FROM autofill_encrypted");
	query.exec();

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

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT id, data_encrypted, password_encrypted, username_encrypted, server FROM autofill_encrypted");
	query.exec();

	AesInterface encryptor;
	AesInterface decryptor;

	while (query.next()) {
		QString server{query.value(4).toString()};
		if (server == INTERNAL_SERVER_ID)
			continue;

		int id{query.value(0).toInt()};
		QByteArray data{query.value(1).toString().toUtf8()};
		QByteArray password{query.value(2).toString().toUtf8()};
		QByteArray username{query.value(3).toString().toUtf8()};

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

		QSqlQuery updateQuery{SqlDatabase::instance()->database()};
		updateQuery.prepare("UPDATE autofill_encrypted SET data_encrypted = ?, password_encrypted = ?, username_encrypted = ? WHERE id = ?");
		updateQuery.addBindValue(data);
		updateQuery.addBindValue(password);
		updateQuery.addBindValue(username);
		updateQuery.addBindValue(id);
		query.exec();
	}
}

void DatabaseEncryptedPasswordBackend::updateSampleData(const QByteArray& password)
{
	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT id FROM autofill_encrypted WHERE server = ?");
	query.addBindValue(INTERNAL_SERVER_ID);
	query.exec();

	if (!password.isEmpty()) {
		AesInterface aes{};
		m_someDataStoredOnDatabase = aes.encrypt(AesInterface::createRandomData(16), password);

		if (query.next())
			query.prepare("UPDATE autofill_encrypted SET password_encrypted = ? WHERE server=?");
		else
			query.prepare("INSERT INTO autofill_encrypted (password_encrypted, server) VALUES (?,?)");

		query.addBindValue(QString::fromUtf8(m_someDataStoredOnDatabase));
		query.addBindValue(INTERNAL_SERVER_ID);
		query.exec();

		m_stateOfMasterPassword = PasswordIsSetted;
	}
	else if (query.next()) {
		query.prepare("DELETE FROM autofill_encrypted WHERE server = ?");
		query.addBindValue(INTERNAL_SERVER_ID);
		query.exec();

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

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT password_encrypted, data_encrypted, username_encrypted FROM autofill_encrypted");
	query.exec();

	QByteArray someData{};

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
}

}