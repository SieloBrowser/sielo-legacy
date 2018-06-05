//! \brief custom database functions specialized for engines or engine category

#ifndef FUNCTION_H_NDB
#define FUNCTION_H_NDB

#include <ndb/function/generic.hpp>

namespace ndb
{
    template<class Database, class Table>
    auto clear(Table table)
    {
        return ndb::functions::clear<typename Database::engine, Database, Table>::process();
    }

    template<class Database, class Table>
    auto last_id(Table table)
    {
        return ndb::engine<typename Database::engine>::get().template last_id<Database>();
    }

    template<class Database>
    void remove()
    {
        //ndb::functions::remove<Database::engine::expr_category(), Database>::process();
    }
} // ndb

#endif // FUNCTION_H_NDB