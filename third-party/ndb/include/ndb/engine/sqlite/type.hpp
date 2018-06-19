#ifndef ENGINE_SQLITE_TYPE_H_NDB
#define ENGINE_SQLITE_TYPE_H_NDB

#pragma once

#include <ndb/engine/type.hpp>
#include <ndb/type.hpp>

#include "sqlite3.h"
#include <chrono>
#include <string>
#include <vector>
#include <variant>

namespace ndb
{
    class sqlite;

    // map ndb type to type id
    ndb_engine_type_id_map(sqlite, null_, SQLITE_NULL);
    ndb_engine_type_id_map(sqlite, int64_, SQLITE_INTEGER);
    ndb_engine_type_id_map(sqlite, double_, SQLITE_FLOAT);
    ndb_engine_type_id_map(sqlite, string_, SQLITE3_TEXT);
    ndb_engine_type_id_map(sqlite, byte_array_, SQLITE_BLOB);


    // storage types
    template<> struct storage_type<sqlite, int_> { using type = int64_; };
    template<> struct storage_type<sqlite, datetime_> { using type = string_; };


    template<class Database>
    struct engine_types<sqlite, Database>
    {
        using type = std::variant
        <
            cpp_type_t<int64_, Database>,
            cpp_type_t<double_, Database>,
            cpp_type_t<string_, Database>,
            cpp_type_t<byte_array_, Database>
        >;
    };
} // ndb


#endif // ENGINE_SQLITE_TYPE_H_NDB
