#ifndef EXPRESSION_UTILITY_H_NDB
#define EXPRESSION_UTILITY_H_NDB

#include <ndb/utility.hpp>

namespace ndb
{
    template<class L, class R>
    using enable_expression = std::enable_if_t<ndb::is_expression<L> || ndb::is_expression<R>
                                               || ndb::is_field<L> || ndb::is_field<R>
                                               || ndb::is_table<L> || ndb::is_table<R>
    >;

    // make void expression
    template<expr_clause_code Clause = expr_clause_code::none>
    constexpr auto expr_make()
    {
        return ndb::expression<void, expr_type_code::null>();
    }

    // make keyword expression
    template<expr_keyword_code Keyword_code>
    constexpr auto expr_make_keyword()
    {
        return ndb::expression<keyword_type<Keyword_code>, expr_type_code::keyword>{};
    }

    template<expr_clause_code Clause = expr_clause_code::none, class T>
    constexpr auto expr_make(const T& v)
    {
        if constexpr (ndb::is_expression<T>) return v;
        else if constexpr (ndb::is_field<T>)
        {
            if constexpr (Clause == expr_clause_code::none)
            {
                return ndb::expression<T, ndb::expr_type_code::field, void, ndb::expr_clause_code::get> {};
            }
            else return ndb::expression<T, ndb::expr_type_code::field, void, Clause> {};
        }
        else if constexpr (ndb::is_table<T>) return ndb::expression<T, ndb::expr_type_code::table, void, ndb::expr_clause_code::source> {};
        // return value expression
        else return ndb::expression<T> { v };
    }
} // ndb

#endif // EXPRESSION_UTILITY_H_NDB