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

        template<class T>
        inline void bind(sqlite3_stmt* statement, int bind_index, const T& v) const;

        template<class Database>
        inline void exec(const char* str_query) const;

        template<class Database>
        inline void exec(const std::string& str_query) const;

        template<class Database, class Query_option, class Expr>
        inline auto exec(const Expr& expr) const;

        template<class Database>
        inline void make();

        template<class Expr>
        inline static std::string to_string(const Expr&);

        inline static constexpr auto expr_category();

        template<class T>
        static constexpr bool is_native = std::is_same_v<std::decay_t<T>, int>
                                          || std::is_same_v<std::decay_t<T>, double>
                                          || std::is_same_v<std::decay_t<T>, std::string>
                                          || std::is_same_v<std::decay_t<T>, std::vector<char>>;

    private:
        std::unordered_map<int, std::unique_ptr<sqlite_connection>> connections_;
        std::unordered_map<int, ndb::connection_flag> connections_flag_;
    };
} // ndb

#include <ndb/engine/sqlite/sqlite.tpp>

#endif // ENGINE_SQLITE_SQLITE_H_NDB