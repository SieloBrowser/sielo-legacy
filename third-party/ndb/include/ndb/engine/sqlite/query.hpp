#ifndef ENGINE_SQLITE_STATEMENT_H_NDB
#define ENGINE_SQLITE_STATEMENT_H_NDB

#include <ndb/type.hpp>
#include <ndb/engine.hpp>
#include <ndb/engine/sqlite/type.hpp>
#include <ndb/result.hpp>

#include <sqlite3.h>

namespace ndb
{
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
            auto status = sqlite3_prepare_v2(sqlite3_db(), str_statement_.c_str(), -1, &statement_, nullptr);

            if (status != SQLITE_OK)
            {
                std::string error = sqlite3_errmsg(sqlite3_db());
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
            if constexpr (std::is_same_v<int, T>) sqlite3_bind_int(statement_, bind_index_, value);
            if constexpr (std::is_same_v<double, T>) sqlite3_bind_double(statement_, bind_index_, value);
            if constexpr (std::is_same_v<std::string, T>) sqlite3_bind_text(statement_, bind_index_, value.c_str(), -1, SQLITE_TRANSIENT); //TODO: use SQLITE_STATIC
            if constexpr (std::is_same_v<std::vector<char>, T>) sqlite3_bind_blob(statement_, bind_index_, value.data(), value.size(), SQLITE_TRANSIENT);
            bind_index_++;
        }

        template<class T>
        void bind(const T& value)
        {
            if constexpr (ndb::is_native_type_v<ndb::sqlite, T>) bind_value(value);
            else bind_value(ndb::custom_type<T, Database>::encode(value)); //check encoders if you have an error here
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
                        case ndb::engine_type_id<sqlite, int_>::value:
                            line.add(field_id,
                                     cpp_type_t<int_, Database>{ sqlite3_value_int(field_value) } ); break;

                        case ndb::engine_type_id<sqlite, double_>::value:
                            line.add(field_id,
                                     cpp_type_t<double_, Database>{ sqlite3_value_double(field_value) } ); break;

                        case ndb::engine_type_id<sqlite, string_>::value:
                            line.add(field_id,
                                     cpp_type_t<string_, Database>{ reinterpret_cast<const char*>(sqlite3_value_text(field_value)) } );
                            break;

                        case ndb::engine_type_id<sqlite, byte_array_>::value:
                            data = reinterpret_cast<const char*>(sqlite3_value_blob(field_value));
                            data_size = sqlite3_value_bytes(field_value);
                            line.add(field_id,
                                     cpp_type_t<string_, Database>{ data, data + data_size } );
                            break;

                        case ndb::engine_type_id<sqlite, null_>::value:
                            line.add(field_id,
                                     ndb::null_type{} ); break;

                        default:
                            ndb_error("unknown engine type");
                    } // switch
                } // for

                if (field_count > 0)
                {
                    auto result_entity = ndb::result_encoder<Result_type, Database>::decode(std::move(line));
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

        auto sqlite3_db() const
        {
            return ndb::engine<ndb::sqlite>::get().connection<Database>().database();
        }

    private:
        sqlite3_stmt* statement_;
        std::string str_statement_;
        int bind_index_;
    };
} // ndb

#endif // ENGINE_SQLITE_STATEMENT_H_NDB