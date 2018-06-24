#ifndef TYPE_MAP_H_NDB
#define TYPE_MAP_H_NDB

#include <ndb/cx_error.hpp>
#include <ndb/scope.hpp>
#include <type_traits>

namespace ndb
{
    #define ndb_bijective_type_map(NDB_TYPE, CPP_TYPE, SCOPE) \
    template<> struct type_map<NDB_TYPE, SCOPE> { using type = CPP_TYPE; }; \
    template<> struct type_map<CPP_TYPE, SCOPE> { using type = NDB_TYPE; }

    // not found type
    namespace internal
    {
        struct type_map_not_found;
    }

    template<class T, class Scope = scope::global>
    struct type_map
    {
        using type = internal::type_map_not_found;
    };

    namespace internal
    {
        // get type_map, if Database is specified, search in Database group and engine scopes
        template<class T, class Scope, class Database>
        struct type_map_get_impl
        {
            using tested_type = typename type_map<T, Scope>::type;
            using type_not_found = std::is_same<tested_type, type_map_not_found>;

            using type = std::conditional_t<type_not_found::value,
            typename type_map_get_impl<T, typename internal::next_scope<Database, Scope>::type, Database>::type,
            tested_type
            >;
        };

        template<class T, class Database>
        struct type_map_get_impl<T, internal::scope_not_found, Database>
        {
            using type = ncx_error_type(type_map_get_impl, cx_err_type_map_not_found, T);
        };

        // get type map
        template<class T, class Scope>
        struct type_map_get
        {
            using type = typename type_map_get_impl<T, Scope, void>::type;
        };

        // get type map using database scopes
        template<class T, class Database>
        struct type_map_get<T, scope::database<Database>>
        {
            using type = typename type_map_get_impl<T, scope::database<Database>, Database>::type;
        };

    } // internal
} // ndb

#endif // TYPE_MAP_H_NDB