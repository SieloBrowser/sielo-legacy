#ifndef ENGINE_SQLITE_SQLITE_H_NDB
#define ENGINE_SQLITE_SQLITE_H_NDB

#include <sqlite3.h>

#include <ndb/engine/basic.hpp>
#include <ndb/engine/sqlite/connection.hpp>
#include <ndb/error.hpp>
#include <ndb/value.hpp>
#include <ndb/result.hpp>
#include <ndb/setup.hpp>

#include <experimental/filesystem>
#include <unordered_map>

namespace ndb
{
    template<class Database>
    class sqlite_query;

    class sqlite : public basic_engine<sqlite>
    {
    public:
        inline sqlite();

        template<class Database>
        inline void connect(const std::string& path = "");

        template<class Database>
        inline void config(ndb::connection_flag);

        template<class Database>
        inline sqlite_connection& connection() const;

        template<class Database>
        inline void exec(const std::string& str_statement) const;

        template<class Database, class Result_type = ndb::line<Database>>
        inline auto exec(sqlite_query<Database>& query) const;

        template<class Database, class Query_option, class Expr>
        inline auto exec(const Expr& expr) const;

        template<class Database>
        inline void make();

        template<class Database>
        auto last_id() const
        {
            return sqlite3_last_insert_rowid(connection<Database>().database());
        }

        template<class Expr>
        inline static std::string to_string(const Expr&);

        inline static constexpr auto expr_category();

    private:
        std::unordered_map<int, std::unique_ptr<sqlite_connection>> connections_;
        std::unordered_map<int, ndb::connection_flag> connections_flag_;
    };
} // ndb

#include <ndb/engine/sqlite/sqlite.tpp>

#endif // ENGINE_SQLITE_SQLITE_H_NDB