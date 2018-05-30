#include "../test.hpp"

#include <ndb/initializer.hpp>
#include <ndb/query.hpp>
#include <ndb/function.hpp>

#include "../database.hpp"



// aliases
static constexpr const auto movie = ndb::models::library.movie;

template <typename Engine>
class expression : public ::testing::Test
{
public:
    expression()
    {
        ndb::connect<dbs::zeta, Engine>();
        ndb::clear<dbs::zeta>(movie);
    }
    ndb::query<dbs::zeta> q;
};

typedef ::testing::Types<TEST_ENGINE> Engines;
TYPED_TEST_CASE(expression, Engines);


TYPED_TEST(expression, simple)
{
    EXPECT_NO_THROW( this->q << movie.id );
    EXPECT_NO_THROW( this->q << (movie.id, movie.name, movie.image) );
}

TYPED_TEST(expression, explicit_keyword)
{
    EXPECT_NO_THROW( this->q << ndb::get(movie.id) );
    //TODO
    //EXPECT_NO_THROW( this->q << (ndb::get(movie.id) << ndb::source(movie)) );
}

TYPED_TEST(expression, function)
{
    EXPECT_NO_THROW( this->q << (ndb::now(), movie.id, ndb::now()) );
    EXPECT_NO_THROW( this->q << (movie.id, movie.name, movie.image) );
}

TYPED_TEST(expression, composed)
{
    EXPECT_NO_THROW( this->q << ((ndb::now(), movie.id) << ndb::limit(2)) );
    EXPECT_NO_THROW( this->q << ((ndb::now(), movie.id) << ndb::limit(2)) );
}