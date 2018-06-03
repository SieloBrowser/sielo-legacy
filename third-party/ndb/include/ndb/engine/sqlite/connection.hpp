#ifndef ENGINE_SQLITE_CONNECTION_H_NDB
#define ENGINE_SQLITE_CONNECTION_H_NDB

#include <sqlite3.h>

#include <ndb/error.hpp>
#include <ndb/option.hpp>
#include <ndb/setup.hpp>

#include <experimental/filesystem>
#include <unordered_map>

namespace ndb
{
    namespace fs = std::experimental::filesystem;

    class sqlite_connection
    {
    public:
        sqlite_connection(const std::string& db_name, const std::string& path = "", ndb::connection_flag flags = connection_flag::default_) :
            db_name_{ db_name },
            database_{ nullptr },
            path_{ path },
            flags_{ flags }
        {}

        void connect()
        {
            if (path_.empty()) path_ = "./";
            if (!fs::exists(path_)) fs::create_directory(path_);
            std::string fullpath = path_ + "/" + db_name_ + setup<sqlite>::ext;

            int native_flag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
            if ((int)flags_ & (int)ndb::connection_flag::read_only)
            {
                native_flag = SQLITE_OPEN_READONLY;
            }


            auto status = sqlite3_open_v2(fullpath.c_str(), &database_, native_flag, nullptr);

            if (status != SQLITE_OK) ndb_error("database connection failed");
        }

        ~sqlite_connection()
        {
            sqlite3_close(database_);
        }

        void flag_add(ndb::connection_flag flag)
        {
            flags_ = static_cast<ndb::connection_flag>((int)flags_ | (int)flag);
        }

        ndb::connection_flag flags()
        {
            return flags_;
        }

        sqlite3* database()
        {
            return database_;
        }

    private:
        sqlite3* database_;
        std::string db_name_;
        std::string path_;

        ndb::connection_flag flags_;
    };

} // ndb

#endif // ENGINE_SQLITE_CONNECTION_H_NDB