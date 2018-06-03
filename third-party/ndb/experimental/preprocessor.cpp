#include <ndb/initializer.hpp>
#include <ndb/engine/sqlite/sqlite.hpp>
#include <ndb/preprocessor.hpp>
#include <ndb/query.hpp>

ndb_table(movie,
          ndb_field_id,
          ndb_field(name, std::string, ndb::size<255>)
)
ndb_table(music,
          ndb_field(id, int),
          ndb_field(image, std::string)
)
ndb_model(library, movie, music)

ndb_project(my_project,
            ndb_database(alpha, library, ndb::sqlite),
            ndb_database(zeta, library, ndb::sqlite)
)

// alias
namespace dbs
{
    using alpha = ndb::databases::my_project::alpha_;
    using zeta = ndb::databases::my_project::zeta_;
}

int main()
{
    const auto& movie = ndb::models::library.movie;

    ndb::initializer<ndb::sqlite> init;
    ndb::connect<dbs::zeta>();

    ndb::query<dbs::zeta>() << (movie.id);
}