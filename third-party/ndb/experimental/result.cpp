#include <iostream>

#include <ndb/initializer.hpp>
#include <ndb/engine/mongo/mongo.hpp>
#include <ndb/engine/sqlite/sqlite.hpp>
#include <ndb/query.hpp>

#include "database.hpp"

static constexpr const models::library library;
static constexpr const databases::project project;

int main()
{
    using ndb::mongo;
    using ndb::sqlite;
    using ndb::query;
    using zeta_query = ndb::query<dbs::zeta>;

    try
    {
        const auto& movie = library.movie;
        const auto& music = library.music;

        project.zeta.movie.name;

        ndb::initializer<mongo, sqlite> init;

        ndb::connect<dbs::alpha>();
        ndb::connect<dbs::zeta>();

        //query<dbs::alpha>() + (movie.id = 3, movie.name = 5);
        //query<dbs::zeta>() + (movie.id = 3, movie.name = 5);


        for (auto& line : zeta_query() << (movie.id, movie.name))
        {
            std::cout << "\nres zeta" << line[movie.id] << " " << line[movie.name];
        }

        for (auto& line : query<dbs::alpha>() << (movie.id))
        {
            std::cout << "\nres alpha" << line[movie.id];
        }


    } catch (const std::exception& e)
    {
        std::cerr << "Exception : " << e.what();
    }
    return 0;
}