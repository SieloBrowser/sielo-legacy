#ifndef ENGINE_MONGO_MONGO_H_NDB
#define ENGINE_MONGO_MONGO_H_NDB

#define BSON_STATIC
#define MONGOC_STATIC

#include <ndb/engine/basic.hpp>

#include <ndb/expression/bson.hpp>
#include <ndb/expression/utility.hpp>
#include <ndb/utility.hpp>
#include <ndb/result.hpp>
#include <memory>

#include <bcon.h>
#include <mongoc.h>
#include <iostream>


namespace ndb
{
    class mongo_connection
    {
    public:
        mongo_connection(const std::string& db_name) :
            client_{ nullptr },
            database_{ nullptr }
        {
            client_ = mongoc_client_new("mongodb://localhost:27017");
            database_ = mongoc_client_get_database(client_, db_name.c_str());
        }
        ~mongo_connection()
        {
            mongoc_database_destroy(database_);
            mongoc_client_destroy(client_);
        }

        mongoc_client_t* client() const
        {
            return client_;
        }

        mongoc_database_t* database() const
        {
            return database_;
        }

    private:
        mutable mongoc_client_t* client_;
        mutable mongoc_database_t* database_;
    };

    class mongo : public basic_engine<mongo>
    {
    public:
        mongo(const mongo&) = delete;
        mongo& operator=(const mongo&) = delete;

        mongo()
        {
            mongoc_init();
        }

        ~mongo()
        {
            mongoc_cleanup();
        }

        template<class Database>
        void connect()
        {
            std::string db_name = Database::group::name;
            db_name += "_D" + std::to_string(ndb::database_id<Database>);
            auto conn = std::make_unique<mongo_connection>(db_name);

            // test server connection
            bson_t reply;
            bson_error_t error;
            bson_t* command = BCON_NEW ("ping", BCON_INT32(1));
            bool status = mongoc_client_command_simple(conn->client(), "admin", command, nullptr, &reply, &error);
            auto err = std::string(error.message);
            bson_destroy(command);
            bson_destroy(&reply);

            if (!status) ndb_error(err);

            connection_list_.emplace(ndb::database_id<Database>, std::move(conn));
        }

        template<class Database>
        const mongo_connection& connection() const
        {
            if (!connection_list_.count(ndb::database_id<Database>)) ndb_error("database connection not found : D" + std::to_string(ndb::database_id<Database>));
            return *connection_list_.at(ndb::database_id<Database>).get();
        }

        template<class Database, class Expr>
        ndb::result<mongo> exec(const Expr& expr) const
        {
            ndb::result<mongo> result;

            auto e = ndb::bson_expression<Expr>{ expr };
            std::cout << "\nQUERY : " << e.to_string();

            // database and table name
            std::string db_str = "D" + std::to_string(ndb::database_id<Database>);
            std::string table_str = "T" + std::to_string(ndb::deduce_source_id<Expr>());

            mongoc_collection_t* table = mongoc_client_get_collection(connection<Database>().client(), db_str.c_str(), table_str.c_str());


            mongoc_cursor_t* cursor;
            const bson_t* doc;

            cursor = mongoc_collection_aggregate(table, MONGOC_QUERY_NONE, e.document(), nullptr, nullptr);
            std::cout << "\nRESULT \n";
            while (mongoc_cursor_next(cursor, &doc))
            {
                char *str = bson_as_canonical_extended_json(doc, nullptr);
                printf("%s\n", str);
                bson_free(str);
            }

            /*
            if constexpr (Expr::clause() == expr_clause_code::add)
            {
                bson_error_t error;
                bool query_success = mongoc_collection_insert(table, MONGOC_INSERT_NONE, e.document(), nullptr, &error);
                if (!query_success) ndb_error(std::string("query error : ") + error.message);
            }

            if constexpr (Expr::clause() == expr_clause_code::get)
            {
                const bson_t* doc;
                mongoc_cursor_t* cursor;
                cursor = mongoc_collection_find(table, MONGOC_QUERY_NONE, 0, 0, 0, e.document(), nullptr, nullptr);
                std::cout << "\nRESULT \n";
                while (mongoc_cursor_next(cursor, &doc)) {
                    char* str = bson_as_canonical_extended_json(doc, nullptr);
                    printf ("%s\n", str);
                    bson_free (str);
                }
            }
             */

            mongoc_collection_destroy(table);

            return result;
        }

        template<class Model>
        void make()
        {}

        static constexpr auto expr_category() { return expr_category_code::bson; }

    private:
        std::unordered_map<int, std::unique_ptr<mongo_connection>> connection_list_;
    };
} // db

#endif // ENGINE_MONGO_MONGO_H_NDB

