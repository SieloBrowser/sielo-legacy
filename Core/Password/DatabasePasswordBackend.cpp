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

#include <ndb/function.hpp>

#include "Password/AutoFill/AutoFill.hpp"

constexpr auto& autofill = ndb::models::password.autofill;

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

	// TODO: manage order
	for (auto& data : ndb::oquery<dbs::password>() << (autofill.server == host))
		list.append(PasswordEntry(data));

//	query.prepare("SELECT id, username, password, data FROM autofill WHERE server=? ORDER BY last_used DESC");

	return list;
}

QVector<PasswordEntry> DatabasePasswordBackend::getAllEntries()
{
	QVector<PasswordEntry> list;

	for (auto& data : ndb::oquery<dbs::password>() << autofill)
		list.append(PasswordEntry(data));

	return list;
}

void DatabasePasswordBackend::addEntry(const PasswordEntry& entry)
{
	if (entry.data.isEmpty()) {
		auto data = ndb::query<dbs::password>() << ((autofill.username)
				<< (autofill.server == entry.host));

		if (data.has_result())
			return;
	}

	ndb::query<dbs::password>() + (autofill.server = entry.host,
								   autofill.data = QString::fromUtf8(entry.data),
								   autofill.username = entry.username,
								   autofill.password = entry.password,
								   autofill.last_used = ndb::now());
}

bool DatabasePasswordBackend::updateEntry(const PasswordEntry& entry)
{
	if (entry.data.isEmpty()) {
		ndb::query<dbs::password>() >> ((autofill.username = entry.username,
				autofill.password = entry.password)
				<< (autofill.server == entry.host));
	}
	else {
		ndb::query<dbs::password>() >> ((autofill.data = QString::fromUtf8(entry.data),
										 autofill.username = entry.username,
										 autofill.password = entry.password)
				<< (autofill.id == entry.id.toLongLong()));
	}

	return true;
}

void DatabasePasswordBackend::updateLastUsed(PasswordEntry& entry)
{
	ndb::query<dbs::password>() >> ((autofill.last_used = ndb::now())
			<< (autofill.id == entry.id.toLongLong()));
}

void DatabasePasswordBackend::removeEntry(const PasswordEntry& entry)
{
	ndb::query<dbs::password>() - (autofill.id == entry.id.toLongLong());
}

void DatabasePasswordBackend::removeAll()
{
	ndb::clear<dbs::password>(autofill);
}
}