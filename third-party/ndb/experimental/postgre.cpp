#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <ndb/initializer.hpp>
#include <ndb/engine/postgre/postgre.hpp>
#include <ndb/preprocessor.hpp>
#include <ndb/engine/connection_param.hpp>
#include <ndb/query.hpp>
#include <iostream>

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
ndb_database(library, library, ndb::postgre)
)


int main(int argc, char **argv)
{
    using db = ndb::databases::exp_main::library_;

    ndb::initializer<ndb::postgre> init;

    ndb::connection_param params;

    params.host = "localhost";
    params.user = "postgres";
    params.port = 5432;
    params.path = "database";
    params.flag = ndb::connection_flag::read_only;

    try
    {
        ndb::connect<db>(params);

        ndb::query<db>() + (db{}.movie.name = "Interstellar");

    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
    }


    return 0;
}