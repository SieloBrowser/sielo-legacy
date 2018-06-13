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

    // associate engine type id to ndb type
    template<> struct engine_ndb_type<sqlite, SQLITE_NULL> { using type = null_; };
    template<> struct engine_ndb_type<sqlite, SQLITE_INTEGER> { using type = int_; };
    template<> struct engine_ndb_type<sqlite, SQLITE_FLOAT> { using type = double_; };
    template<> struct engine_ndb_type<sqlite, SQLITE3_TEXT> { using type = string_; };
    template<> struct engine_ndb_type<sqlite, SQLITE_BLOB> { using type = byte_array_; };

    // associate ndb type to engine type id
    template<> struct engine_type_id<sqlite, null_> { static constexpr auto value = SQLITE_NULL; };
    template<> struct engine_type_id<sqlite, int_> { static constexpr auto value = SQLITE_INTEGER; };
    template<> struct engine_type_id<sqlite, double_> { static constexpr auto value = SQLITE_FLOAT; };
    template<> struct engine_type_id<sqlite, string_> { static constexpr auto value = SQLITE3_TEXT; };
    template<> struct engine_type_id<sqlite, byte_array_> { static constexpr auto value = SQLITE_BLOB; };


    template<class T>
    struct is_native_type<sqlite, T>
    {
        static constexpr bool value = std::is_same_v<std::decay_t<T>, int>
                                      || std::is_same_v<std::decay_t<T>, double>
                                      || std::is_same_v<std::decay_t<T>, std::string>
                                      || std::is_same_v<std::decay_t<T>, std::vector<char>>;
    };


/*
    // associate custom type to native type
    template<> struct native_type<sqlite, const char*> { using type = std::string; };
    template<class T, class U>
    struct native_type<sqlite, std::chrono::duration<T, U>> { using type = double; };

    // custom type conversions
    template<>
    struct type<sqlite> : basic_type<sqlite>
    {
        inline static auto encode(const char* v) -> native<std::decay_t<decltype(v)>>
        {
            return v;
        }

        template<class T, class U>
        inline static auto encode(const std::chrono::duration<T, U>& v) -> native<std::decay_t<decltype(v)>>
        {
            return v.count();
        }
        template<class T>
        inline static T decode(const native<T>& v)
        {
            return T{ v };
        };
    };*/
} // ndb


#endif // ENGINE_SQLITE_TYPE_H_NDB
