#ifndef LINE_H_NDB
#define LINE_H_NDB

#include <ndb/type.hpp>
#include <ndb/utility.hpp>
#include <ndb/engine/type.hpp>
#include <ndb/error.hpp>
#include <ndb/value.hpp>
#include <unordered_map>
#include <vector>

namespace ndb
{
    template<class Database>
    class line
    {
    public:
        void add(int field_id, ndb::value field_value)
        {
            // value accessible by field
            if (field_id >= 0) value_index_.emplace(field_id, static_cast<unsigned int>(values_.size()));
            values_.emplace_back(std::move(field_value));
        }

        size_t field_count() const
        {
            return values_.size();
        }

        template<class Field, class Field_value_type = typename Field::value_type>
        auto get(const Field& f, Field_value_type value_if_null)
        {
            if (!value_index_.count(ndb::field_id<Field>)) ndb_error("Field does not exist in the result, check the select clause");
            size_t index = value_index_.at(ndb::field_id<Field>);
            auto& value = values_.at(index);

            using Engine = typename Database::engine;
            using native_type = ndb::native_type<typename Field::value_type, Database>;

            constexpr auto b = ndb::is_native_type_v<Engine, Field_value_type>; // msvc crash, separate in 2 lines
            if constexpr (b)
            {
                if (value.is_null()) return value_if_null;
                else return value.get<native_type>();
            }
            else
            {
                if (value.is_null()) return value_if_null;
                else return ndb::custom_type<Field_value_type, Database>::template decode<Field_value_type>(value.get<native_type>());
            }
        }

        ndb::value operator[](int index)
        {
            if (index >= values_.size()) ndb_error("ndb::value out of range");
            return values_.at(index);
        }

        template<class Field>
        typename Field::value_type operator[](const Field& f) const
        {
            if (!value_index_.count(ndb::field_id<Field>)) ndb_error("Field does not exist in the result, check the select clause");
            size_t index = value_index_.at(ndb::field_id<Field>);
            auto& value = values_.at(index);

            using Engine = typename Database::engine;
            using native_type = ndb::native_type<typename Field::value_type, Database>;
            using value_type = typename Field::value_type;

            constexpr auto b = ndb::is_native_type_v<Engine, value_type>; // msvc crash, separate in 2 lines
            if constexpr (b)
            {
                return value.get<native_type>();
            }
            else return ndb::custom_type<value_type, Database>::template decode<value_type>(value.get<native_type>());
        }

    private:
        std::vector<ndb::value> values_;
        std::unordered_map<unsigned int, unsigned int> value_index_;
    };
} // ndb



#endif // LINE_H_NDB