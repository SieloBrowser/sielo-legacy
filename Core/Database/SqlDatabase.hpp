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
#ifndef SIELO_BROWSER_SQLDATABASE_HPP
#define SIELO_BROWSER_SQLDATABASE_HPP

#include <ndb/initializer.hpp>
#include <ndb/engine/sqlite/sqlite.hpp>
#include <ndb/preprocessor.hpp>
#include <ndb/type.hpp>

#include <QString>
#include <QByteArray>

using Opt_NotNull = ndb::field_option::not_null;

// Password
ndb_table(autofill,
	ndb_field_id,
	ndb_field(data, QString, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(password, QString, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(username, QString, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(server, QString, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(last_used, QString, ndb::size<16>, ndb::option<Opt_NotNull>)
)

ndb_table(autofill_encrypted,
	ndb_field_id,
	ndb_field(data_encrypted, QString, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(password_encrypted, QString, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(username_encrypted, QString, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(server, QString, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(last_used, QString, ndb::size<16>, ndb::option<Opt_NotNull>)
)

ndb_table(autofill_exceptions,
	ndb_field_id,
	ndb_field(server, QString, ndb::size<255>, ndb::option<Opt_NotNull>)
)

ndb_model(password, autofill, autofill_encrypted, autofill_exceptions)

// History (called "Navigation")
ndb_table(history,
	ndb_field_id,
	ndb_field(title, QString, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(url, QString, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(date, qint64, ndb::option<Opt_NotNull>),
	ndb_field(count, int, ndb::option<Opt_NotNull>)
)

ndb_model(navigation, history)

// Icons (in 
ndb_table(icons,
	ndb_field_id,
	ndb_field(icon, QByteArray),
	ndb_field(url, QString, ndb::size<255>, ndb::option<Opt_NotNull>))

ndb_model(images, icons)

ndb_project(sielo, ndb_database(password, password, ndb::sqlite), ndb_database(navigation, navigation, ndb::sqlite), ndb_database(images, images, ndb::sqlite))

namespace dbs
{
using password = ndb::databases::sielo::password_;
using navigation = ndb::databases::sielo::navigation_;
using image = ndb::databases::sielo::images_;
}

using sielo_scope = ndb::scope::group<ndb::databases::sielo>;

namespace ndb
{
ndb_bijective_type_map(string_, QString, sielo_scope)

ndb_bijective_type_map(ndb::types::int64_, qint64, sielo_scope)

ndb_bijective_type_map(byte_array_, QByteArray, sielo_scope)
}
#endif //SIELO_BROWSER_SQLDATABASE_HPP
