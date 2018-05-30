#ifndef EXPRESSION_BSON_TYPE_H_NDB
#define EXPRESSION_BSON_TYPE_H_NDB

#include <ndb/expression/bson/code.hpp>
#include <ndb/expression/type.hpp>
#include <ndb/utility.hpp>

namespace ndb
{
    // default make for expression_type
    template<expr_type_code T>
    struct expression_type<T, expr_category_code::bson>
    {
        template<class L, class R, class Native_expression>
        static constexpr void make(const L& lhs, const R& rhs, Native_expression& ne)
        {
            //ne.document_begin(expr_code<T, expr_category_code::bson>::value);
            //ne.document_end();
            lhs.make(ne);
            rhs.make(ne);
        }
    };

    // shift_left
    template<>
    struct expression_type<expr_type_code::op_shift_left, expr_category_code::bson>
    {
        template<class L, class R, class Native_expression>
        static constexpr void make(const L &lhs, const R &rhs, Native_expression &ne)
        {
            lhs.make(ne);

            rhs.make(ne);
        }
    };

    // assign
    template<>
    struct expression_type<expr_type_code::op_assign, expr_category_code::bson>
    {
        template<class L, class R, class Native_expression>
        static constexpr void make(const L& lhs, const R& rhs, Native_expression& ne)
        {
            if constexpr (ndb::expr_is_field<L>)
            {
                using Ftype = typename std::decay_t<decltype(lhs)>::value_type;
                std::string key = "F" + std::to_string(ndb::field_id<Ftype>);
                //ne.append(key.c_str(), rhs.value());
            }

            lhs.make(ne);
            rhs.make(ne);
        }
    };

    // list
    template<>
    struct expression_type<expr_type_code::op_list, expr_category_code::bson>
    {
        template<class L, class R, class Native_expression>
        static constexpr void make(const L& lhs, const R& rhs, Native_expression& ne)
        {
            /*
            ne.document_begin("$project");
            if constexpr (ndb::expr_is_field<L>)
            {
                using Ftype = typename std::decay_t<decltype(lhs)>::value_type;
                std::string key = "F" + std::to_string(ndb::field_id<Ftype>);
                ne.append(key.c_str(), 1);
            }
            if constexpr (ndb::expr_is_field<R>)
            {
                using Ftype = typename std::decay_t<decltype(rhs)>::value_type;
                std::string key = "F" + std::to_string(ndb::field_id<Ftype>);
                ne.append(key.c_str(), 1);
            }

            ne.document_end();*/

            lhs.make(ne);
            rhs.make(ne);
        }
    };
} // ndb

#endif // EXPRESSION_BSON_TYPE_H_NDB
