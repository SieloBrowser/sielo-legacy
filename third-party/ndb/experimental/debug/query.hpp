#ifndef QUERY_H_NDB
#define QUERY_H_NDB

#include <ndb/setup.hpp>
#include <ndb/expression.hpp>
#include <ndb/engine.hpp>
#include <ndb/result.hpp>
#include <iostream>
#include <string>

namespace ndb
{
    template<class Database, class Engine = typename Database::engine>
    class query
    {
    public:
        constexpr query() {}

        template<class T>
        constexpr auto operator<<(const T& t)
        {
            const auto& engine = ndb::engine<Engine>::get();

            auto expr = ndb::expr_make(t);

            auto e = ndb::expression<decltype(expr), expr_type_code::root, void, expr_clause_code::get> { expr };

            return engine.template exec<Database>(e);
        }

        template<class Expr>
        constexpr auto operator+(const Expr& expr) const
        {
            const auto& engine = ndb::engine<Engine>::get();
            auto e = ndb::expression<Expr, expr_type_code::root, void, expr_clause_code::add> { expr };

            return engine.template exec<Database>(e);
        }

        template<class Expr>
        constexpr auto operator-(const Expr& expr)
        {
            const auto& engine = ndb::engine<Engine>::get();
            auto e = ndb::expression<Expr, expr_type_code::root, void, expr_clause_code::del> { expr };

            return engine.template exec<Database>(e);
        }
    };


    //template<class Database>
    //static constexpr auto query = query_impl<Database>();
} // ndb

namespace ndb
{
    template<class Database, class Engine = typename Database::engine>
    class query_view
    {
    public:
        mutable std::string str_;
        constexpr query_view() {}

        auto view()
        {
            return str_;
        }

        template<class T>
        constexpr auto operator=(const T& t)
        {
            const auto& engine = ndb::engine<Engine>::get();

            auto expr = ndb::expr_make(t);
            auto e = ndb::expression<decltype(expr), expr_type_code::root, void, expr_clause_code::get> { std::move(expr) };

            str_ = ndb::sqlite::to_string(e);
        }

        template<class T>
        constexpr auto operator<<(const T& t)
        {
            const auto& engine = ndb::engine<Engine>::get();

            auto expr = ndb::expr_make(t);
            auto e = ndb::expression<decltype(expr), expr_type_code::root, void, expr_clause_code::get> { std::move(expr) };

            str_ = ndb::sqlite::to_string(e);
        }

        template<class Expr>
        constexpr auto operator+(const Expr& expr) const
        {
            const auto& engine = ndb::engine<Engine>::get();
            auto e = ndb::expression<Expr, expr_type_code::root, void, expr_clause_code::add> { expr };

            str_ = ndb::sqlite::to_string(e);
        }

        template<class Expr>
        constexpr auto operator-(const Expr& expr)
        {
            const auto& engine = ndb::engine<Engine>::get();
            auto e = ndb::expression<Expr, expr_type_code::root, void, expr_clause_code::del> { expr };

            str_ = ndb::sqlite::to_string(e);
        }
    };


    //template<class Database>
    //static constexpr auto query = query_impl<Database>();
} // ndb

#endif // QUERY_H_NDB