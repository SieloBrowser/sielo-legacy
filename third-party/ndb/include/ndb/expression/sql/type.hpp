#ifndef EXPRESSION_SQL_TYPE_H_NDB
#define EXPRESSION_SQL_TYPE_H_NDB

#include <ndb/expression/sql/code.hpp>
#include <ndb/expression/type.hpp>
#include <ndb/expression/utility.hpp>
#include <ndb/expression/deduction.hpp>

namespace ndb
{
    // shift_left (append expressions)
    template<>
    struct expression_type<expr_type_code::op_shift_left, expr_category_code::sql>
    {
        template<class L, class R, expr_clause_code Clause, int Pass, class Native_expression>
        static constexpr void static_make(Native_expression& ne)
        {
            L::template static_make<Clause, Pass>(ne);

            // get << source (no keyword)
            if constexpr (expr_has_clause<R, expr_clause_code::source>
                          && !expr_is_keyword_code<R, expr_keyword_code::source>)
            {
                ne.push_back(keyword_code<expr_keyword_code::source, expr_category_code::sql>::value);
            }

            // get << not source
            if constexpr ( !(static_cast<bool>((int)Clause & (int)expr_clause_code::set) )) // do not deduce for SET
            {
                if constexpr (!expr_has_clause<L, expr_clause_code::source> && !expr_has_clause<R, expr_clause_code::source>)
                {
                    if (!ne.deduced_source)
                    {
                        ne.deduced_source = true;
                        ne.push_back(keyword_code<expr_keyword_code::source, expr_category_code::sql>::value);
                        ne.push_back("T");
                        ne.push_back(deduce_source_id<L>() + 48);
                    }
                }
            }


            // not condition << condition
            if constexpr (!(expr_has_clause<L, expr_clause_code::condition>) && expr_has_clause<R, expr_clause_code::condition>)
            {
                ne.push_back(keyword_code<expr_keyword_code::condition, expr_category_code::sql>::value);
            }

            R::template static_make<Clause, Pass>(ne);
        }
    };

    // assign
    template<>
    struct expression_type<expr_type_code::op_assign, expr_category_code::sql>
    {
        template<class L, class R, expr_clause_code Clause, int Pass, class Native_expression>
        static constexpr void static_make(Native_expression& ne)
        {
            if constexpr (static_cast<bool>((int)Clause & (int)expr_clause_code::set))
            {
                L::template static_make<Clause, Pass>(ne);
                ne.push_back(expr_code<expr_type_code::op_assign, expr_category_code::sql>::value);
                R::template static_make<Clause, Pass>(ne);
            }
            else
            {
                if constexpr (Pass == 0)
                {
                    L::template static_make<Clause, Pass>(ne);
                }
                else R::template static_make<Clause, Pass>(ne);
            }
        }
    };

    // function
    template<>
    struct expression_type<expr_type_code::op_function, expr_category_code::sql>
    {
        template<class L, class R, expr_clause_code Clause, int Pass, class Native_expression>
        static constexpr void static_make(Native_expression& ne)
        {
            L::template static_make<Clause, Pass>(ne);
            ne.push_back('(');
            R::template static_make<Clause, Pass>(ne);
            ne.push_back(')');
        }
    };
} // ndb

#endif // EXPRESSION_SQL_TYPE_H_NDB
