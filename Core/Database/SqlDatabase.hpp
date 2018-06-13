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

// History and Bookmarks (called "Navigation")
ndb_table(history,
	ndb_field_id,
	ndb_field(title, std::string, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(url, std::string, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(date, std::string, ndb::size<255>, ndb::option<Opt_NotNull>),
	ndb_field(count, int, ndb::option<Opt_NotNull>)
)

ndb_model(navigation, history)

ndb_project(sielo, ndb_database(password, password, ndb::sqlite), ndb_database(navigation, navigation, ndb::sqlite))

namespace dbs
{
using password = ndb::databases::sielo::password_;
using navigation = ndb::databases::sielo::navigation_;
}

#endif //SIELO_BROWSER_SQLDATABASE_HPP
