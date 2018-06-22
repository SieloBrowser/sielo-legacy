#ifndef EXPRESSION_CODE_H_NDB
#define EXPRESSION_CODE_H_NDB

#include <ndb/setup.hpp>

namespace ndb
{
    enum class expr_type_code
    {
        root,
        null,
        //
        keyword,
        value,
        field,
        table,
        //
        op_and,
        op_or,
        op_equal,
        op_assign,
        op_shift_left,
        op_list,
        op_function,
        op_match,
        op_like,
        op_glob
    };

    enum class expr_keyword_code
    {
        all,
        get, set, add, del,
        source,
        condition,
        limit,
        sort, asc, desc,
        group,
        values,
        between,
        // function
        now,
        count,
        min,
        max,
        sum
    };

    template<expr_keyword_code Keyword_code>
    struct keyword_type
    {
        static constexpr auto code = Keyword_code;
    };

    // order is important to add missing clauses
    enum class expr_clause_code
    {
        none       = 0,
        get        = 1,
        set        = 2,
        add        = 4,
        del        = 8,
        source     = 16,
        condition  = 32,
        join       = 64,
        sort       = 128,
        value_list = 256,
        command    = 512
    };

    enum class expr_category_code
    {
        sql = 0,
        bson
    };

    constexpr expr_clause_code operator|(expr_clause_code lhs, expr_clause_code rhs)
    {
        return static_cast<expr_clause_code>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    constexpr expr_clause_code operator&(expr_clause_code lhs, expr_clause_code rhs)
    {
        return static_cast<expr_clause_code>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    template<expr_type_code, expr_category_code>
    struct expr_code
    {
        static constexpr const char* value = " _expr_code_undefined_ ";
    };

    template<expr_keyword_code K, expr_category_code>
    struct keyword_code
    {
        static constexpr const char* value = " _keyword_code_undefined_ ";
    };
} // ndb

#endif // EXPRESSION_CODE_H_NDB