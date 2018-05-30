#ifndef SETUP_H_NDB
#define SETUP_H_NDB

//! \brief compile time configuration

namespace ndb
{
    class global;
    template<class T = global> struct setup {};
} // ndb


#include <string>

namespace ndb
{
    // engine fwd
    class sqlite;
    class mongo;
    class nse;

    // default engine
    template<>
    struct setup<>
    {
        using engine = sqlite;
    };

    template<class Engine = setup<>::engine>
    class basic_engine;

    template<class Engine>
    struct setup<basic_engine<Engine>>
    {
        static constexpr auto path  = "./database/";
    };

    template<>
    struct setup<sqlite>
    {
        //using expression_type = std::string;
        static constexpr auto path = setup<basic_engine<sqlite>>::path;
        static constexpr auto ext = ".sql.db";
    };
} // ndb

#endif // SETUP_H_NDB