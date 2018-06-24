#ifndef EXPRESSION_DEDUCTION_H_NDB
#define EXPRESSION_DEDUCTION_H_NDB

#include <ndb/expression/function.hpp>
#include <iostream>

//TODO make all deductions in one browse

namespace ndb
{
    // deduce source
    template<class Expr>
    constexpr auto deduce_source_id()
    {
        int table_id = -1;
        // get source from first field
        Expr::static_eval([&](auto&& e)
        {
            if constexpr (std::decay_t<decltype(e)>::type == expr_type_code::field)
            {
                using Table = typename std::decay_t<decltype(e)>::value_type::table;
                table_id = ndb::table_id<Table>;
            }
            //else if constexpr (e.type == expr_type_code::keyword) return 7;
            //else 9;
        });

        return table_id;
    }
} // ndb

#endif // EXPRESSION_DEDUCTION_H_NDB
