#ifndef EXPRESSION_OPERATOR_H_NDB
#define EXPRESSION_OPERATOR_H_NDB

#include <ndb/expression/code.hpp>
#include <ndb/expression/utility.hpp>

namespace ndb
{
    // op_list
    template<class L, class R, class = ndb::enable_expression<L, R>>
    constexpr const auto operator,(const L& l, const R& r)
    {
        auto lhs = ndb::expr_make(l);
        auto rhs = ndb::expr_make(r);
        return ndb::expression<decltype(lhs), ndb::expr_type_code::op_list, decltype(rhs)> { lhs, rhs };
    }

    // op_equal
    template<class L, class R, class = ndb::enable_expression<L, R>>
    constexpr const auto operator==(const L& l, const R& r)
    {
        auto lhs = ndb::expr_make<ndb::expr_clause_code::condition>(l);
        auto rhs = ndb::expr_make(r);
        return ndb::expression<decltype(lhs), ndb::expr_type_code::op_equal, decltype(rhs), ndb::expr_clause_code::condition> { lhs, rhs };
    }

    // op_and
    template<class L, class R, class = ndb::enable_expression<L, R>>
    constexpr const auto operator&&(const L& lhs, const R& rhs)
    {
        return ndb::expression<L, ndb::expr_type_code::op_and, R> { lhs, rhs };
    }

    // op_or
    template<class L, class R, class = ndb::enable_expression<L, R>>
    constexpr const auto operator||(const L& lhs, const R& rhs)
    {
        return ndb::expression<L, ndb::expr_type_code::op_or, R> { lhs, rhs };
    }

    // op_shift_left
    template<class L, class R, class = ndb::enable_expression<L, R>>
    constexpr const auto operator<<(const L& l, const R& r)
    {
        auto lhs = ndb::expr_make(l);
        auto rhs = ndb::expr_make(r);
        return ndb::expression<decltype(lhs), ndb::expr_type_code::op_shift_left, decltype(rhs)> { lhs, rhs };
    }
} // ndb

#endif // EXPRESSION_OPERATOR_H_NDB
