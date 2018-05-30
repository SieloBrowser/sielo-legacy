#ifndef EXPRESSION_NATIVE_H_NDB
#define EXPRESSION_NATIVE_H_NDB

#include <ndb/expression/code.hpp>

namespace ndb
{
    //! generic native expression, do nothing by default, specialize it to add scalar expressions
    template<expr_category_code, expr_type_code>
    struct native_expression
    {
        template<class T, expr_clause_code Clause, class Native_expression>
        static constexpr void static_make(Native_expression& ne)
        {
            // add nothing
        }
    };
} // ndb

#endif // EXPRESSION_NATIVE_H_NDB
