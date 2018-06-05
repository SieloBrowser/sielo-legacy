#ifndef VALUE_H_NDB
#define VALUE_H_NDB

#include <ndb/error.hpp>
#include <ndb/type.hpp>

#include <string>
#include <vector>
#include <variant>

namespace ndb
{
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

        template<class T>
        const T& get() const
        {
            if (auto value = std::get_if<T>(&value_)) return *value;
            else ndb_error("Can't get unknown type");
        }

        template<class T>
        T& get()
        {
            return const_cast<T&>(static_cast<const ndb::value&>(*this).get<T>());
        }

        bool is_null() const
        {
            return is_null_;
        }

    private:
        bool is_null_;
        std::variant<int, double, float, std::string, std::vector<char>> value_;
    };
} // ndb

#endif // VALUE_H_NDB
