#ifndef ENGINE_BASIC_CONNECTION_H_NDB
#define ENGINE_BASIC_CONNECTION_H_NDB

#include <ndb/engine/connection_param.hpp>

#include <sqlite3.h>
#include <memory>

namespace ndb
{
    template<class Engine>
    class engine_connection;

    template<class Engine>
    class basic_connection
    {
    public:
        static auto make(ndb::connection_param params)
        {
            return std::make_unique<ndb::engine_connection<Engine>>(std::move(params));
        }

        const ndb::connection_param& params() const
        {
            return params_;
        }

    protected:
        basic_connection(ndb::connection_param params) :
            params_{ std::move(params) }
        {}

        basic_connection(const basic_connection&) = delete;
        basic_connection& operator=(const basic_connection&) = delete;

    protected:
        ndb::connection_param params_;
    };
} // ndb

#endif // ENGINE_BASIC_CONNECTION_H_NDB