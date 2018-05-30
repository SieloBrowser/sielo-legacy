#include "../test.hpp"

#include <ndb/preprocessor.hpp>

struct user_type{};
struct unregistered_user_type{};

struct user_string{};



ndb_table(
    movie
    , ndb_field(id, int)
    , ndb_field(custom, user_type)
    , ndb_field(name, std::string, ndb::size<255>)
)

ndb_model(library, movie)

ndb_project(group,
            ndb_database(db, library, ndb::sqlite),
            ndb_database(db2, library, ndb::sqlite)
)

ndb_project(group2,
            ndb_database(db, library, ndb::sqlite)
)

using db = ndb::databases::group::db_;
using db2 = ndb::databases::group::db2_;
using dbgroup2 = ndb::databases::group2::db_;

struct map_group{};
struct map_group2{};
struct map_db{};
struct map_db2{};

namespace ndb
{
    // user type
    template<class Database>
    struct custom_type<user_type, Database>
    {
        using ndb_type = ndb::int_;
    };
}


#define TYPE_MAP_CHECK(NDB_TYPE, CPP_TYPE) \
ASSERT_TRUE(( std::is_same_v< ndb::cpp_type_t<NDB_TYPE>, CPP_TYPE > )); \
ASSERT_TRUE(( std::is_same_v< ndb::ndb_type_t<CPP_TYPE>, NDB_TYPE > ));

TEST(type_map, global)
{
    TYPE_MAP_CHECK(ndb::int_, int);
    TYPE_MAP_CHECK(ndb::float_, float);
    TYPE_MAP_CHECK(ndb::double_, double);
    TYPE_MAP_CHECK(ndb::string_, std::string);
    TYPE_MAP_CHECK(ndb::byte_array_, std::vector<char>);

    // custom type
    ASSERT_TRUE(( ndb::is_custom_type<user_type>::value ));
    ASSERT_TRUE(( std::is_same_v< ndb::ndb_type_t<user_type>, ndb::int_ > ));
    ASSERT_FALSE(( std::is_same_v< ndb::ndb_type_t<user_type>, ndb::float_ > ));

    // unregistered user type
    ASSERT_FALSE(( std::is_same_v< ndb::ndb_type_t<unregistered_user_type>, ndb::int_ > ));
    ASSERT_FALSE(( ndb::is_custom_type<unregistered_user_type>::value ));
}


// spe group
namespace ndb
{
    template< class Database, class Engine>
    struct type_map<ndb::string_, Database, ndb::databases::group, Engine> { using type = map_group; };
}

#define TYPE_MAP_CHECK_GROUP(NDB_TYPE, CPP_TYPE) \
ASSERT_TRUE(( std::is_same_v< ndb::cpp_type_t<NDB_TYPE, ndb::global_database, ndb::databases::group>, CPP_TYPE > )); \
ASSERT_TRUE(( std::is_same_v< ndb::ndb_type_t<CPP_TYPE, ndb::global_database, ndb::databases::group>, NDB_TYPE > ));

TEST(type_map, group)
{
    // new map for group
    TYPE_MAP_CHECK_GROUP(ndb::string_, map_group);
}

// spe db
namespace ndb
{
    template< class Engine>
    struct type_map<ndb::string_, db, ndb::databases::group, Engine> { using type = map_db; };

    // user type
    template<>
    struct custom_type<user_type, db>
    {
        using ndb_type = ndb::string_;
    };
}


#define TYPE_MAP_CHECK_DB(NDB_TYPE, CPP_TYPE) \
ASSERT_TRUE(( std::is_same_v< ndb::cpp_type_t<NDB_TYPE, db>, CPP_TYPE > )); \
ASSERT_TRUE(( std::is_same_v< ndb::ndb_type_t<CPP_TYPE, db>, NDB_TYPE > ));

TEST(type_map, db)
{
    // new map for db
    TYPE_MAP_CHECK_DB(ndb::string_, map_db);

    // custom type
    ASSERT_TRUE(( std::is_same_v< ndb::ndb_type_t<user_type, db>, ndb::string_ > ));
    // global user type is false
    ASSERT_FALSE(( std::is_same_v< ndb::ndb_type_t<user_type, db>, ndb::int_ > ));
}

