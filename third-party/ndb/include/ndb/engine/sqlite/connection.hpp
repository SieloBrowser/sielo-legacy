#ifndef ENGINE_SQLITE_CONNECTION_H_NDB
#define ENGINE_SQLITE_CONNECTION_H_NDB

#include <ndb/engine/basic_connection.hpp>
#include <ndb/engine/connection_param.hpp>
#include <ndb/error.hpp>
#include <ndb/setup.hpp>

#include <sqlite3.h>
#include <experimental/filesystem>

namespace ndb
{
    namespace fs = std::experimental::filesystem;

    template<class Engine>
    class engine_connection;

    template<>
    class engine_connection<sqlite> : basic_connection<sqlite>
    {
    public:
        engine_connection(ndb::connection_param params) :
            basic_connection(params),
            connection_{ nullptr }
        {
            if (params_.path.empty()) params_.path = "./";
            if (!fs::exists(params_.path)) fs::create_directory(params_.path);
            std::string fullpath = params_.path + "/" + params_.db_name + setup<sqlite>::ext;

            int native_flag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
            if ((int)params_.flag & (int)ndb::connection_flag::read_only)
            {
                native_flag = SQLITE_OPEN_READONLY;
            }

            auto status = sqlite3_open_v2(fullpath.c_str(), &connection_, native_flag, nullptr);

            if (status != SQLITE_OK) ndb_error("database connection failed");
        }

        ~engine_connection()
        {
            sqlite3_close(connection_);
        }

        operator sqlite3*()
        {
            return connection_;
        }

    private:
        sqlite3* connection_;
    };
} // ndb

#endif // ENGINE_SQLITE_CONNECTION_H_NDB