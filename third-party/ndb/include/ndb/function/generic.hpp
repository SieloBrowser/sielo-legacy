#ifndef FUNCTION_GENERIC_H_NDB
#define FUNCTION_GENERIC_H_NDB

#include <ndb/expression/code.hpp>
#include <ndb/cx_error.hpp>

namespace ndb::functions
{
    template<class Engine, class Database, class Table, ndb::expr_category_code Expr_category = Engine::expr_category()>
    struct clear
    {
        static auto process()
        {
            ncx_error(clear, cx_err_not_implemented);
        }
    };

    template<class Engine, ndb::expr_category_code Expr_category, class Database>
    struct remove
    {
        static auto process()
        {
            ncx_error(remove, cx_err_not_implemented);
        }
    };
} // ndb::functions

#endif // FUNCTION_GENERIC_H_NDB