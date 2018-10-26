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

#include "DatabasePasswordBackend.hpp"

#include <QSqlQuery>

#include "Password/AutoFill/AutoFill.hpp"

namespace Sn {

DatabasePasswordBackend::DatabasePasswordBackend() :
		PasswordBackend()
{
	// Empty
}

QString DatabasePasswordBackend::name() const
{
	return AutoFill::tr("Database (plaintext)");
}

QVector<PasswordEntry> DatabasePasswordBackend::getEntries(const QUrl& url)
{
	const QString host{PasswordManager::createHost(url)};
	QVector<PasswordEntry> list;

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT id, username, password, data FROM autofill "
				  "WHERE server=? ORDER BY last_used DESC");
	query.addBindValue(host);
	query.exec();

	while (query.next()) {
		PasswordEntry data{};
		data.id = query.value(0);
		data.host = host;
		data.username = query.value(1).toString();
		data.password = query.value(2).toString();
		data.data = query.value(3).toByteArray();

		list.append(data);
	}

	return list;
}

QVector<PasswordEntry> DatabasePasswordBackend::getAllEntries()
{
	QVector<PasswordEntry> list;

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT id, server, username, password, data FROM autofill");
	query.exec();

	while (query.next()) {
		PasswordEntry data{};
		data.id = query.value(0);
		data.host = query.value(1).toString();
		data.username = query.value(2).toString();
		data.password = query.value(3).toString();
		data.data = query.value(4).toByteArray();

		list.append(data);
	}

	return list;
}

void DatabasePasswordBackend::addEntry(const PasswordEntry& entry)
{
	// Data is empty only for HTTP/FTP authorization
	if (entry.data.isEmpty()) {
		// Multiple-usernames for HTTP/FTP authorization not supported
		QSqlQuery query{SqlDatabase::instance()->database()};
		query.prepare("SELECT username FROM autofill WHERE server=?");
		query.addBindValue(entry.host);
		query.exec();

		if (query.next()) 
			return;
	}

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("INSERT INTO autofill (server, data, username, password, last_used) "
				  "VALUES (?,?,?,?,strftime('%s', 'now'))");
	query.bindValue(0, entry.host);
	query.bindValue(1, entry.data);
	query.bindValue(2, entry.username);
	query.bindValue(3, entry.password);
	query.exec();
}

bool DatabasePasswordBackend::updateEntry(const PasswordEntry& entry)
{
	QSqlQuery query{SqlDatabase::instance()->database()};

	// Data is empty only for HTTP/FTP authorization
	if (entry.data.isEmpty()) {
		query.prepare("UPDATE autofill SET username=?, password=? WHERE server=?");
		query.bindValue(0, entry.username);
		query.bindValue(1, entry.password);
		query.bindValue(2, entry.host);
	}
	else {
		query.prepare("UPDATE autofill SET data=?, username=?, password=? WHERE id=?");
		query.addBindValue(entry.data);
		query.addBindValue(entry.username);
		query.addBindValue(entry.password);
		query.addBindValue(entry.id);
	}

	return query.exec();
}

void DatabasePasswordBackend::updateLastUsed(PasswordEntry& entry)
{
	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("UPDATE autofill SET last_used=strftime('%s', 'now') WHERE id=?");
	query.addBindValue(entry.id);
	query.exec();
}

void DatabasePasswordBackend::removeEntry(const PasswordEntry& entry)
{
	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("DELETE FROM autofill WHERE id=?");
	query.addBindValue(entry.id);
	query.exec();
}

void DatabasePasswordBackend::removeAll()
{
	QSqlQuery query{SqlDatabase::instance()->database()};
	query.exec("DELETE FROM autofill");
}
}