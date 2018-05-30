#ifndef EXPRESSION_BSON_NATIVE_H_NDB
#define EXPRESSION_BSON_NATIVE_H_NDB

#include <ndb/expression/bson/code.hpp>
#include <ndb/expression/native.hpp>
#include <ndb/expression/utility.hpp>

namespace ndb
{
    template<>
    struct native_expression<expr_category_code::bson, expr_type_code::root>
    {
        template<expr_clause_code Clause, class Expr, class Native_expression>
        static constexpr void make(const Expr& expr, Native_expression& ne)
        {
            ne.array_begin("pipeline");


            //ne.document_begin("$project");
            //ne.append("test", 1);
            if constexpr (Expr::clause() == expr_clause_code::get)
            {
                //ne.append("test", 0);
            }
            //ne.document_end();

            //expr.make(ne);

            ne.array_end();
        }
    };

    // table
    template<>
    struct native_expression<expr_category_code::bson, expr_type_code::table>
    {
        template<expr_clause_code Clause, class T, class Native_expression>
        static constexpr void make(const T& expr, Native_expression& ne)
        {

        }
    };

    // field
    template<>
    struct native_expression<expr_category_code::bson, expr_type_code::field>
    {
        template<expr_clause_code Clause, class T, class Native_expression>
        static constexpr void make(const T& expr, Native_expression& ne)
        {

        }
    };

    // value
    template<>
    struct native_expression<expr_category_code::bson, expr_type_code::value>
    {
        template<expr_clause_code Clause, class T, class Native_expression>
        static constexpr void make(const T& expr, Native_expression& ne)
        {

        }
    };
} // ndb

#endif // EXPRESSION_BSON_NATIVE_H_NDB
