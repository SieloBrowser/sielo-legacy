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

#include "PasswordManager.hpp"

#include <QCryptographicHash>

#include <QUrl>

#include "Utils/Settings.hpp"

#include "Password/PasswordBackend.hpp"
#include "Password/DatabasePasswordBackend.hpp"
#include "Password/DatabaseEncryptedPasswordBackend.hpp"

namespace Sn {

static const int passwordEntryVersion = 1;

QString PasswordManager::createHost(const QUrl& url)
{
	QString host{url.host()};

	if (host.isEmpty())
		host = url.toString();

	if (url.port() != -1) {
		host.append(QLatin1Char(':'));
		host.append(QString::number(url.port()));
	}

	return host;
}

QByteArray PasswordManager::urlEncodePassword(const QString& password)
{
	QByteArray encodedPass = QUrl::toPercentEncoding(password, " ");

	encodedPass.replace(' ', '+');
	encodedPass.replace('~', "%7E");

	return encodedPass;
}

QByteArray PasswordManager::passwordToHash(const QString& masterPassword)
{
	if (!masterPassword.isEmpty()) {
		QByteArray result{masterPassword.toUtf8()};

		result = QCryptographicHash::hash(result, QCryptographicHash::Sha1) + result;
		result = QCryptographicHash::hash(result, QCryptographicHash::Sha1);

		return result.toBase64();
	}
	else
		return QByteArray();
}

QDataStream& operator<<(QDataStream& stream, const PasswordEntry& entry)
{
	stream << passwordEntryVersion;
	stream << entry.host;
	stream << entry.id;
	stream << entry.username;
	stream << entry.password;
	stream << entry.data;
	stream << entry.updated;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, PasswordEntry& entry)
{
	int version;

	stream >> version;

	if (version != passwordEntryVersion)
		return stream;

	stream >> entry.host;
	stream >> entry.id;
	stream >> entry.username;
	stream >> entry.password;
	stream >> entry.data;
	stream >> entry.updated;

	return stream;
}

PasswordManager::PasswordManager(QObject* parent) :
	QObject(parent),
	m_loaded(false),
	m_databaseBackend(new DatabasePasswordBackend),
	m_databaseEncryptedBackend(new DatabaseEncryptedPasswordBackend)
{
	m_backends["database"] = m_databaseBackend;
	m_backends["database-encrypted"] = m_databaseEncryptedBackend;
}

PasswordManager::~PasswordManager()
{
	delete m_databaseBackend;
	delete m_databaseEncryptedBackend;
}

void PasswordManager::loadSettings()
{
	Settings settings{};
	QString backendId{settings.value("PasswordManager/backend", "database").toString()};

	m_backend = m_backends[m_backends.contains(backendId) ? backendId : "database"];
	m_backend->setActive(true);
}

QVector<PasswordEntry> PasswordManager::getEntries(const QUrl& url)
{
	ensureLoaded();
	return m_backend->getEntries(url);
}

QVector<PasswordEntry> PasswordManager::getAllEntries()
{
	ensureLoaded();
	return m_backend->getAllEntries();
}

void PasswordManager::addEntry(const PasswordEntry& entry)
{
	ensureLoaded();
	m_backend->addEntry(entry);
}

bool PasswordManager::updateEntry(const PasswordEntry& entry)
{
	ensureLoaded();
	return m_backend->updateEntry(entry);
}

void PasswordManager::updateLastUsed(PasswordEntry& entry)
{
	ensureLoaded();
	m_backend->updateLastUsed(entry);
}

void PasswordManager::removeEntry(const PasswordEntry& entry)
{
	ensureLoaded();
	m_backend->removeEntry(entry);
}

void PasswordManager::removeAllEntries()
{
	ensureLoaded();
	m_backend->removeAll();
}

QHash<QString, PasswordBackend*> PasswordManager::availableBackends()
{
	ensureLoaded();
	return m_backends;
}

PasswordBackend* PasswordManager::activeBackend()
{
	ensureLoaded();
	return m_backend;
}

void PasswordManager::switchBackend(const QString& backendID)
{
	PasswordBackend* backend = m_backends.value(backendID);

	if (!backend)
		return;

	m_backend->setActive(false);
	m_backend = backend;
	m_backend->setActive(true);

	Settings settings{};

	settings.setValue("PasswordManager/backend", backendID);

	emit passwordBackendChanged();
}

bool PasswordManager::registerBackend(const QString& id, PasswordBackend* backend)
{
	if (m_backends.contains(id))
		return false;

	m_backends[id] = backend;
	return true;
}

void PasswordManager::unregisterBackend(PasswordBackend* backend)
{
	const QString key{m_backends.key(backend)};

	m_backends.remove(key);

	if (m_backend == backend)
		m_backend = m_databaseBackend;
}

void PasswordManager::ensureLoaded()
{
	if (!m_loaded) {
		loadSettings();
		m_loaded = true;
	}
}

}
