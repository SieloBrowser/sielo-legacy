#ifndef FWD_H_NDB
#define FWD_H_NDB

namespace ndb
{
    template<class Model_table, class... Options>
    struct model_detail;

    struct field_base;

    template<class Table, class T, class = void, class...>
    struct field;

    template<class Model>
    struct table;

    template<class... Fields>
    struct entity;

    struct expression_base;
    class table_base;
    class option_base;
    class database_base;

    // type
    struct proxy_type_base;
    namespace internal { struct custom_type_not_found; }
    namespace scope { struct global; }
    template<class T, class Database>
    struct custom_type;

    // expression
    enum class expr_clause_code;
    enum class expr_type_code;
    enum class expr_category_code;
    class sql;
    class bson;

} // ndb

#endif // FWD_H_NDB