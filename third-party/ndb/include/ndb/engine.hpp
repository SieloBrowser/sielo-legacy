#ifndef ENGINE_H_NDB
#define ENGINE_H_NDB

#include <ndb/engine/connection_param.hpp>

namespace ndb
{
    template<class Engine>
    class engine
    {
    public:
        static Engine& get()
        {
            return Engine::instance();
        }

        template<class Database>
        static void connect(ndb::connection_param params)
        {
            get().template connect<Database>(std::move(params));
        }
    };

    template<class Database, class Engine = typename Database::engine>
    void connect(ndb::connection_param params = ndb::connection_param{})
    {
        ndb::engine<Engine>::template connect<Database>(std::move(params));
    }
} // ndb

#endif // ENGINE_H_NDB
