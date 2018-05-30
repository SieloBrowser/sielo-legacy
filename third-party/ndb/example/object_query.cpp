#include <ndb/initializer.hpp>
#include <ndb/engine/sqlite/sqlite.hpp> // engine
#include <ndb/preprocessor.hpp> // database macros
#include <ndb/function.hpp> // ndb::clear
#include <ndb/query.hpp> // query and expression

/*! Generate database entities (instances, structures etc ...)
 * Entites are generated in namespaces ndb::models, ndb::tables, ndb::databases::my_project
 * Add _ to the name tu use the type (example : ndb::tables::movie_)
 */

// database
ndb_table(movie,
          ndb_field_id,
          ndb_field(name, std::string, ndb::size<255>),
          ndb_field(duration, std::chrono::duration<double, std::ratio<3600>>)
)
ndb_table(music,
          ndb_field(id, int),
          ndb_field(image, std::string)
)
ndb_model(library, movie, music)

ndb_project(my_project,
            ndb_database(library, library, ndb::sqlite)
)

// alias
namespace dbs
{
    using library = ndb::databases::my_project::library_;
}

int main()
{
    using namespace std::chrono_literals;
    const auto& movie = ndb::models::library.movie;

    ndb::initializer<ndb::sqlite> init;
    //! connect to database library
    ndb::connect<dbs::library>();
    //! clear movie table
    ndb::clear<dbs::library>(movie);

    //! add records
    ndb::query<dbs::library>() + (movie.name = "Interstellar", movie.duration = 2.49h);
    ndb::query<dbs::library>() + (movie.name = "Watchmen", movie.duration = 3.30h);


    auto [interstellar] = ndb::oquery<dbs::library>() << (movie.name == "Watchmen");
    std::cout << interstellar.id << " | " << interstellar.name << std::endl;

    for (auto [id, name, duration] : ndb::oquery<dbs::library>() << movie)
    {
        std::cout << "id : " << id << std::endl;
        std::cout << "name : " << name << std::endl;
        std::cout << "name : " << duration.count() << std::endl;
    }
    return 0;
}