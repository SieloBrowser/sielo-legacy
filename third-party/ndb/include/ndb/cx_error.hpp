#ifndef CX_ERROR_H_NDB
#define CX_ERROR_H_NDB

// \brief compile time error
namespace ndb::internal
{
    template<class T> struct false_ { static constexpr bool value = false; };
    template<class T> static constexpr bool false_v = false_<T>::value;
} // ndb::internal

#define ncx_error(T, Message) static_assert(ndb::internal::false_v<T>, Message);
#define ncx_error_not_implemented(T) static_assert(ndb::internal::false_v<T>, "no implementation");

#define ncx_error_value(Source, Err, ...) ndb::cx_error_value<Source, Err< __VA_ARGS__ > >::value
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
    struct cx_error_value { static constexpr auto value = cx_error_occured<Source, Err>{}; };

    template<template<class...> class Source, class Err, class... Ts>
    struct cx_error_value<Source<Ts...>, Err> { static constexpr auto value = cx_error_occured_tpl<Source, Err>{}; };

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
} // ndb

#endif // CX_ERROR_H_NDB
