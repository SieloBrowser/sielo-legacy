//#define NDB_DEBUG_QUERY

#include <ndb/initializer.hpp>
#include <ndb/engine/sqlite/sqlite.hpp>
#include <ndb/query.hpp>
#include <iostream>
#include <sstream>

#include <ndb/expression/deduce.hpp>
#include <ndb/preprocessor.hpp>

/* tree_table

template<class Table, class Depth>
ndb::hierarchy_table<Table>
{
    static constexpr struct parent_id_ : ndb::field<Table, int> {} parent_id {};
    static constexpr struct depth_ : ndb::field<Table, int> {} depth {};
    static constexpr struct path_ : ndb::field<Table, ndb::bytes, ndb::size<Depth>> {} path {};
}
struct my_htable : ndb::table<Model>, ndb::hierarchy_table<my_htable, depth<10>>
{
    // name
}

query() << (myhtable.name) << ndb::hierarchy_table::childs(id)

 template<class Table>
ndb::connection_table<Table>
{
    static constexpr struct source_id_ : ndb::field<Table, int> {} source_id {};
    static constexpr struct target_id_ : ndb::field<Table, int> {} target_id {};
}




struct my_table : ndb::table<Model>, ndb::connection_table<my_ctable>
{
    // id
    // name
    static constexpr struct connexions_ : ndb::field<my_table, int> {} connexions {};
}

 */

// database

/*
ndb_table(
movie
, ndb_field_id
, ndb_field(f1, char)
, ndb_field(f2, int8_t, ndb::size<255>)
, ndb_field(f3, int16_t, ndb::size<255>)
, ndb_field(f3, int32_t, ndb::size<255>)
)*/

ndb_table(movie
          , ndb_field_id
          , ndb_field(name, std::string, ndb::size<255>)
          , ndb_field(director, std::string, ndb::size<255>)
)
ndb_table(music,
          ndb_field(id, int),
          ndb_field(image, std::string)
)
ndb_model(library, movie, music)

ndb_project(exp_main,
            ndb_database(library, library, ndb::sqlite)
)


//#include "database.hpp"



int main()
{
    using ndb::sqlite;
    // using db = dbs::zeta;
    using db = ndb::databases::exp_main::library_;
    constexpr auto library = ndb::models::library;

    std::stringstream result;

    try
    {
        ndb::initializer<sqlite> init;
        ndb::connect<db>();

        std::string match_expr = "%test%";

        /*
        ndb::types<bool_ int8, int16, int32>

        ndb::engine_types<sqlite, null_, int_, float_, string_, vector_>

        // storage types from engine types
        ndb::storage_type<sqlite, int64_t, null_type, double, std::string>

        int int_
        int8_t int8_
        int16_t int16_

        int_ int64
        int8_ int64
        int16_ int64
        bool_ int64

*/


        ndb::query<db>() + (library.movie.name = "test");

        std::cout << ndb::last_id<db>();



        //ndb::query<db>() << (library.movie.name << ndb::native("native_expr") );


        //ndb::query<db>() << (ndb::match( library.movie.name, match_expr ) || ndb::match( library.movie.director, match_expr ));


    } catch(const std::exception& e)
    {
        std::cout << e.what();
    }

    return 0;

}