//! \brief types conversions for engines

#ifndef ENGINE_TYPE_H_NDB
#define ENGINE_TYPE_H_NDB


namespace ndb
{
    // engine ndb_type / type_id
    template<class Engine, class T>
    struct engine_type_id;

    template<class T, int Type_id>
    struct engine_ndb_type;

    template<class Engine, class T>
    struct is_native_type
    {
        static constexpr bool value = false;
    };

    template<class Engine, class T>
    static constexpr auto is_native_type_v = is_native_type<Engine, T>::value;
} // ndb

#endif // ENGINE_TYPE_H_NDB