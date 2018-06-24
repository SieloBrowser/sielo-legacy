#ifndef NDB_PREPROCESSOR_HPP
#define NDB_PREPROCESSOR_HPP

#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/list/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_list.hpp>

#include <ndb/expression.hpp>
#include <ndb/database.hpp>
#include <ndb/model.hpp>
#include <ndb/table.hpp>
#include <ndb/field.hpp>
#include <ndb/entity.hpp>
#include <ndb/result.hpp>

// operator= for fields
#define ndb_internal_field_op \
template<class R> \
constexpr const auto operator=(const R& rhs) const \
{ \
    using L = std::decay_t<decltype(*this)>; \
    constexpr auto lhs = ::ndb::expression<L, ::ndb::expr_type_code::field, void> {}; \
    auto expr_value = ndb::expr_make(rhs); \
    return ::ndb::expression<decltype(lhs), ::ndb::expr_type_code::op_assign, decltype(expr_value), ::ndb::expr_clause_code::add> { lhs, expr_value }; \
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////           GENERATION           ////////////////////////
////////////////////////////////////////////////////////////////////////////////
// declare namespaces
namespace ndb::databases{}
namespace ndb::models{}
namespace ndb::tables{}
namespace ndb::objects{}

// common
// unpack macro
#define ndb_internal_unpack(ITEM) BOOST_PP_CAT(ndb_internal_unpack_, ITEM)

////////////////////////////////////////////////////////////////////////////////
////////////////////////             FIELD              ////////////////////////
////////////////////////////////////////////////////////////////////////////////
// apply func on each ndb_field(...)
#define ndb_internal_for_each_fields(TABLE_NAME, FUNC,...) \
BOOST_PP_LIST_FOR_EACH_I( \
        FUNC, \
        TABLE_NAME, \
        BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__) \
)

// make field
#define ndb_internal_make_field_impl(PARENT, NAME, ...) static constexpr struct NAME##_ : \
    ::ndb::field<PARENT, __VA_ARGS__> { ndb_internal_field_op } NAME {};
#define ndb_internal_make_field_impl_args(TABLE, FIELD) (TABLE, FIELD)
#define ndb_internal_make_field(r, TABLE, i, FIELD_PACK) BOOST_PP_EXPAND(ndb_internal_make_field_impl ndb_internal_make_field_impl_args(TABLE, ndb_internal_unpack(FIELD_PACK)) )

#define ndb_internal_unpack_ndb_field(...)  __VA_ARGS__


// make object
#define ndb_internal_make_object_field(r, TABLE_NAME, i, FIELD_PACK) \
typename ::ndb::tables::TABLE_NAME<void>::BOOST_PP_CAT(BOOST_PP_VARIADIC_ELEM(0, ndb_internal_unpack(FIELD_PACK)), _)::value_type BOOST_PP_VARIADIC_ELEM(0, ndb_internal_unpack(FIELD_PACK));
// make object result encoder
#define ndb_internal_make_object_result_encoder(r, TABLE_NAME, i, FIELD_PACK) \
object.BOOST_PP_VARIADIC_ELEM(0, ndb_internal_unpack(FIELD_PACK)) = line[::ndb::tables::TABLE_NAME<void>::BOOST_PP_VARIADIC_ELEM(0, ndb_internal_unpack(FIELD_PACK))];

////////////////////////////////////////////////////////////////////////////////
////////////////////////             TABLE              ////////////////////////
////////////////////////////////////////////////////////////////////////////////
// table detail
#define ndb_internal_make_table_detail_entity(r, data, i, FIELD_PACK) BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(i, 0)) BOOST_PP_CAT(BOOST_PP_VARIADIC_ELEM(0, ndb_internal_unpack(FIELD_PACK)), _)

#define ndb_table(TABLE_NAME, ...) namespace ndb::objects { struct TABLE_NAME; } namespace ndb::tables { \
    template<class Model> struct TABLE_NAME : ::ndb::table<Model> { \
        ndb_internal_for_each_fields(TABLE_NAME, ndb_internal_make_field, __VA_ARGS__) \
     \
    using Detail_ = ::ndb::table_detail \
    < \
        ::ndb::entity< \
            ndb_internal_for_each_fields(TABLE_NAME, ndb_internal_make_table_detail_entity, __VA_ARGS__) \
        >, \
        ndb::parent<void>, ::ndb::objects::TABLE_NAME \
    >; \
}; \
} /* ndb::tables */ \
namespace ndb::objects \
{ \
    struct TABLE_NAME \
    { \
         BOOST_PP_LIST_FOR_EACH_I( \
                ndb_internal_make_object_field, \
                TABLE_NAME, \
                BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)) \
    }; \
} \
namespace ndb { \
    template<class Engine> \
    struct result_encoder< ::ndb::objects::TABLE_NAME, Engine > \
    { \
        static auto decode(const ::ndb::line<Engine>& line) \
        { \
            ::ndb::objects::TABLE_NAME object; \
                ndb_internal_for_each_fields(TABLE_NAME, ndb_internal_make_object_result_encoder, __VA_ARGS__) \
            return object; \
        } \
    }; \
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////             MODEL              ////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define ndb_internal_make_model_detail_entity(r, data, i, item) BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(i, 0)) BOOST_PP_CAT(item, _)

#define ndb_internal_make_table_impl(MODEL_NAME, TABLE_NAME) using TABLE_NAME##_ = ::ndb::tables::TABLE_NAME< ::ndb::models::MODEL_NAME##_ >; static constexpr TABLE_NAME##_ TABLE_NAME {};
#define ndb_internal_make_table(r, data, item) ndb_internal_make_table_impl(data, item)

#define ndb_model(MODEL_NAME, ...) namespace ndb::models { \
    struct MODEL_NAME##_ { \
    BOOST_PP_LIST_FOR_EACH( \
        ndb_internal_make_table, \
        MODEL_NAME, \
        BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__) \
    ) \
    using Detail_ = ::ndb::model_detail \
    < \
        ::ndb::entity< \
            BOOST_PP_LIST_FOR_EACH_I( \
                ndb_internal_make_model_detail_entity, \
                BOOST_NIL, \
                BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__) \
            ) \
        > \
    >; \
}; \
static constexpr const ::ndb::models::MODEL_NAME##_ MODEL_NAME = {}; \
} // ::ndb::models

////////////////////////////////////////////////////////////////////////////////
////////////////////////            DATABASE            ////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define ndb_internal_make_database_detail_entity(r, data, i, item) BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(i, 0)) BOOST_PP_CAT(BOOST_PP_VARIADIC_ELEM(0, ndb_internal_unpack(item)), _)

#define ndb_internal_unpack_ndb_database(...)  __VA_ARGS__

#define ndb_internal_make_database_impl(PROJECT_NAME, NAME, MODEL_NAME, DB_ENGINE) static constexpr struct NAME##_ : ::ndb::database< ::ndb::databases::PROJECT_NAME, ::ndb::models::MODEL_NAME##_, DB_ENGINE >{} NAME{};
#define ndb_internal_make_database_impl_args(data, item) (data, item)

#define ndb_internal_make_database(r, data, item) BOOST_PP_EXPAND(ndb_internal_make_database_impl ndb_internal_make_database_impl_args(data, ndb_internal_unpack(item)) )

#define ndb_project(PROJECT_NAME, ...) namespace ndb::databases { \
    struct PROJECT_NAME { \
    static constexpr auto name = #PROJECT_NAME; \
    BOOST_PP_LIST_FOR_EACH( \
        ndb_internal_make_database, \
        PROJECT_NAME, \
        BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__) \
    ) \
    using Detail_ = ::ndb::database_detail \
    < \
        ::ndb::entity< \
            BOOST_PP_LIST_FOR_EACH_I( \
                ndb_internal_make_database_detail_entity, \
                BOOST_NIL, \
                BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__) \
            ) \
        > \
    >; \
}; \
} // ::ndb::databases

////////////////////////////////////////////////////////////////////////////////
////////////////////////             ALIASES            ////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define ndb_field_id ndb_field(id, ::ndb::type_get< ::ndb::int64_>, ::ndb::option<ndb::field_option::oid>)

#endif // NDB_PREPROCESSOR_HPP
