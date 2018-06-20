#ifndef TYPE_H_NDB
#define TYPE_H_NDB

#include <ndb/cx_error.hpp>
#include <ndb/type/custom.hpp>
#include <ndb/type/map.hpp>
#include <ndb/scope.hpp>
#include <ndb/utility.hpp>

namespace ndb
{
    struct null_type {};
    struct proxy_type_base {};

    // ndb types are proxy types
    namespace types
    {
        struct null_ : proxy_type_base{};
        struct custom_ : proxy_type_base{};
        struct bool_ : proxy_type_base{};
        struct int_ : proxy_type_base{};
        struct int64_ : proxy_type_base{};
        struct float_ : proxy_type_base{};
        struct double_ : proxy_type_base{};
        struct string_ : proxy_type_base{};
        struct datetime_ : proxy_type_base{};
        struct byte_array_ : proxy_type_base{};
    } // types
    using namespace ::ndb::types;

    template<class...>
    struct ndb_types;

    using ndb_types_t = ndb_types
    <
        bool_,
        byte_array_,
        datetime_,
        double_,
        float_,
        int_,
        int64_,
        null_,
        string_
    >;

    // get a type from a scope
    template<class T, class Scope = scope::global>
    using type_get = typename internal::type_map_get<T, Scope>::type;

    // cpp_type
    template<class Ndb_type, class Database>
    struct cpp_type { using type = typename internal::type_map_get<Ndb_type, scope::database<Database>>::type; };

    template<class Ndb_type, class Database>
    using cpp_type_t = typename cpp_type<Ndb_type, Database>::type;

    // ndb_type
    template<class Cpp_type, class Database>
    struct ndb_type
    {
        using type = std::conditional_t<
        ndb::is_custom_type<Cpp_type, Database>::value,
        typename custom_type<Cpp_type, Database>::ndb_type,
        typename internal::type_map_get<Cpp_type, scope::database<Database>>::type
        >;
    };

    template<class Cpp_type, class Database>
    using ndb_type_t = typename ndb_type<Cpp_type, Database>::type;

    // construct a type from generic params
    template<class T, class... Args>
    inline auto type_make(Args... args)
    {
        return T{ std::forward<Args>(args)... };
    };
} // ndb

#include <ndb/type/system.tpp>

#endif // TYPE_H_NDB