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

ndb_table(autofill,
		  ndb_field_id,
		  ndb_field(data, std::string, ndb::size<255>),
		  ndb_field(password, std::string, ndb::size<255>),
		  ndb_field(username, std::string, ndb::size<255>),
		  ndb_field(server, std::string, ndb::size<255>),
		  ndb_field(last_used, std::string, ndb::size<16>)
)

ndb_table(autofill_encrypted,
		  ndb_field_id,
		  ndb_field(data_encrypted, std::string, ndb::size<255>),
		  ndb_field(password_encrypted, std::string, ndb::size<255>),
		  ndb_field(username_encrypted, std::string, ndb::size<255>),
		  ndb_field(server, std::string, ndb::size<255>),
		  ndb_field(last_used, std::string, ndb::size<16>)
)

ndb_table(autofill_exceptions,
		  ndb_field_id,
		  ndb_field(server, std::string, ndb::size<255>)
)

ndb_model(password, autofill, autofill_encrypted, autofill_exceptions)

ndb_project(sielo, ndb_database(password, password, ndb::sqlite))

namespace dbs {
using password = ndb::databases::sielo::password_;


}

#endif //SIELO_BROWSER_SQLDATABASE_HPP
