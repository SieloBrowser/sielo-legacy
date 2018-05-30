#include <ndb/initializer.hpp>
#include <ndb/engine/sqlite/sqlite.hpp>


#include <iostream>
#include <sstream>

#include "database.hpp"

#include <kvasir/mpl/algorithm/filter.hpp>
#include <kvasir/mpl/algorithm/transform.hpp>
#include <kvasir/mpl/algorithm/extreme.hpp>
#include <kvasir/mpl/functional/call.hpp>
#include <kvasir/mpl/sequence/join.hpp>
#include <kvasir/mpl/sequence/front.hpp>
#include <kvasir/mpl/types/int.hpp>
#include <kvasir/mpl/types/list.hpp>
#include <kvasir/mpl/functions/arithmetic/plus.hpp>
namespace ndb
{
    template<class Expr>
    struct expr_is_field_t :
        std::conditional_t<Expr::type == expr_type_code::field, std::true_type, std::false_type>{};

    template<class Expr>
    struct expr_is_table_t :
        std::conditional_t<Expr::type == expr_type_code::table, std::true_type, std::false_type>{};
    template<class Table>
    constexpr unsigned char table_id_str = table_id<Table> + 48;
}





constexpr auto movie = models::library.movie;

namespace mpl = kvasir::mpl;

template<class Expr>
struct linearize_impl
{
    using make = mpl::list<>;
};

template<class Expr>
using linearize = typename linearize_impl<Expr>::make;

template<class Expr>
using linearize_rec_impl =
typename mpl::conditional<ndb::expr_is_scalar<Expr>>
::template f<mpl::listify, mpl::cfe<linearize>>
::template f<Expr>
;

template<class L, ndb::expr_type_code T, class R, ndb::expr_clause_code Clause>
struct linearize_impl<ndb::expression<L, T, R, Clause>>
{
    using make = typename mpl::join<>::template f<
    linearize_rec_impl<L>,
    linearize_rec_impl<R>
    >;
};


template<class T>
struct expr_size
{
    static constexpr auto value = 0;
};

template<class L, class R, ndb::expr_clause_code Clause>
struct expr_size<ndb::expression<L, ndb::expr_type_code::field, R, Clause>>
{
    static constexpr auto value = 2;
};

template<class L, class R, ndb::expr_clause_code Clause>
struct expr_size<ndb::expression<L, ndb::expr_type_code::table, R, Clause>>
{
    static constexpr auto value = 10;
};

template<class Expr>
struct cx_expr
{
    using linear_type = linearize<Expr>;
    using tables = mpl::eager::filter<linear_type, ndb::expr_is_table_t>;

    using main_table = typename
    mpl::call
    <
    mpl::unpack
    <
    mpl::filter<mpl::cfe<ndb::expr_is_field_t>, mpl::front<>>
    >
    , linear_type
    >::value_type::table;

    static constexpr auto size =
                          mpl::call
                          <
                          mpl::unpack
                          <
                          mpl::transform
                          <
                          mpl::cfe<expr_size>, mpl::extreme<mpl::plus<> >
    >
    >
    , linear_type
    >::value;
};


int main()
{
    using ndb::sqlite;

    std::stringstream result;

    try
    {
        ndb::initializer<sqlite> init;
        ndb::connect<dbs::zeta, sqlite>();


        const auto& user = models::library.user;


        auto expr = (movie.id, movie.name) << (movie);

        using Expr = decltype(expr);

        //auto realname = abi::__cxa_demangle(typeid( cx_expr<Expr>::size ).name(), 0, 0, 0);
        auto realname = typeid( cx_expr<Expr>::size ).name();
        std::cout << "\n" << realname;
        std::cout << "\n main_table" << ndb::table_id_str<cx_expr<Expr>::main_table>;
        std::cout << "\n size " << cx_expr<Expr>::size;


    } catch(const std::exception& e)
    {
        std::cout << e.what();
    }


    return 0;
}
















/*
#include <iostream>
#include <cxxabi.h>
#include <tuple>

struct op_value;
struct op_field;
struct op_table;
struct op_and;

template<class> struct field { using table = void; };
template<class> struct table;
template<int N> struct size { static constexpr auto value = N; };

template<class T>
struct param;

struct void_
{
    using linearize = void;
};

template<class...>
struct fields{};

template<class T, class... Ts>
struct fields<field<T>, fields<Ts...>>
{
    using pack = fields<T, Ts...>;
};
template<class... T, class... Ts>
struct fields<fields<T...>, fields<Ts...>>
{
    using pack = fields<T..., Ts...>;
};

template<class... Ts>
struct tables { };

template<class T, class... Ts>
struct tables<table<T>, tables<Ts...>>
{
    using pack = tables<T, Ts...>;
};
template<class... Ts>
struct tables<void, tables<Ts...>>
{
    using pack = tables<Ts...>;
};

template<class... T, class... Ts>
struct tables<tables<T...>, tables<Ts...>>
{
    using pack = tables<T..., Ts...>;
};

//
template<class Fs, class Ts, class S>
struct result
{
    using linearize = result<Fs, Ts, S>;

    using Fields = Fs;
    using Tables = Ts;
    using Size = S;
};


template<class L, class R, class S>
struct result<field<L>, table<R>, S>
{
    using Fields = fields<L>;
    using Tables = tables<R>;

    using linearize = result<fields<L>, tables<R, typename L::table>, size<S::value + R::size::value + L::size::value>>;
};


template<class L, class S, class... R>
struct result<table<L>, result<R...>, S>
{
    using Fields = typename result<R...>::Fields;
    using Tables = typename tables<table<L>, typename result<R...>::Tables>::pack;
    using linearize = result<Fields, Tables, size<5>>;
};


template<class L, class S, class... R>
struct result<field<L>, result<R...>, S>
{
    using Fields = typename fields<field<L>, typename result<R...>::Fields>::pack;
    using Tables = typename tables<typename field<L>::table, typename result<R...>::Tables>::pack;
    using linearize = result<Fields, Tables, size<5>>;
};


template<class S, class... L, class... R>
struct result<result<L...>, result<R...>, S>
{
    using Fields = typename fields<typename result<L...>::Fields, typename result<R...>::Fields>::pack;
    using Tables = typename tables<typename result<L...>::Tables, typename result<R...>::Tables>::pack;
    using linearize = result<Fields, Tables, size<5>>;
};


struct T {using size = ::size<1>; using linearize = table<T>; };
struct T2 {using size = ::size<1>; using linearize = table<T2>; };
struct T3 {using size = ::size<1>; using linearize = table<T3>; };

struct F{  using size = ::size<2>; using linearize = field<F>; using table = T; };
struct F2 { using size = ::size<2>; using linearize = field<F2>; using table = T3; };

template<class L, class T = op_value, class R = void_>
struct expr
{
    using linearize = typename result<typename L::linearize, typename R::linearize, size<0>>::linearize;
};

template<class L>
struct expr<L, op_field, void_>
{
    using linearize = field<L>;
};

template<class L>
struct expr<L, op_table, void_>
{
    using linearize = table<L>;
};


int main()
{
    using Expr_F_T =
    expr<
        expr<F2, op_field>,
            op_and,
        expr<T2, op_table>
    >;

    using Linearized = typename Expr_F_T::linearize;

    auto realname = abi::__cxa_demangle(typeid(Linearized).name(), 0, 0, 0);
    std::cout << realname;

    std::cout << "\n_" << Linearized::Size::value;

    return 0;
}
 */