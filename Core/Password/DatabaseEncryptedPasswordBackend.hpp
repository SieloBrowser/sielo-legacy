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
#ifndef SIELO_BROWSER_DATABASEENCRYPTEDPASSWORDBACKEND_HPP
#define SIELO_BROWSER_DATABASEENCRYPTEDPASSWORDBACKEND_HPP

#include "SharedDefines.hpp"

#include <QVector>

#include "Password/PasswordBackend.hpp"

namespace Sn {
class AesInterface;

class SIELO_SHAREDLIB DatabaseEncryptedPasswordBackend: public PasswordBackend {
public:
	enum MasterPasswordState {
		PasswordIsSetted,
		PasswordIsNotSetted,
		UnknownState = -1
	};

	DatabaseEncryptedPasswordBackend();
	~DatabaseEncryptedPasswordBackend();

	QVector<PasswordEntry> getEntries(const QUrl& url);
	QVector<PasswordEntry> getAllEntries();

	void setActive(bool active);

	void addEntry(const PasswordEntry& entry);
	bool updateEntry(const PasswordEntry& entry);
	void updateLastUsed(PasswordEntry& entry);

	void removeEntry(const PasswordEntry& entry);
	void removeAll();

	QString name() const;

	bool hasSettings() const { return true; }
	void showSettings(QWidget* parent);

	bool isMasterPasswordSetted();

	QByteArray masterPassword() const { return m_masterPassword; }

	bool hasPermission();
	bool isPasswordVerified(const QByteArray& password);

	bool decryptPasswordEntry(PasswordEntry& entry, AesInterface* aesInterface);
	bool encryptPasswordEntry(PasswordEntry& entry, AesInterface* aesInterface);

	void tryToChangeMasterPassword(const QByteArray& newPassword);
	void removeMasterPassword();

	void setAskMasterPasswordState(bool ask);

	void encryptDatabaseTableOnFly(const QByteArray& decryptorPassword, const QByteArray& encryptorPassword);

	void updateSampleData(const QByteArray& password);

	void showMasterPasswordDialog();

private:
	QByteArray someDataFromDatabase();

	MasterPasswordState m_stateOfMasterPassword{};
	QByteArray m_someDataStoredOnDatabase{};

	bool m_askPasswordDialogVisible{false};
	bool m_askMasterPassword{false};
	QByteArray m_masterPassword{};
};

}

#endif //SIELO_BROWSER_DATABASEENCRYPTEDPASSWORDBACKEND_HPP
