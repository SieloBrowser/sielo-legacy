#ifndef ENGINE_SQLITE_STATEMENT_H_NDB
#define ENGINE_SQLITE_STATEMENT_H_NDB

#include <ndb/type.hpp>
#include <ndb/engine.hpp>
#include <ndb/engine/sqlite/type.hpp>
#include <ndb/result.hpp>
#include <ndb/type.hpp>

#include <sqlite3.h>

namespace ndb
{
    template<class Engine>
    class engine_connection;

    template<class Database>
    class sqlite_query
    {
    public:
        using Engine = typename Database::engine;

        sqlite_query(std::string str_statement) :
            statement_{ nullptr },
            str_statement_{ std::move(str_statement) },
            bind_index_{ 1 }
        {
            auto status = sqlite3_prepare_v2(connection(), str_statement_.c_str(), -1, &statement_, nullptr);

            if (status != SQLITE_OK)
            {
                std::string error = sqlite3_errmsg(connection());
                ndb_error("query error : " + error);
            }
        }

        ~sqlite_query()
        {
            sqlite3_finalize(statement_);
        }

        template<class T>
        void bind_value(const T& value)
        {
            using storage_type = ndb::storage_type_t<typename Database::engine, ndb_type_t<T, Database>>;

            if constexpr (std::is_same_v<int_, storage_type>) sqlite3_bind_int(statement_, bind_index_, value);
            else if constexpr (std::is_same_v<int64_, storage_type>) sqlite3_bind_int64(statement_, bind_index_, value);
            else if constexpr (std::is_same_v<double_, storage_type>) sqlite3_bind_double(statement_, bind_index_, value);
            else if constexpr (std::is_same_v<string_, storage_type>)
            {
                auto data = ndb::type_data(value);
                constexpr int char_size = sizeof(std::remove_pointer_t<decltype(data)>);

                if constexpr (char_size == 1)
                    sqlite3_bind_text(statement_, bind_index_, data, -1, SQLITE_TRANSIENT);
                else if constexpr (char_size == 2)
                    sqlite3_bind_text16(statement_, bind_index_, data, -1, SQLITE_TRANSIENT);
                else ncx_error(T, "unsupported string format");
            }
            else if constexpr (std::is_same_v<byte_array_, storage_type>) sqlite3_bind_blob(statement_, bind_index_, ndb::type_data(value), value.size(), SQLITE_TRANSIENT);
            else ncx_error(T, "type unknown, add a custom type or use an engine type");
            bind_index_++;
        }

        template<class T>
        void bind(const T& value)
        {
            if constexpr (ndb::is_custom_type_v<T, Database>) bind_value(ndb::custom_type<T, Database>::internal_encode(value));
            else bind_value(value);
        };

        template<class Result_type = ndb::line<Database>>
        auto exec() const
        {
            int step = SQLITE_DONE;

            ndb::result<Database, Result_type> result;

            #ifdef NDB_DEBUG_QUERY
                auto str_statement = std::string(sqlite3_expanded_sql(statement_));
                std::cout << "[ndb:debug_query]" << str_statement << std::endl;
            #endif

            step = sqlite3_step(statement_);

            // select
            while (step == SQLITE_ROW)
            {
                ndb::line<Database> line;
                int field_count = sqlite3_column_count(statement_);

                for(int field_it = 0; field_it < field_count; field_it++)
                {
                    //const char* table_name = sqlite3_column_table_name(statement, field_it);
                    const char* field_name = sqlite3_column_name(statement_, field_it);
                    int field_id = -1;
                    if (field_name[0] == 'F') field_id = std::stoi(std::string(field_name + 1));

                    int field_type_id = sqlite3_column_type(statement_, field_it);
                    sqlite3_value* field_value = sqlite3_column_value(statement_, field_it);

                    const char* data = nullptr;
                    int data_size = 0;

                    switch(field_type_id)
                    {
                        case ndb::engine_type_id<sqlite, int64_>::value:
                            line.add(field_id, ndb::type_make<cpp_type_t<int64_, Database>>(sqlite3_value_int64(field_value)));
                            break;

                        case ndb::engine_type_id<sqlite, double_>::value:
                            line.add(field_id, ndb::type_make<cpp_type_t<double_, Database>>(sqlite3_value_double(field_value)));
                            break;

                        case ndb::engine_type_id<sqlite, string_>::value:
                            line.add(field_id, ndb::type_make<cpp_type_t<string_, Database>>(reinterpret_cast<const char*>(sqlite3_value_text(field_value))));
                            break;

                        case ndb::engine_type_id<sqlite, byte_array_>::value:
                            data = reinterpret_cast<const char*>(sqlite3_value_blob(field_value));
                            data_size = sqlite3_value_bytes(field_value);
                            line.add(field_id, ndb::type_make<cpp_type_t<byte_array_, Database>>(data, data_size));
                            break;

                        case ndb::engine_type_id<sqlite, null_>::value:
                            line.add(field_id, ndb::null_type{} );
                            break;

                        default:
                            ndb_error("unknown engine type");
                    } // switch
                } // for

                if (field_count > 0)
                {
                    auto result_entity = ndb::result_encoder<Result_type, Database>::decode(line);
                    result.add(std::move(result_entity));
                }
                step = sqlite3_step(statement_);
            } // while

            return result;
        }

        const char* c_str() const
        {
            return str_statement_.c_str();
        }

        const std::string& str() const
        {
            return str_statement_;
        }

        ndb::engine_connection<sqlite>& connection() const
        {
            return ndb::engine<ndb::sqlite>::get().connection<Database>();
        }

    private:
        sqlite3_stmt* statement_;
        std::string str_statement_;
        int bind_index_;
    };
} // ndb

#endif // ENGINE_SQLITE_STATEMENT_H_NDB