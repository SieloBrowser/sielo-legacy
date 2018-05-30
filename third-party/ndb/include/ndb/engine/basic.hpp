#ifndef ENGINE_BASIC_H_NDB
#define ENGINE_BASIC_H_NDB

#include <ndb/error.hpp>

namespace ndb
{
    template<class Engine>
    class basic_engine
    {
    public:
        static Engine& instance()
        {
            if (instance_ == nullptr) ndb_error("engine not initialized");
            return *instance_;
        }

    protected:
        basic_engine()
        {
            if (instance_ == nullptr)
            {
                instance_ = static_cast<Engine*>(this);
            }
            else ndb_error("engine already initialized");
        }

        ~basic_engine()
        {
            instance_ = nullptr;
        }

    private:
        static Engine* instance_;
    };

    template<class Engine>
    Engine* basic_engine<Engine>::instance_ = nullptr;
} // ndb

#endif // ENGINE_BASIC_H_NDB