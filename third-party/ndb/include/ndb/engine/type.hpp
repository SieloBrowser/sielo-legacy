//! \brief types conversions for engines

#ifndef ENGINE_TYPE_H_NDB
#define ENGINE_TYPE_H_NDB

namespace ndb
{
    #define ndb_engine_type_id_map(ENGINE, NDB_TYPE, ENGINE_TYPE_ID) \
    template<> struct engine_type_ndb<ENGINE, ENGINE_TYPE_ID> { using type = NDB_TYPE; }; \
    template<> struct engine_type_id<ENGINE, NDB_TYPE> { static constexpr auto value = ENGINE_TYPE_ID; }

    // engine ndb_type / type_id
    template<class Engine, class T>
    struct engine_type_id;

    template<class T, int Type_id>
    struct engine_type_ndb;

    // engine types
    template<class Engine, class Database>
    struct engine_types;

    template<class Database>
    using engine_variant_t = typename engine_types<typename Database::engine, Database>::type;

    // engines can specialize storage types (ex: sqlite use int_ for datetime_)
    template<class Engine, class T>
    struct storage_type
    {
        using type = T;
    };

    template<class Engine, class T>
    using storage_type_t = typename storage_type<Engine, T>::type;
} // ndb

#endif // ENGINE_TYPE_H_NDB