#ifndef EXPRESSION_DEDUCE_H_NDB
#define EXPRESSION_DEDUCE_H_NDB

#pragma warning(push, 0)

#include <kvasir/mpl/algorithm/filter.hpp>
#include <kvasir/mpl/algorithm/transform.hpp>
#include <kvasir/mpl/algorithm/all.hpp>
#include <kvasir/mpl/sequence/join.hpp>
#include <kvasir/mpl/sequence/front.hpp>
#include <kvasir/mpl/sequence/size.hpp>
#include <kvasir/mpl/types/int.hpp>
#include <kvasir/mpl/types/list.hpp>
#include <kvasir/mpl/functional/call.hpp>
#include <kvasir/mpl/functional/fork.hpp>
#include <kvasir/mpl/functions/comparison/equal.hpp>
#include <kvasir/mpl/functions/logical/logical_or.hpp>

#pragma warning(pop)

#include <ndb/expression.hpp>
#include <ndb/cx_error.hpp>

namespace mpl = kvasir::mpl;

namespace ndb::internal
{
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
} // ndb::internal

struct error
{
    using table = int;
};

template<class T>
struct table_bot_found
{
    using value_type = error;
    static constexpr auto value = 0;
};

namespace ndb
{
    template<class Expr>
    struct deduce
    {
        using linear_type = internal::linearize<Expr>;
        using tables = mpl::eager::filter<linear_type, internal::expr_is_table>;

        // must be expr<...>, fail if pack is empty
        using main_table = typename
        mpl::call<
            mpl::unpack<
                mpl::filter<
                    mpl::fork<
                        mpl::cfe<internal::expr_is_field>,
                        mpl::cfe<internal::expr_is_table>,
                        mpl::logical_or<>
                    >,
                    mpl::front<>
                >
            >
        , linear_type
        >::table;
    };
} // ndb

#endif // EXPRESSION_DEDUCE_H_NDB

