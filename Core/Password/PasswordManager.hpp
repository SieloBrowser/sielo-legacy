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
#ifndef SIELO_BROWSER_PASSWORDMANAGER_HPP
#define SIELO_BROWSER_PASSWORDMANAGER_HPP

#include "SharedDefines.hpp"

#include <QObject>
#include <QVariant>
#include <QString>
#include <QByteArray>

#include <QString>

#include <QDataStream>

#include <QHash>
#include <QVector>

#include "Database/SqlDatabase.hpp"

namespace Sn {
class PasswordBackend;

class DatabasePasswordBackend;

class DatabaseEncryptedPasswordBackend;

struct PasswordEntry {
	QVariant id{};
	QString host{};
	QString username{};
	QString password{};
	QByteArray data{};
	int updated{-1};

	PasswordEntry() :
			updated(-1) {}

	bool isValid() const
	{
		return !password.isEmpty() && !host.isEmpty();
	}

	bool operator==(const PasswordEntry& other) const
	{
		return id == other.id;
	}

	bool operator<(const PasswordEntry& other) const
	{
		return updated > other.updated;
	}

	friend QDataStream& operator<<(QDataStream& stream, const PasswordEntry& entry);
	friend QDataStream& operator>>(QDataStream& stream, PasswordEntry& entry);

};

class SIELO_SHAREDLIB PasswordManager : public QObject {
Q_OBJECT

public:
	PasswordManager(QObject* parent = 0);
	~PasswordManager();

	void loadSettings();

	QVector<PasswordEntry> getEntries(const QUrl& url);
	QVector<PasswordEntry> getAllEntries();

	void addEntry(const PasswordEntry& entry);
	bool updateEntry(const PasswordEntry& entry);
	void updateLastUsed(PasswordEntry& entry);

	void removeEntry(const PasswordEntry& entry);
	void removeAllEntries();

	QHash<QString, PasswordBackend*> availableBackends();
	PasswordBackend* activeBackend();
	void switchBackend(const QString& backendID);

	bool registerBackend(const QString& id, PasswordBackend* backend);
	void unregisterBackend(PasswordBackend* backend);

	static QString createHost(const QUrl& url);
	static QByteArray urlEncodePassword(const QString& password);
	static QByteArray passwordToHash(const QString& masterPassword);

signals:
	void passwordBackendChanged();

private:
	void ensureLoaded();

	bool m_loaded{false};

	PasswordBackend* m_backend{nullptr};
	DatabasePasswordBackend* m_databaseBackend{nullptr};
	DatabaseEncryptedPasswordBackend* m_databaseEncryptedBackend{nullptr};

	QHash<QString, PasswordBackend*> m_backends;

};

}

Q_DECLARE_TYPEINFO(Sn::PasswordEntry, Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(Sn::PasswordEntry)

#endif //SIELO_BROWSER_PASSWORDMANAGER_HPP
