/*! expression<Left_expression, expression_type ( [operators, field, value] ), Right_expression>
 * expression::type : [operator, field, value, function]
 * expression::clause : get, condition
 *
 * Void expression :
 * expression<>
 *
 * Scalar types :
 * expression<Encapsuled_type, Scalar_type>
 *
 * Process of static_make to generate a Native_expression:
 * Given expression<L, Type, R>
 * call expression<L, Type, R>::static_make
 * call expression_type<Type, Native_expression::expr_category>::static_make(lhs_, rhs_, ne);
 * Default for all Type of expression :
 *          L::template static_make<Pass>(ne);
 *          ne.push_back(expr_code<T, expr_category_code::sql>::value);
 *          R::template static_make<Pass>(ne);
 *
 * static_make can be specialized for Type
 * recursion stop on scalar expressions (specialized expressions by expression_type) ex : expression<T, spe, ...>
 *
 * scalar expressions call :
 * native_expression<Native_expression::expr_category, Type>::static_make<Expr, Clause>(ne);
 *
 * to append the expression to the Native_expression object
 */

#include <ndb/expression/scalar.hpp>
#include <ndb/expression/operator.hpp>
#include <ndb/expression/type.hpp>

namespace ndb
{
    template<class L, expr_type_code T, class R, expr_clause_code Clause>
    constexpr expression<L, T, R, Clause>::expression(const L& lhs, const R& rhs) :
        lhs_(lhs),
        rhs_(rhs)
    {}

    template<class L, expr_type_code T, class R, expr_clause_code Clause>
    constexpr expr_clause_code expression<L, T, R, Clause>::clause()
    {
        return L::clause() | R::clause() | Clause;
    }

    template<class L, expr_type_code T, class R, expr_clause_code Clause>
    template<class F>
    constexpr void expression<L, T, R, Clause>::eval(F&& f) const
    {
        lhs_.eval(f);
        rhs_.eval(f);
    }

    template<class L, expr_type_code T, class R, expr_clause_code Clause>
    template<class F>
    constexpr auto expression<L, T, R, Clause>::static_eval(F&& f)
    {
        L::static_eval(f);
        R::static_eval(f);
    }

    template<class L, expr_type_code T, class R, expr_clause_code Clause>
    template<class Native_expression>
    constexpr void expression<L, T, R, Clause>::make(Native_expression& ne) const
    {
        expression_type<T, Native_expression::expr_category>::make(lhs_, rhs_, ne);
    }

    template<class L, expr_type_code T, class R, expr_clause_code Clause>
    template<expr_clause_code SM_Clause, int Pass, class Native_expression>
    constexpr void expression<L, T, R, Clause>::static_make(Native_expression& ne)
    {
        expression_type<T, Native_expression::expr_category>::template static_make<L, R, SM_Clause, Pass>(ne);
    }
} // ndb