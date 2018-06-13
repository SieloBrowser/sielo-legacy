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

struct type_group{};
struct type_group2{};
struct type_group_db{};
struct type_group_db2{};
struct type_group2_db{};
struct type_global{};
struct type_engine{};
template<class T>
struct scoped_type{};


#define TYPE_MAP_CHECK(T, T2, Scope) ASSERT_TRUE(( std::is_same_v< ndb::type_get<T, Scope>, T2 > ));

#define BIJECTIVE_TYPE_MAP_CHECK(T, T2, Scope) \
ASSERT_TRUE(( std::is_same_v< ndb::type_get<T, Scope>, T2 > )); \
ASSERT_TRUE(( std::is_same_v< ndb::type_get<T2, Scope>, T > ));

TEST(type_map, system_scope)
{
    BIJECTIVE_TYPE_MAP_CHECK(ndb::int_, int, ndb::scope::system);
    BIJECTIVE_TYPE_MAP_CHECK(ndb::float_, float, ndb::scope::system);
    BIJECTIVE_TYPE_MAP_CHECK(ndb::double_, double, ndb::scope::system);
    BIJECTIVE_TYPE_MAP_CHECK(ndb::string_, std::string, ndb::scope::system);
    BIJECTIVE_TYPE_MAP_CHECK(ndb::byte_array_, std::vector<char>, ndb::scope::system);
}

// spe group
namespace ndb
{
    //template<> struct type_map<string_> { using type = std::string; };
    template<> struct type_map<string_, scope::group<ndb::databases::group>> { using type = type_group; };
    template<> struct type_map<type_group, scope::group<ndb::databases::group>> { using type = string_; };
}

TEST(type_map, group_scope)
{
    // new map for group
    TYPE_MAP_CHECK(ndb::string_, type_group, ndb::scope::group<ndb::databases::group>);
    TYPE_MAP_CHECK(type_group, ndb::string_, ndb::scope::group<ndb::databases::group>);
}

// spe db
namespace ndb
{
    template<> struct type_map<string_, scope::database<db>> { using type = type_group_db; };
    template<> struct type_map<string_, scope::database<db2>> { using type = type_group_db2; };
    template<> struct type_map<string_, scope::database<dbgroup2>> { using type = type_group2_db; };
}

TEST(type_map, database_scope)
{
    TYPE_MAP_CHECK(ndb::string_, type_group_db, ndb::scope::database<db>);

    ASSERT_TRUE(( std::is_same_v< ndb::cpp_type_t<ndb::string_, db2>, type_group_db2 > ));
    ASSERT_TRUE(( std::is_same_v< ndb::cpp_type_t<ndb::string_, dbgroup2>, type_group2_db > ));

    ASSERT_TRUE(( std::is_same_v< ndb::cpp_type_t<ndb::string_, dbgroup2>, type_group2_db > ));

    ASSERT_FALSE(( std::is_same_v< ndb::cpp_type_t<ndb::string_, db2>, type_group_db > ));
}

namespace ndb
{
    template<> struct type_map<ndb::int_, scope::database<db>> { using type = scoped_type<db>; };
    template<> struct type_map<ndb::int_, scope::group<ndb::databases::group>> { using type = scoped_type<ndb::databases::group>; };
    template<> struct type_map<ndb::int_, scope::engine<ndb::sqlite>> { using type = scoped_type<ndb::sqlite>; };
    template<> struct type_map<ndb::int_, scope::global> { using type = type_global; };
}

TEST(type_map, scope_priority)
{
    TYPE_MAP_CHECK(ndb::int_, scoped_type<db>, ndb::scope::database<db>);
    TYPE_MAP_CHECK(ndb::int_, scoped_type<ndb::databases::group>, ndb::scope::group<ndb::databases::group>);
    TYPE_MAP_CHECK(ndb::int_, scoped_type<ndb::sqlite>, ndb::scope::engine<ndb::sqlite>);
    TYPE_MAP_CHECK(ndb::int_, type_global, ndb::scope::global);
    TYPE_MAP_CHECK(ndb::int_, int, ndb::scope::system);
}

// custom type, defined after tpye_maps
namespace ndb
{
    // user type
    template<class Database>
    struct custom_type<user_type, Database> : basic_type<user_type, bool_, Database>
    {

    };
}

TEST(type_map, custom_type)
{
    // custom type
    ASSERT_TRUE(( ndb::is_custom_type<user_type, db>::value ));
    ASSERT_TRUE(( std::is_same_v< ndb::ndb_type_t<user_type, db>, ndb::bool_ > ));
    ASSERT_FALSE(( std::is_same_v< ndb::ndb_type_t<user_type, db>, ndb::float_ > ));

    // unregistered user type
    ASSERT_FALSE(( std::is_same_v< ndb::ndb_type_t<unregistered_user_type, db>, ndb::int_ > ));
    ASSERT_FALSE(( ndb::is_custom_type<unregistered_user_type, db>::value ));
}