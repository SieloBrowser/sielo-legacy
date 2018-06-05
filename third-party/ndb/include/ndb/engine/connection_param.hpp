#ifndef ENGINE_CONNECTION_PARAM_H_NDB
#define ENGINE_CONNECTION_PARAM_H_NDB

#include <string>

namespace ndb
{
    enum class connection_flag
    {
        default_ = 0,
        read_only = 1
    };


    struct connection_param
    {
        std::string host;
        int16_t port;
        std::string user;
        std::string pass;
        std::string path;

        std::string db_name;

        connection_flag flag;

        connection_param() :
            host{ "localhost" },
            port{ 5555 },
            user{ "ndb_user" },
            path{ "database" }
        {}
    };

} // ndb

#endif // ENGINE_CONNECTION_PARAM_H_NDB