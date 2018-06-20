#ifndef VALUE_H_NDB
#define VALUE_H_NDB

#include <ndb/error.hpp>
#include <ndb/type.hpp>

#include <variant>

namespace ndb
{
    template<class Database>
    class value
    {
    public:
        template<class T>
		value(T v) :
            is_null_{ false },
            value_{ std::move(v) }
        {}

        value(ndb::null_type) :
            is_null_{ true }
        {}

        // get decoded value
        template<class Field>
        typename Field::value_type decode() const
        {
            if (is_null()) ndb_error("Field is null");

            using Engine = typename Database::engine;
            using value_type = typename Field::value_type;
            using value_ndb_type = ndb::ndb_type_t<value_type, Database>;
            using value_storage_type = ndb::storage_type_t<Engine, value_ndb_type>;

            constexpr bool b_storage_type = ndb::is_storage_type_v<Engine, value_ndb_type>;
            constexpr bool b_custom_type = ndb::is_custom_type_v<value_type, Database>;



            // custom type
            if constexpr (b_custom_type) // msvc crash, separate in 2 lines
            {
                return ndb::custom_type<value_type, Database>::internal_decode(get< ndb::cpp_type_t<value_storage_type, Database> >());
            }
            // storage type (mapped by engine)
            else if constexpr (b_storage_type)
            {
                return value_type( get< ndb::cpp_type_t<value_storage_type, Database> >() );
            }
            // native type
            else return get<value_type>();
        }

        // get ref on engine storage type
        template<class T>
        const T& get() const
        {
            if (auto value = std::get_if<T>(&value_)) return *value;
            else ndb_error("Can't get unknown type");
        }

        template<class T>
        T& get()
        {
            return const_cast<T&>(static_cast<const ndb::value<Database>&>(*this).get<T>());
        }

        bool is_null() const
        {
            return is_null_;
        }

    private:
        bool is_null_;
        ndb::engine_variant_t<Database> value_;
    };
} // ndb

#endif // VALUE_H_NDB
