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
        void add(int field_id, ndb::value<Database> field_value)
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
            size_t value_index = value_index_.at(ndb::field_id<Field>);
            const ndb::value<Database>& value = values_.at(value_index);

            if (value.is_null()) return value_if_null;
            else return value.template decode<Field>();
        }

        ndb::value<Database> operator[](int index)
        {
            if (index >= values_.size()) ndb_error("ndb::value out of range");
            return values_.at(index);
        }

        template<class Field>
        typename Field::value_type operator[](const Field& f) const
        {
            if (!value_index_.count(ndb::field_id<Field>)) ndb_error("Field does not exist in the result, check the select clause");
            size_t value_index = value_index_.at(ndb::field_id<Field>);
            const ndb::value<Database>& value = values_.at(value_index);

            return value.template decode<Field>();
        }

    private:
        std::vector<ndb::value<Database>> values_;
        std::unordered_map<unsigned int, unsigned int> value_index_;
    };
} // ndb



#endif // LINE_H_NDB