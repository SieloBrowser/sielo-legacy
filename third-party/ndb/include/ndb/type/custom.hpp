#ifndef TYPE_CUSTOM_H_NDB
#define TYPE_CUSTOM_H_NDB

#include <ndb/scope.hpp>
#include <ndb/type/map.hpp>

namespace ndb
{
    #define ndb_custom_type(NDB_TYPE, ...) \
    struct custom_type<__VA_ARGS__> : basic_type<__VA_ARGS__, NDB_TYPE>

    // not found type
    namespace internal { struct custom_type_not_found; }

    template<class T, class Database>
    struct custom_type
    {
        // used to check if T is custom
        using ndb_type = internal::custom_type_not_found;

        static double encode(const T& v);
        static T decode(const T& v);
    };

    template<class T, class Ndb_type, class Database>
    struct basic_type
    {
        using type = T;
        using ndb_type = Ndb_type;
        using cpp_type = typename internal::type_map_get<ndb_type, scope::database<Database>>::type;

        static cpp_type internal_encode(const T& v)
        {
            return custom_type<T, Database>::encode(v);
        }

        static T internal_decode(const cpp_type& v)
        {
            return custom_type<T, Database>::decode(v);
        };

        // default decode
        static T decode(const cpp_type& v)
        {
            return T{ v };
        };
    };
} // ndb

#endif // TYPE_CUSTOM_H_NDB