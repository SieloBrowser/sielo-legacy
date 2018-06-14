#ifndef ENGINE_POSTGRE_CONNECTION_H_NDB
#define ENGINE_POSTGRE_CONNECTION_H_NDB

#include <ndb/engine/basic_connection.hpp>
#include <ndb/engine/connection_param.hpp>
#include <ndb/error.hpp>

#include <array>
#include <postgresql/libpq-fe.h>

namespace ndb
{
    template<class Engine>
    class engine_connection;

    class postgre;

    template<>
    class engine_connection<postgre> : basic_connection<postgre>
    {
    public:
        engine_connection(ndb::connection_param params) :
            basic_connection(std::move(params)),
            connection_{ nullptr }
        {
            constexpr auto param_count = 4;
            unsigned int param_index = 0;
            std::array<const char*, param_count> keys;
            std::array<const char*, param_count> values;

            auto param_add = [&](const char* key, const char* value)
            {
                keys[param_index] = key;
                values[param_index] = value;
                param_index++;
            };

            param_add("user", params_.user.c_str());
            param_add("port", std::to_string(params_.port).c_str());
            param_add("host", params_.host.c_str());
            //param_add("dbname", params_.db_name.c_str());
            param_add(nullptr, nullptr);

            // check server status
            PGPing server_status = PQpingParams(keys.data(), values.data(), 0);
            if (server_status != PQPING_OK) ndb_error("unable to connect to host : " + params_.host);

            connection_ = PQconnectdbParams(keys.data(), values.data(), 0);

            if (PQstatus(connection_) != CONNECTION_OK)
            {
                auto error_message = std::string(PQerrorMessage(connection_));
                PQfinish(connection_);
                ndb_error("database connection failed : " + error_message);
            }
        }

        ~engine_connection()
        {
            PQfinish(connection_);
        }

        operator PGconn*()
        {
            return connection_;
        }

    private:
        PGconn* connection_;
    };
} // ndb

#endif // ENGINE_POSTGRE_CONNECTION_H_NDB