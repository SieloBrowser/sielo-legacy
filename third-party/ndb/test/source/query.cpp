#include "../test.hpp"

#include <ndb/initializer.hpp>
#include <ndb/query.hpp>
#include <ndb/function.hpp>
#include <ndb/preprocessor.hpp>

ndb_table(
         movie
        , ndb_field(id, int)
        , ndb_field(name, std::string, ndb::size<255>)
        , ndb_field(image, std::string, ndb::size<255>)
)

ndb_model(library, movie)

ndb_project(
    query,
    ndb_database(db, library, ndb::sqlite)
)

namespace dbs
{
    using zeta = ndb::databases::query::db_;
}

// aliases
static constexpr const auto movie = ndb::models::library.movie;

template<class Engine, class T>
testing::AssertionResult result_line_field_eq(ndb::result<Engine> result, int index, const T& field)
{
    try
    {

        /*if (result[0][index].template get<typename T::value_type>() == result[0][field])
            return testing::AssertionSuccess();*/
        //else return testing::AssertionFailure() << "Field index does not match field type";
    }
    catch(const std::exception& e)
    {
        return testing::AssertionFailure() << "Exception : " <<  e.what();
    }
    return testing::AssertionSuccess();
}

template <typename Engine>
class query : public testing::Test
{
public:
    query()
    {
        ndb::connect<dbs::zeta, Engine>( );
        ndb::clear<dbs::zeta>(movie);
    }
};

typedef ::testing::Types<TEST_ENGINE> Engines;
TYPED_TEST_CASE( query, Engines );

TYPED_TEST(query, general)
{
    using Engine = TypeParam;

    try
    {

        ndb::result<dbs::zeta> result;

        // insert 2 rows with unique data
        ASSERT_NO_THROW((result = ndb::query<dbs::zeta>() + (movie.id = 1, movie.name = "2", movie.image = "9")));
        ASSERT_NO_THROW((result = ndb::query<dbs::zeta>() + (movie.id = 3, movie.name = "4")));

        // get data
        ASSERT_NO_THROW((result = ndb::query<dbs::zeta>() << (movie.id, movie.name, movie.image)));

        // check data count
        ASSERT_TRUE(result.size() == 2);

        // check ndb::line access
        //ASSERT_TRUE(result_line_field_eq<Engine>(result, 0, movie.id));
        //ASSERT_TRUE(result_line_field_eq<Engine>(result, 1, movie.name));
        //ASSERT_TRUE(result_line_field_eq<Engine>(result, 2, movie.image));

        // check values
        ASSERT_TRUE(result[0][movie.id] == 1);
        ASSERT_TRUE(result[0][movie.name] == "2");

        ASSERT_TRUE(result[1][movie.id] == 3);
        ASSERT_TRUE(result[1][movie.name] == "4");

    } catch(const std::exception& e)
    {
        std::cout << e.what();
    }
}
