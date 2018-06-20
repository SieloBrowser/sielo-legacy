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

    try
    {
        ndb::initializer<ndb::sqlite> init;
        //! connect to database library
        ndb::connect<dbs::library>();
        //! clear movie table
        ndb::clear<dbs::library>(movie);

        //! add records
        ndb::query<dbs::library>() + (movie.name = "Interstellar", movie.duration = 2.49h);
        ndb::query<dbs::library>() + (movie.name = "Watchmen", movie.duration = 3.30h);

        //! get movie with specified duration
        //! missing informations of the query are deduced compile time
        //! for SQL-like engines, the string is generated compile time
        for (auto& line : ndb::query<dbs::library>() << (movie.duration == 3.30h))
        {
            //! access fields from result using field name
            std::cout << "movie.id : " << line[movie.id] << std::endl;
            std::cout << "movie.name : " << line[movie.name] << std::endl;
            std::cout << "movie.duration : " << line[movie.duration].count() << " Hours" << std::endl;
        }
    }
    catch (const std::exception& e) { std::cout << e.what(); }

    return 0;
}