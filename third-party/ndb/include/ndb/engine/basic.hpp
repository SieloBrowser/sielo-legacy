#ifndef ENGINE_BASIC_H_NDB
#define ENGINE_BASIC_H_NDB

#include <ndb/engine/connection_param.hpp>
#include <ndb/engine/sqlite/connection.hpp>

#include <unordered_map>

namespace ndb
{
    template<class Engine>
    class basic_engine
    {
    public:
        static Engine& instance();

        template<class Database>
        inline void connect(ndb::connection_param params);

        template<class Database>
        inline ndb::engine_connection<Engine>& connection() const;

    protected:
        basic_engine();
        ~basic_engine();

        basic_engine(const basic_engine&) = delete;
        basic_engine& operator=(const basic_engine&) = delete;

        template<class Database>
        inline void make();

    private:
        static Engine* instance_;

        std::unordered_map<int, std::unique_ptr<ndb::engine_connection<Engine>>> connections_;
    };
} // ndb

#include <ndb/engine/basic.tpp>

#endif // ENGINE_BASIC_H_NDB