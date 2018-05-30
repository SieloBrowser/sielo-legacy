#ifndef EXPRESSION_SCALAR_H_NDB
#define EXPRESSION_SCALAR_H_NDB

#include <ndb/expression/code.hpp>
#include <ndb/expression/native.hpp>

namespace ndb
{
    //! common code to all scalar expressions
    template<class T, expr_type_code Expression_type, class R, expr_clause_code Clause>
    struct scalar_expression : public expression_base
    {
        using value_type = T;
        static constexpr auto type = Expression_type;

        constexpr scalar_expression() {}

        template<class F>
        constexpr void eval(F&& f) const
        {
            f(*this);
        }

        template<class F>
        static constexpr auto static_eval(F&& f)
        {
            f(ndb::expression<T, Expression_type, void, Clause>{});
            // return ndb::call(f, ndb::expression<T, Expression_type, void, Clause>{});
        }

        template<class Native_expression>
        constexpr void make(Native_expression& ne) const
        {
            ndb::native_expression<Native_expression::expr_category, Expression_type>::template make<Clause>(*this, ne);
        }

        template<expr_clause_code SM_Clause = Clause, int Pass = 0,class Native_expression>
        static constexpr void static_make(Native_expression& ne)
        {
            ndb::native_expression<Native_expression::expr_category, Expression_type>::template static_make<T, Clause>(ne);
        }

        constexpr static auto clause()
        {
            return Clause;
        }
    };


    // root
    template<class Expr, expr_clause_code Clause>
    struct expression<Expr, expr_type_code::root, void, Clause> : expression_base
    {
        static constexpr auto type = expr_type_code::root;

        Expr expr_;

        constexpr explicit expression(const Expr& e) : expr_(e) {}

        template<class F>
        constexpr void eval(F&& f) const
        {
            expr_.eval(f);
        }

        template<class F>
        static constexpr auto static_eval(F&& f)
        {
            return Expr::static_eval(f);
        }

        template<class Native_expression>
        constexpr void make(Native_expression& ne) const
        {
            native_expression<Native_expression::expr_category, expr_type_code::root>::template make<Clause>(expr_, ne);
        }

        template<expr_clause_code SM_Clause = Clause, int Pass = 0,class Native_expression>
        static constexpr void static_make(Native_expression& ne)
        {
            native_expression<Native_expression::expr_category, expr_type_code::root>::template static_make<Expr, Clause>(ne);
        }

        constexpr static auto clause()
        {
            return Clause;
        }
    };

    // table
    template<class T, expr_clause_code Clause>
    struct expression<T, expr_type_code::table, void, Clause>
        : public scalar_expression<T, expr_type_code::table, void, Clause>
    {
        using table = T;
    };

    // field
    template<class T, expr_clause_code Clause>
    struct expression<T, expr_type_code::field, void, Clause>
        : public scalar_expression<T, expr_type_code::field, void, Clause>
    {
        using table = typename T::table;
    };

    // value
    template<class T, expr_clause_code Clause>
    struct expression<T, expr_type_code::value, void, Clause> :
        public scalar_expression<T, expr_type_code::value, void, Clause>
    {
        const T& value_;

        constexpr explicit expression() = delete;

        constexpr explicit expression(const T& n) :
            value_{ n }
        {}

        auto value() const
        {
            return value_;
        }

        template<class F>
        constexpr void eval(F&& f) const
        {
            f(*this);
        }

        template<class F>
        static constexpr auto static_eval(F&& f)
        {
            //f(expression<T, expr_type_code::value, void, Clause>{ value_ });
            //return ndb::call(f, expression<T, expr_type_code::value, void, Clause> { 0 });
        }
    };

    // void expression
    template<expr_clause_code Clause>
    struct expression<void, expr_type_code::null, void, Clause>
        : public scalar_expression<void, expr_type_code::null, void, Clause>
    {};

    // keyword
    template<expr_keyword_code Keyword_code, expr_clause_code Clause>
    struct expression<keyword_type<Keyword_code>, expr_type_code::keyword, void, Clause>
        : public scalar_expression<keyword_type<Keyword_code>, expr_type_code::keyword, void, Clause>
    {
        static constexpr auto keyword_code = Keyword_code;
    };
} // ndb

#endif // EXPRESSION_SCALAR_H_NDB