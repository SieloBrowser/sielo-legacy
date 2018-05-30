#ifndef EXPRESSION_H_NDB
#define EXPRESSION_H_NDB

#include <ndb/expression/code.hpp>

//! .tpp include operators to overload them

namespace ndb
{
    struct expression_base {};

    template<class L = void, expr_type_code T = expr_type_code::value, class R = void, expr_clause_code Clause = expr_clause_code::none>
    struct expression : expression_base
    {
        static constexpr auto type = T;
        using Lexpr = L;
        using Rexpr = R;

        const L lhs_;
        const R rhs_;

        inline constexpr expression(const L& lhs, const R& rhs);

        template<class F>
        inline constexpr void eval(F&& f) const;

        template<class Native_expression>
        inline constexpr void make(Native_expression& ne) const;

        template<class F>
        inline static constexpr auto static_eval(F&& f);

        template<expr_clause_code SM_Clause = Clause, int Pass = 0, class Native_expression>
        inline static constexpr void static_make(Native_expression& ne);

        inline static constexpr expr_clause_code clause();
    };
} // ndb

#include <ndb/expression.tpp>

#endif // EXPRESSION_H_NDB