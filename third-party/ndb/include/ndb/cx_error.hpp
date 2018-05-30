#ifndef CX_ERROR_H_NDB
#define CX_ERROR_H_NDB

// \brief compile time error

#define ncx_error(Source, Err, ...) ndb::cx_error<Source, Err< __VA_ARGS__ > >::value;
#define ncx_error_type(Source, Err, ...) typename ndb::cx_error<Source, Err< __VA_ARGS__ > >::type;

namespace ndb
{
    struct cx_err;

    template<class Source, class Err>
    struct cx_error_occured;

    template<template<class...> class Source, class Err>
    struct cx_error_occured_tpl;

    template<class Source, class Err>
    struct cx_error
    {
        static constexpr auto value = cx_error_occured<Source, Err>{};
    };

    template<template<class...> class Source, class Err, class... Ts>
    struct cx_error<Source<Ts...>, Err>
    {
        static constexpr auto value = cx_error_occured_tpl<Source, Err>{};
    };

    // cx errors
    //! no type_map found for Cpp_type, add a type_map specialization
    template<class Cpp_type, class Engine>
    struct cx_err_type_map_not_found;

    //! type_map for Cpp_type exists but mapped ndb_type is not found in ndb_types
    template<class Cpp_type, class Database, class Engine = typename Database::engine, class Group = typename Engine::group>
    struct cx_err_ndb_type_not_found_for;


    template<class T, template<class...> class Container>
    struct cx_err_type_not_found;

    template<class...>
    struct cx_err_not_implemented;
} // ndb

#endif // CX_ERROR_H_NDB
