#ifndef EXPRESSION_BSON_CODE_H_NDB
#define EXPRESSION_BSON_CODE_H_NDB

#include <ndb/expression/code.hpp>

namespace ndb
{
    template<> struct expr_code<expr_type_code::op_and, expr_category_code::bson> { constexpr static auto value ="AND"; };
    template<> struct expr_code<expr_type_code::op_or, expr_category_code::bson> { constexpr static auto value ="OR"; };
    template<> struct expr_code<expr_type_code::op_equal, expr_category_code::bson> { constexpr static auto value ="$eq"; };
    template<> struct expr_code<expr_type_code::op_assign, expr_category_code::bson> { constexpr static auto value ="="; };
    template<> struct expr_code<expr_type_code::op_list, expr_category_code::bson> { constexpr static auto value =","; };

    template<> struct clause_code<expr_clause_code::get, expr_category_code::bson> { constexpr static auto value ="SELECT"; };
    template<> struct clause_code<expr_clause_code::add, expr_category_code::bson> { constexpr static auto value ="INSERTINTO"; };
    template<> struct clause_code<expr_clause_code::del, expr_category_code::bson> { constexpr static auto value ="DELETE"; };
    template<> struct clause_code<expr_clause_code::value_list, expr_category_code::bson> { constexpr static auto value ="VALUES"; };
    template<> struct clause_code<expr_clause_code::source, expr_category_code::bson> { constexpr static auto value ="FROM"; };
    template<> struct clause_code<expr_clause_code::condition, expr_category_code::bson> { constexpr static auto value ="WHERE"; };
} // ndb

#endif // EXPRESSION_BSON_CODE_H_NDB
