#include <ndb/type.hpp>
#include <ndb/type/map.hpp>
#include <ndb/type/custom.hpp>

#include <chrono>
#include <vector>

namespace ndb
{
    // system type_map
    #define ndb_internal_bijective_type_map(NDB_TYPE, CPP_TYPE) ndb_bijective_type_map(NDB_TYPE, CPP_TYPE, scope::system)

    ndb_internal_bijective_type_map(bool_, bool);
    ndb_internal_bijective_type_map(int_, int);
    ndb_internal_bijective_type_map(int64_, int64_t);
    ndb_internal_bijective_type_map(float_, float);
    ndb_internal_bijective_type_map(double_, double);
    ndb_internal_bijective_type_map(string_, std::string);
    ndb_internal_bijective_type_map(byte_array_, std::vector<char>);

    #undef ndb_internal_bijective_type_map

    // spe type_make
    template<>
    inline auto type_make<std::vector<char>>(const char* data, int data_size)
    {
        return std::vector<char>{ data, data + data_size };
    }

    // system custom_type
    template<class Database>
    struct custom_type<const char*, Database> : basic_type<const char*, string_, Database>
    {
        static auto encode(const char* v)
        {
            return cpp_type_t<string_, Database>{ v };
        }
    };

    template<class T, class U, class Database>
    struct custom_type<std::chrono::duration<T, U>, Database> : basic_type<std::chrono::duration<T, U>, double_, Database>
    {
        static auto encode(const std::chrono::duration<T, U>& v)
        {
            return v.count();
        }
    };
} // ndb
