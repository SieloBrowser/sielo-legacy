#ifndef FUNCTION_SQL_H_NDB
#define FUNCTION_SQL_H_NDB

#include <ndb/function/generic.hpp>
#include <ndb/engine.hpp>
#include <ndb/utility.hpp>

#include <iostream>

namespace ndb::functions
{
    template<class Engine, class Database, class Table>
    struct clear<Engine, Database, Table, ndb::expr_category_code::sql>
    {
        static auto process()
        {
            const auto& engine = ndb::engine<Engine>::get();
            auto q = "TRUNCATE TABLE `T" + std::to_string(ndb::table_id<Table>) + "`";
            return engine.template exec<Database>(q);
        }
    };

    template<class Engine, class Database>
    struct remove<Engine, ndb::expr_category_code::sql, Database>
    {
        static auto process()
        {
            return "DROP DATABASE `D" + std::to_string(ndb::database_id<Database>) + "`";
        }
    };
} // ndb::functions

#endif // FUNCTION_SQL_H_NDB