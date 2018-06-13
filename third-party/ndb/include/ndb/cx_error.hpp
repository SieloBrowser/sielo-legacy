#ifndef CX_ERROR_H_NDB
#define CX_ERROR_H_NDB

// \brief compile time error

#define ncx_error(Source, Err, ...) ndb::cx_error<Source, Err< __VA_ARGS__ > >::value
#define ncx_error_type(Source, Err, ...) typename ndb::cx_error_type<Source, Err< __VA_ARGS__ > >::type

namespace ndb
{
    template<class...>
    struct cx_err;

    template<class Source, class Err>
    struct cx_error_occured;

    template<template<class...> class Source, class Err>
    struct cx_error_occured_tpl;

    template<class Source, class Err>
    struct cx_error { static constexpr auto value = cx_error_occured<Source, Err>{}; };

    template<template<class...> class Source, class Err, class... Ts>
    struct cx_error<Source<Ts...>, Err> { static constexpr auto value = cx_error_occured_tpl<Source, Err>{}; };

    template<class Source, class Err>
    struct cx_error_type { using type = cx_error_occured<Source, Err>; };

    template<template<class...> class Source, class Err, class... Ts>
    struct cx_error_type<Source<Ts...>, Err> { using type = cx_error_occured_tpl<Source, Err>; };

    // cx errors
    //! no type_map found for T, add a type_map specialization
    template<class T>
    struct cx_err_type_map_not_found;

    template<class T>
    struct cx_err_type_unknown;

    template<class T, template<class...> class Container>
    struct cx_err_type_not_found;

    template<class...>
    struct cx_err_not_implemented;
} // ndb

#endif // CX_ERROR_H_NDB
