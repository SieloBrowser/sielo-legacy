#ifndef UTILITY_H_NDB
#define UTILITY_H_NDB

//! \brief utility functions

#include <ndb/fwd.hpp>
#include <ndb/cx_error.hpp>
#include <ndb/engine/type.hpp>
#include <ndb/expression/code.hpp>
#include <ndb/scope.hpp>

#include <utility>

#ifdef __GNUC__
    #include <cxxabi.h>
#endif

namespace ndb
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////             GENERIC            ////////////////////////
////////////////////////////////////////////////////////////////////////////////
    // compile time str _len
    constexpr size_t cx_str_len(const char* str)
    {
        return *str ? 1 + cx_str_len(str + 1) : 0;
    }

    namespace internal
    {
        template <class T, class = void>
        struct is_complete : std::false_type{};

        template <class T>
        struct is_complete<T, decltype(void(sizeof(T)))> : std::true_type{};

        // T is a complete type and base_of T
        template<class Base, class T, class = void>
        struct is_base_of : std::false_type {};

        template<class Base, class T>
        struct is_base_of<Base, T, std::enable_if_t<is_complete<T>::value>> : std::conditional_t<std::is_base_of_v<Base, T>, std::true_type, std::false_type> {};
    } // internal

    // index_of
    template<class T, class Container>
    struct index_of;

    template<class T, template<class...> class Container>
    struct index_of<T, Container<>>
    {
        constexpr static auto value = ncx_error_value(index_of, cx_err_type_not_found, T, Container);
    };

    template<class T, template<class...> class Container, class... Ts>
    struct index_of<T, Container<T, Ts...>>
    {
        constexpr static auto value = 0;
    };

    template<class T, template<class...> class Container, class T_test, class... Ts>
    struct index_of<T, Container<T_test, Ts...>>
    {
        constexpr static auto value = 1 + index_of<T, Container<Ts...>>::value;
    };

    namespace internal
    {
        template<class Needle, class Haystack>
        struct has_type;

        template<class Needle, template<class...> class Haystack, class T1, class... T>
        struct has_type<Needle, Haystack<T1, T...>> { static constexpr bool value = has_type<Needle, Haystack<T...>>::value; };

        template<class Needle, template<class...> class Haystack>
        struct has_type<Needle, Haystack<>> : std::false_type {};

        template<class Needle, template<class...> class Haystack, class... T>
        struct has_type<Needle, Haystack<Needle, T...>> : std::true_type {};
    } // internal

    template<class Needle, class Haystack>
    constexpr auto has_type_v = internal::has_type<Needle, Haystack>::value;

    struct void_{};
    template<class F, class... Args>
    constexpr auto call(F&& f, Args... args)
    {
        if constexpr (std::is_void_v<decltype(f(args...))>)
        {
            return void_{};
        }
        else return f(std::forward<Args>(args)...);
    }

////////////////////////////////////////////////////////////////////////////////
////////////////////////              NDB               ////////////////////////
////////////////////////////////////////////////////////////////////////////////
    template<class T>
    static constexpr bool is_table = internal::is_base_of<ndb::table_base, T>::value;

    template<class T>
    static constexpr bool is_field =  internal::is_base_of<ndb::field_base, T>::value;

    template<class T>
    static constexpr bool is_field_entity = internal::is_base_of<ndb::table_base, typename T::value_type>::value;

    template<class T>
    static constexpr bool is_field_entity_vector = is_field_entity<T> && (T{}.detail_.size == 0);

    template<class T>
    static constexpr bool is_database =  internal::is_base_of<ndb::database_base, T>::value;

    template<class T>
    static constexpr bool is_option = internal::is_base_of<ndb::option_base, T>::value;

    template<class T>
    static constexpr bool is_expression = internal::is_base_of<ndb::expression_base, T>::value;

    template<class Field>
    constexpr unsigned char field_id = index_of<std::decay_t<Field>, typename Field::table::Detail_::entity>::value;

    template<class Table>
    constexpr unsigned char table_id = index_of<std::decay_t<Table>, typename Table::model::Detail_::entity>::value;

    template<class Database>
    constexpr unsigned char database_id = index_of<std::decay_t<Database>, typename Database::group::Detail_::entity>::value;

/////////////////////////////////////////////////////////////////////////////////
////////////////////////           EXPRESSION            ////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//TODO wrap expression enum to type for easier traits (exrp_type::field class)
    namespace internal
    {
        template<class Expr, expr_type_code T>
        struct expr_type_check { static constexpr auto value = Expr::type == T; };

        template<class Expr>
        struct expr_is_value :
            std::conditional_t<is_expression<Expr>, expr_type_check<Expr, expr_type_code::value>, std::false_type>{};

        template<class Expr>
        struct expr_is_field :
            std::conditional_t<is_expression<Expr>, expr_type_check<Expr, expr_type_code::field>, std::false_type>{};

        template<class Expr>
        struct expr_is_table :
            std::conditional_t<is_expression<Expr>, expr_type_check<Expr, expr_type_code::table>, std::false_type>{};

        template<class Expr>
        struct expr_is_scalar
        {
            static constexpr bool value = expr_is_field<Expr>::value || expr_is_value<Expr>::value || expr_is_table<Expr>::value;
        };

        template<class Expr, expr_keyword_code Keyword_code>
        constexpr bool expr_is_keyword_code()
        {
            // is a keyword
            if constexpr (Expr::type == expr_type_code::keyword)
            {
                // keyword_code match
                if constexpr (Expr::Lexpr::keyword_code == Keyword_code) return true;
                else return false;
            }
                // not a keyword
            else return false;
        };
    } // internal

    template<class Expr> constexpr bool expr_is_value = internal::expr_is_value<Expr>::value;
    template<class Expr> constexpr bool expr_is_field = internal::expr_is_field<Expr>::value;
    template<class Expr> constexpr bool expr_is_table = internal::expr_is_table<Expr>::value;
    template<class Expr> constexpr bool expr_is_scalar = internal::expr_is_scalar<Expr>::value;

    template<class Expr, expr_keyword_code Keyword_code>
    constexpr bool expr_is_keyword_code = internal::expr_is_keyword_code<Expr, Keyword_code>();

    template<class Expr, expr_clause_code Clause>
    constexpr bool expr_has_clause = static_cast<bool>(Expr::clause() & Clause);

    template<expr_clause_code Check, expr_clause_code Clause>
    constexpr bool expr_has_clause_value = static_cast<bool>(Check & Clause);

////////////////////////////////////////////////////////////////////////////////
////////////////////////            FOR_EACH            ////////////////////////
////////////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        // call f for each type
        template<class... Ts, std::size_t... Ns, class F>
        void for_each_impl(std::index_sequence<Ns...>, F&& f)
        {
            using expand = int[];
            (void)expand{1, (std::forward<F>(f)(std::integral_constant<std::size_t, Ns>{}, Ts{}), 0)...};
        }

        // call f for each table or field
        template<class Entity, std::size_t... Ns, class F>
        void for_each_entity_impl(std::index_sequence<Ns...>&&, F&& f)
        {
            using expand = int[];
            (void)expand{1, ((void)std::forward<F>(f)(std::integral_constant<std::size_t, Ns>{}, typename Entity::template item_at<Ns>{}), 0)...};
        }

        // call f for each arg
        template<std::size_t... N, class F, class... Ts>
        void for_each_impl(std::index_sequence<N...>, F&& f, Ts&&... args)
        {
            using expand = int[];
            (void)expand{((void)std::forward<F>(f)(std::integral_constant<std::size_t, N>{}, std::forward<Ts>(args)), 0)...};
        }
    } // detail

    // for each on pack type
    template<class... Ts, class F>
    void for_each(F&& f)
    {
        detail::for_each_impl<Ts...>(std::index_sequence_for<Ts...>{}, std::forward<F>(f));
    }

    // for_each on model entity
    template<class DB_Entity, class F>
    void for_each_entity(DB_Entity&& e, F&& f)
    {
        using Entity = typename std::decay_t<DB_Entity>::Detail_::entity;
        using Ns = std::make_index_sequence<Entity::count()>;
        detail::for_each_entity_impl<Entity>(Ns{}, std::forward<F>(f));
    }

    // for each on model entity
    template<class DB_Entity, class F>
    void for_each_entity(F&& f)
    {
        for_each_entity(DB_Entity{}, std::forward<F>(f));
    }

    // for each on arguments
    template<class F, class... Ts>
    void for_each(F&& f, Ts&&... args)
    {
        detail::for_each_impl(std::index_sequence_for<Ts...>{}, std::forward<F>(f), std::forward<Ts>(args)...);
    }
////////////////////////////////////////////////////////////////////////////////
////////////////////////             TYPES              ////////////////////////
////////////////////////////////////////////////////////////////////////////////
    template<class T>
    struct is_proxy_type { static constexpr bool value = std::is_base_of<ndb::proxy_type_base, T>::value; };

    // type is native for the engine (no storage type mapping)
    template<class Engine, class T>
    struct is_native_type { static constexpr bool value = is_proxy_type<T>::value
                                                           && std::is_same_v<T, ndb::storage_type_t<Engine, T>>; };

    template<class Engine, class T>
    struct is_storage_type { static constexpr bool value = is_proxy_type<T>::value
                                                           && !std::is_same_v<T, ndb::storage_type_t<Engine, T>>; };
    template<class Engine, class T>
    static constexpr bool is_storage_type_v = is_storage_type<Engine, T>::value;

    template<class T, class Database>
    struct is_custom_type { static constexpr bool value = !std::is_same<ndb::internal::custom_type_not_found, typename ndb::custom_type<T, Database>::ndb_type>::value; };

    template<class T, class Database>
    static constexpr auto is_custom_type_v = is_custom_type<T, Database>::value;


    template<class T>
    std::string type_str()
    {
        std::string output;
        #ifdef __GNUC__
            output = abi::__cxa_demangle(typeid(T).name(), 0, 0, 0);
        #else
            output = typeid(T).name();
        #endif

        return output;
    }

////////////////////////////////////////////////////////////////////////////////
////////////////////////             NAMES              ////////////////////////
////////////////////////////////////////////////////////////////////////////////
    template<class T>
    std::string name()
    {
        if constexpr (ndb::is_field<T>) return "F" + std::to_string(ndb::field_id<T>);
        else if constexpr (ndb::is_table<T>) return "T" + std::to_string(ndb::table_id<T>);
        else if constexpr (ndb::is_database<T>) return std::string(T::group::name) + "_D" + std::to_string(ndb::database_id<T>);
    }

    template<class T>
    std::string name(const T&)
    {
        return name<T>();
    }
} // ndb

#endif // UTILITY_H_NDB