#ifndef EXPRESSION_FUNCTION_H_NDB
#define EXPRESSION_FUNCTION_H_NDB

#include <ndb/expression.hpp>
#include <ndb/expression/code.hpp>
#include <ndb/expression/utility.hpp>
#include <ndb/expression/operator.hpp>

namespace ndb
{
    namespace expr_function
    {
        struct count;
        struct now;
    } // expr_function


    // keyword
    template<class... Ts>
    constexpr auto get(const Ts&... t)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::get>();

        if constexpr (sizeof...(Ts) == 0)
        {
            auto expr = ndb::expr_make_keyword<expr_keyword_code::all>();
            return ndb::expression<decltype(keyword), expr_type_code::keyword, decltype(expr), expr_clause_code::get> { keyword, expr };
        }
        else
        {
            // msvc fix : use (( )) to compile
            auto expr = ((ndb::expr_make(t), ...));
            return ndb::expression<decltype(keyword), expr_type_code::keyword, decltype(expr), expr_clause_code::get> { keyword, expr };
        }
    }

    template<class... Ts>
    constexpr auto source(const Ts&... t)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::source>();
        auto expr = (ndb::expr_make(t), ...);
        return ndb::expression<decltype(keyword), expr_type_code::keyword, decltype(expr), expr_clause_code::source> { keyword, expr };
    }

    template<class T>
    constexpr auto filter(const T& t)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::condition>();
        auto expr = ndb::expr_make(t);
        return ndb::expression<decltype(keyword), expr_type_code::keyword, decltype(expr), expr_clause_code::condition> { keyword, expr };
    }

    template<class T>
    constexpr auto match(const T& t, const std::string& match_expr)
    {
        auto expr_field = ndb::expression<T, ndb::expr_type_code::field, void, ndb::expr_clause_code::condition> {};
        auto expr = ndb::expr_make(match_expr);
        return ndb::expression<decltype(expr_field), expr_type_code::op_match, decltype(expr), expr_clause_code::condition> { expr_field, expr };
    }

    template<class T>
    constexpr auto like(const T& t, const std::string& match_expr)
    {
        auto expr_field = ndb::expression<T, ndb::expr_type_code::field, void, ndb::expr_clause_code::condition> {};
        auto expr = ndb::expr_make(match_expr);
        return ndb::expression<decltype(expr_field), expr_type_code::op_like, decltype(expr), expr_clause_code::condition> { expr_field, expr };
    }

    constexpr auto limit(const int& count, const int& offset = 0)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::limit>();
        auto expr = ndb::expression<int, expr_type_code::value>{ count };
        return ndb::expression<decltype(keyword), expr_type_code::keyword, decltype(expr)> { keyword, expr };
    }

    template<class... Ts>
    constexpr auto sort(const Ts&... t)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::sort>();
        auto expr = ((ndb::expr_make(t), ...)); // msvc fix : use (( )) to compile
        return ndb::expression<decltype(keyword), expr_type_code::keyword, decltype(expr)> { keyword, expr };
    }

    template<class T>
    constexpr auto asc(const T& t)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::asc>();
        auto expr = ndb::expression<T, ndb::expr_type_code::field, void, ndb::expr_clause_code::sort> {};
        return ndb::expression<decltype(expr), expr_type_code::null, decltype(keyword)> { expr, keyword };
    }

    template<class T>
    constexpr auto desc(const T& t)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::desc>();
        auto expr = ndb::expression<T, ndb::expr_type_code::field, void, ndb::expr_clause_code::sort> {};
        return ndb::expression<decltype(expr), expr_type_code::null, decltype(keyword)> { expr, keyword };
    }

    template<class T, class N1, class N2>
    constexpr auto range(const T& t, const N1& a, const N2& b)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::between>();
        auto expr_field = ndb::expression<T, ndb::expr_type_code::field, void, ndb::expr_clause_code::condition> {};
        auto expr_values = ndb::expr_make(a) && ndb::expr_make(b);

        auto expr = ndb::expression<decltype(keyword), expr_type_code::null, decltype(expr_values)> { keyword, expr_values };

        return ndb::expression<decltype(expr_field), expr_type_code::null, decltype(expr)> { expr_field, expr };
    }

    // function
    template<class T>
    constexpr auto sum(const T& t)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::sum>();
        auto expr = ndb::expr_make(t);
        return ndb::expression<decltype(keyword), expr_type_code::op_function, decltype(expr)> { keyword, expr };
    }

    template<class T>
    constexpr auto count(const T& t)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::count>();
        auto expr = ndb::expr_make(t);
        return ndb::expression<decltype(keyword), expr_type_code::op_function, decltype(expr)> { keyword, expr };
    }

    template<class T>
    constexpr auto min(const T& t)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::min>();
        auto expr = ndb::expr_make(t);
        return ndb::expression<decltype(keyword), expr_type_code::op_function, decltype(expr)> { keyword, expr };
    }

    template<class T>
    constexpr auto max(const T& t)
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::max>();
        auto expr = ndb::expr_make(t);
        return ndb::expression<decltype(keyword), expr_type_code::op_function, decltype(expr)> { keyword, expr };
    }

    constexpr auto now()
    {
        auto keyword = ndb::expr_make_keyword<expr_keyword_code::now>();
        auto expr = ndb::expr_make();
        return ndb::expression<decltype(keyword), expr_type_code::op_function, decltype(expr)> { keyword, expr };
    }
} // ndb

#endif // EXPRESSION_FUNCTION_H_NDB