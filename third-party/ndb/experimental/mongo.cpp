#include <ndb/initializer.hpp>
#include <ndb/engine/mongo/mongo.hpp>
#include <ndb/query.hpp>

#include "database.hpp"

#include <iostream>

template<class Database>
class query_impl
{
public:
    constexpr query_impl() {}

    template<class Expr>
    const query_impl& operator+(const Expr& expr) const
    {
        const auto& engine = ndb::engine<typename Database::engine>::get();

        engine.template exec<Database>(expr);

        return *this;
    }
};


static constexpr const models::library library;
static constexpr const databases::project project;

template<class Database>
static constexpr const auto query = query_impl<Database>();

int main()
{
    using ndb::mongo;
    using ndb::query;

    try
    {
        const auto& movie = library.movie;
        const auto& music = library.music;

        int a = 50;
        int b = 88;

        project.alpha.movie.name;

        ndb::initializer<mongo> init;

        ndb::connect<dbs::alpha>();

        // field
        //ndb::query<dbs::alpha>() << ( movie.id );
        // field_list
        ndb::query<dbs::alpha>() << ( movie.id, movie.image );
        /*
        // field << table
        ndb::query<dbs::alpha>() << ( movie.id << movie );
        // coondition
        ndb::query<dbs::alpha>() << ( movie.id == a );
        ndb::query<dbs::alpha>() << ( movie.id == a && movie.name == b );
        // field << table << condition
        ndb::query<dbs::alpha>() << ( (movie.id) << movie << (movie.id == a && movie.name == b) );
        ndb::query<dbs::alpha>() << ( (movie.id, movie.name) << movie << (movie.id == a && movie.name == b) );
        // field << condition
        ndb::query<dbs::alpha>() << ( (movie.id, movie.name, movie.image) << (movie.id == a && movie.name == b) );
        // field << join
        //ndb::query<db::library>{} << movie.name << (  movie.user_id == user.id );
        // add
        ndb::query<dbs::alpha>() + ((movie.id = a, movie.name = b));
        // del
        ndb::query<dbs::alpha>() - ( movie.id == 3 );
*/
        //ndb::query<dbs::alpha>() + ((movie.id = a, movie.name = b));



    } catch (const std::exception& e)
    {
        std::cerr << "Exception : " << e.what();
    }
    return 0;
}