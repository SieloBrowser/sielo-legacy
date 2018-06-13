#ifndef TYPE_H_NDB
#define TYPE_H_NDB

#include <ndb/cx_error.hpp>

#include <chrono>
#include <vector>

namespace ndb
{
    // used to get global type_map
    struct global_database
    {
        using group = void;
        using engine = void;
    };


    struct null_type {};

    // ndb types are proxy types
    namespace types
    {
        struct null_;
        struct int_;
        struct float_;
        struct double_;
        struct string_;
        struct byte_array_;
    } // types
    using namespace ::ndb::types;

    template<class...>
    struct ndb_types;

    using ndb_types_t = ndb_types<int_, float_, double_, string_, byte_array_>;


    // mapping ndb types
    template<class Cpp_type, class Database, class Group = typename Database::group, class Engine = typename Database::engine>
    struct type_map { using type = ncx_error_type(type_map, cx_err_type_map_not_found, Cpp_type, Engine); };

#define ndb_internal_type_map(NDB_TYPE, CPP_TYPE) \
    template<class Database, class Group, class Engine> \
    struct type_map<ndb::types::NDB_TYPE, Database, Group, Engine> { using type = CPP_TYPE; };

    // default type mapping for all configurations
    ndb_internal_type_map(int_, int);
    ndb_internal_type_map(float_, float);
    ndb_internal_type_map(double_, double);
    ndb_internal_type_map(string_, std::string);
    ndb_internal_type_map(byte_array_, std::vector<char>);


    // custom types
    struct not_custom_type;
    // T is a ndb fundamental type
    template<class T, class Database = global_database, class Group = typename Database::group, class Engine = typename Database::engine>
    struct custom_type { using ndb_type = not_custom_type; };

    template<class T, class Database = global_database, class Group = typename Database::group, class Engine = typename Database::engine>
    struct is_custom_type
    {
        static constexpr bool value = !std::is_same_v<typename custom_type<T, Database, Group, Engine>::ndb_type, ndb::not_custom_type>;
    };


    // cpp_type
    template<class T, class Database = global_database, class Group = typename Database::group, class Engine = typename Database::engine>
    struct cpp_type
    {
        using type = typename type_map<T, Database, Group, Engine>::type;
    };
    template<class Ndb_type, class Database = global_database, class Group = typename Database::group, class Engine = typename Database::engine>
    using cpp_type_t = typename cpp_type<Ndb_type, Database, Group, Engine>::type;

    // ndb_type
    namespace internal
    {
        template<class Cpp_type, class Database, class Engine, class Group, class Ndb_types>
        struct ndb_type;

        template<class Cpp_type, class Database, class Engine, class Group>
        struct ndb_type< Cpp_type, Database, Group, Engine, ndb_types<> >
        {
            using type = cx_err_ndb_type_not_found_for<Cpp_type, Database, Group, Engine>;
        };

        template<class Cpp_type, class Ndb_type, class Database, class Engine, class Group, class... Types>
        struct ndb_type<Cpp_type, Database, Group, Engine, ndb_types<Ndb_type, Types...>>
        {
            using tested_type = typename cpp_type<Ndb_type, Database, Group, Engine>::type;
            using type = std::conditional_t<std::is_same_v<Cpp_type, tested_type>, Ndb_type, typename ndb_type< Cpp_type, Database, Group, Engine, ndb_types<Types...> >::type >;
        };
    } // internal

    // ndb_type from cpp_type
    // if Cpp_type is a custom type, use the specified ndb_type by the custom type
    template<class Cpp_type, class Database = global_database, class Group = typename Database::group, class Engine = typename Database::engine>
    struct ndb_type
    {
        using type = std::conditional_t<
        ndb::is_custom_type<Cpp_type, Database, Group, Engine>::value,
        typename custom_type<Cpp_type, Database, Group, Engine>::ndb_type,
        typename internal::ndb_type<Cpp_type, Database, Group, Engine, ndb_types_t>::type
        >;
    };
    template<class Cpp_type, class Database = global_database, class Group = typename Database::group, class Engine = typename Database::engine>
    using ndb_type_t = typename ndb_type<Cpp_type, Database, Group, Engine>::type;


    // ndb_type from T, then cpp_type
    template<class T, class Database = global_database, class Group = typename Database::group, class Engine = typename Database::engine>
    using native_type = cpp_type_t< ndb_type_t<T, Database, Group, Engine>, Database, Group, Engine >;





    // default specialization for custom types
    template<class Database, class Group, class Engine>
    struct custom_type<const char*, Database, Group, Engine>
    {
        using ndb_type = string_;

        static auto encode(const char* t)
        {
            return std::string{t};
        }
    };
    //

    template<class T>
    class line;
    template<class T, class U, class Database, class Group, class Engine>
    struct custom_type<std::chrono::duration<T, U>, Database, Group, Engine>
{
    using ndb_type = double_;

    static double encode(const std::chrono::duration<T, U>& v)
    {
        return v.count();
    }

    template<class R>
    inline static R decode(const double& v)
    {
        return R{ v };
    };
};



} // ndb

#endif // TYPE_H_NDB