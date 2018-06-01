#include <ndb/expression/sql.hpp>
#include <ndb/engine/sqlite/function.hpp>
#include <ndb/engine/sqlite/type.hpp>
#include <ndb/engine/sqlite/query.hpp>
#include <ndb/expression/deduce.hpp>
#include <ndb/option.hpp>
#include <iostream> // query_debug

namespace ndb
{
    sqlite::sqlite() {}

    template<class Database>
    void sqlite::connect(const std::string& path)
    {
        int db_id = ndb::database_id<Database>;
        auto conn_flag = ndb::connection_flag::default_;
        if (connections_flag_.count(db_id)) conn_flag = connections_flag_.at(db_id);

        std::string db_name = Database::group::name;
        db_name += "_D" + std::to_string(db_id);
        auto conn = std::make_unique<sqlite_connection>(db_name, path, conn_flag);
        conn->connect();
        connections_.emplace(db_id, std::move(conn));

        // database connected, add foreign keys and create model
        exec<Database>(std::string("PRAGMA foreign_keys = ON;"));
        make<Database>();
    }

    template<class Database>
    void sqlite::config(ndb::connection_flag flag)
    {
        connections_flag_.emplace(ndb::database_id<Database>, flag);
    }

    template<class Database>
    sqlite_connection& sqlite::connection() const
    {
        if (!connections_.count(ndb::database_id<Database>)) ndb_error("database connection not found : D" + std::to_string(ndb::database_id<Database>));
        return *connections_.at(ndb::database_id<Database>).get();
    }

    template<class Database>
    void sqlite::exec(const std::string& str_statement) const
    {
        sqlite_query<Database> statement{ str_statement };
        exec<Database>(statement);
    }

    template<class Database, class Query_option, class Expr>
    auto sqlite::exec(const Expr& expr) const
    {
        constexpr auto str_query = ndb::sql_expression<Expr>{};
        sqlite_query<Database> query{ str_query.c_str() };

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

    template<class Database, class Result_type>
    auto sqlite::exec(sqlite_query<Database>& query) const
    {
        return query.exec();
    }

    template<class Database>
    void sqlite::make()
    {
        using Model = typename Database::model;

        std::string output;

        ndb::for_each_entity<Model>([this, &output](auto&& index, auto&& table)
        {
            using Table = std::decay_t<decltype(table)>;

            output += "\ncreate table if not exists `T" + std::to_string(ndb::table_id<Table>) + "` (";

            ndb::for_each_entity(table, [&output](auto&& i, auto&& field)
            {
                using Field = std::decay_t<decltype(field)>;

                if constexpr (decltype(i){} != 0) output += ",";

                // field id
                output += "\nF" + std::to_string(ndb::field_id<Field>);
                bool need_size = false;

                // field type
                using field_value_type = typename std::decay_t<decltype(field)>::value_type;
                using field_ndb_type = ndb_type_t<field_value_type, Database>;
                if constexpr (std::is_same_v<int_, field_ndb_type>) output += " integer ";
                if constexpr (std::is_same_v<double_, field_ndb_type>) output += " float ";
                if constexpr (std::is_same_v<string_, field_ndb_type>)
                {
                    output += " text ";
                    need_size = true;
                }
                if constexpr (std::is_same_v<byte_array_, field_ndb_type>) output += " blob ";

                // field size
                if (field.detail_.size > 0 && need_size) output += "(" + std::to_string(field.detail_.size) + ")";

                // field option
                if (field.detail_.is_primary || field.detail_.is_oid) output += " primary key";
                if (field.detail_.is_auto_increment) output += " autoincrement";
                if (field.detail_.is_not_null) output += " not null";
                if (field.detail_.is_unique) output += " unique";

                if constexpr (ndb::is_field_entity<Field>)
                {
                    /*
                    auto store_type = typename Field::type{};

                    //output += "\n\tentity type " + std::string(store_type.detail_.name);
                    output += "\n\tentity count : " + std::to_string(field.detail_.size);
                    output += "\n\ttotal size : " + std::to_string(store_type.detail_.size);

                    if constexpr (ndb::is_field_entity_vector<Field>) output += " VECTOR";*/
                }
            });
            output += ");";

            // exec create table
            exec<Database>(output);
            output = "";
        });
    }

    template<class Expr>
    std::string sqlite::to_string(const Expr&)
    {
        constexpr auto str_query = ndb::sql_expression<Expr>{};
        return str_query.to_string();
    }

    constexpr auto sqlite::expr_category()
    {
        return expr_category_code::sql;
    }
} // ndb
