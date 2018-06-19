#include <ndb/expression/sql.hpp>
#include <ndb/engine/postgre/query.hpp>
#include <ndb/expression/deduce.hpp>
#include <ndb/option.hpp>

namespace ndb
{
    template<class Database, class Result_type>
    auto postgre::exec(const postgre_query<Database>& query) const
    {
        return query.exec();
    }

    template<class Database, class Query_option, class Expr>
    auto postgre::exec(const Expr& expr) const
    {
        constexpr auto str_query = ndb::sql_expression<Expr>{};
        postgre_query<Database> query{ str_query.c_str() };

        // bind values from expression
        expr.eval([&](auto&& e)
                  {
                      using expr_type = std::decay_t<decltype(e)>;
                      // e is expr_value
                      if constexpr (ndb::expr_is_value<expr_type>)
                      {
                          using value_type = std::decay_t<decltype(e.value())>;
                          using native_type = ndb::native_type<value_type, Database>;

                          query.bind(e.value());
                      }
                  });

        using Result_type = typename
        std::conditional_t<
            ndb::has_option_v<query_option::object, Query_option>,
            typename ndb::deduce<Expr>::main_table::Detail_::object_type,
            ndb::line<Database>
        >;

        return exec<Database, Result_type>(query);
    };

    template<class Database>
    void postgre::make()
    {
        using Model = typename Database::model;

        std::string output;

        ndb::for_each_entity<Model>([this, &output](auto&& index, auto&& table)
        {
            using Table = std::decay_t<decltype(table)>;

            output += "\ncreate table if not exists T" + std::to_string(ndb::table_id<Table>) + " (";

            ndb::for_each_entity(table, [&output](auto&& i, auto&& field)
            {
                using Field = std::decay_t<decltype(field)>;

                if constexpr (decltype(i){} != 0) output += ",";

                // field id
                output += "\nF" + std::to_string(ndb::field_id<Field>);
                bool need_size = false;

                // field type
                std::string str_field_type;
                using field_value_type = typename std::decay_t<decltype(field)>::value_type;
                using field_ndb_type = ndb_type_t<field_value_type, Database>;
                if constexpr (std::is_same_v<int_, field_ndb_type>) str_field_type += " integer ";
                if constexpr (std::is_same_v<double_, field_ndb_type>) str_field_type += " float ";
                if constexpr (std::is_same_v<string_, field_ndb_type>)
                {
                    str_field_type += " varchar ";
                    need_size = true;
                }
                if constexpr (std::is_same_v<byte_array_, field_ndb_type>) str_field_type += " blob ";

                // field size
                if (field.detail_.size > 0 && need_size) str_field_type += "(" + std::to_string(field.detail_.size) + ")";

                if (field.detail_.is_oid) output += " serial";
                else output += str_field_type;

                // field option
                if (field.detail_.is_primary) output += " primary key";
                if (field.detail_.is_auto_increment) output += " autoincrement";
                if (field.detail_.is_not_null) output += " not null";
                if (field.detail_.is_unique) output += " unique";

            });
            output += ");";

            // exec create table
            exec<Database>(postgre_query<Database>(output));
            output = "";
        });
    }
} // ndb