#ifndef ENGINE_NSE_NSE_H_NDB
#define ENGINE_NSE_NSE_H_NDB

// #include <nse/engine.hpp>
#include <ndb/engine/basic.hpp>
#include <ndb/model.hpp>
#include <ndb/utility.hpp>
#include <iostream>

namespace ndb
{
    class nse : basic_engine<nse>
    {
    public:
        template<class Model>
        static constexpr void make()
        {
            ndb::for_each_entity<Model>([](auto&& index, auto&& table)
            {
                std::cout << "\ncreate table : " << table.detail_.name;

                ndb::for_each_entity(table, [](auto&& i, auto&& field)
                {
                    std::cout << "\ncreate field : " << i << " : " << field.detail_.size;
                });
            });
        }
    };
} // db

//TODO: faire un foreach qui parcours tout un model, table et champs

#endif // ENGINE_NSE_NSE_H_NDB