#ifndef QUERY_H_NDB
#define QUERY_H_NDB

#include <ndb/setup.hpp>
#include <ndb/expression.hpp>
#include <ndb/engine.hpp>
#include <ndb/result.hpp>
#include <ndb/option.hpp>

namespace ndb
{
    template<class Database, class Option = ndb::option<>>
    class basic_query
    {
    public:
        using Engine = typename Database::engine;

        constexpr basic_query() {}

        template<class T>
        constexpr auto operator<<(const T& t)
        {
            const auto& engine = ndb::engine<Engine>::get();

            auto expr = ndb::expr_make(t);
            auto e = ndb::expression<decltype(expr), expr_type_code::root, void, expr_clause_code::get> { std::move(expr) };

            return engine.template exec<Database, Option>(e);
        }

        template<class T>
        constexpr auto operator>>(const T& t)
        {
            const auto& engine = ndb::engine<Engine>::get();

            auto expr = ndb::expr_make(t);
            auto e = ndb::expression<decltype(expr), expr_type_code::root, void, expr_clause_code::set> { std::move(expr) };

            return engine.template exec<Database, Option>(e);
        }

        template<class Expr>
        constexpr auto operator+(const Expr& expr) const
        {
            const auto& engine = ndb::engine<Engine>::get();
            auto e = ndb::expression<Expr, expr_type_code::root, void, expr_clause_code::add> { expr };

            return engine.template exec<Database, Option>(e);
        }

        template<class Expr>
        constexpr auto operator-(const Expr& expr)
        {
            const auto& engine = ndb::engine<Engine>::get();
            auto e = ndb::expression<Expr, expr_type_code::root, void, expr_clause_code::del> { expr };

            return engine.template exec<Database, Option>(e);
        }
    };

    template<class Database, class Option = ndb::option<>>
    using query = basic_query<Database, Option>;

    template<class Database, class Option = ndb::option<>>
    using oquery = basic_query<Database, ndb::option<ndb::query_option::object>>;


    // utility class to display querys
    template<class Database>
    class query_str
    {
        using Engine = typename Database::engine;
    public:
        constexpr query_str() {}

        template<class T>
        constexpr auto operator=(const T& t)
        {
            const auto& engine = ndb::engine<Engine>::get();

            auto expr = ndb::expr_make(t);
            auto e = ndb::expression<decltype(expr), expr_type_code::root, void, expr_clause_code::get> { std::move(expr) };

            return Engine::to_string(e);
        }

        template<class T>
        constexpr auto operator<<(const T& t)
        {
            const auto& engine = ndb::engine<Engine>::get();

            auto expr = ndb::expr_make(t);
            auto e = ndb::expression<decltype(expr), expr_type_code::root, void, expr_clause_code::get> { std::move(expr) };

            return Engine::to_string(e);
        }

        template<class Expr>
        constexpr auto operator+(const Expr& expr)
        {
            const auto& engine = ndb::engine<Engine>::get();
            auto e = ndb::expression<Expr, expr_type_code::root, void, expr_clause_code::add> { expr };

            return Engine::to_string(e);
        }

        template<class Expr>
        constexpr auto operator-(const Expr& expr)
        {
            const auto& engine = ndb::engine<Engine>::get();
            auto e = ndb::expression<Expr, expr_type_code::root, void, expr_clause_code::del> { expr };

            return Engine::to_string(e);
        }
    };
} // ndb

#endif // QUERY_H_NDB
