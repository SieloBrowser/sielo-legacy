#ifndef EXPRESSION_TYPE_NDB
#define EXPRESSION_TYPE_NDB

#include <ndb/expression/code.hpp>

namespace ndb
{
    // default make for expression_type <L T R>
    template<expr_type_code T, expr_category_code Ec>
    struct expression_type
    {
        template<class L, class R, expr_clause_code Clause, int Pass, class Native_expression>
        static constexpr void static_make(Native_expression& ne)
        {
            L::template static_make<Clause, Pass>(ne);
            ne.push_back(expr_code<T, Ec>::value);
            R::template static_make<Clause, Pass>(ne);
        }
    };
} // ndb

#endif // EXPRESSION_TYPE_NDB
