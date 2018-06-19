#ifndef EXPRESSION_SQL_H_NDB
#define EXPRESSION_SQL_H_NDB

#include <ndb/expression/sql/code.hpp>
#include <ndb/expression/sql/native.hpp>
#include <ndb/expression/sql/type.hpp>

#include <ndb/utility.hpp>
#include <array>

namespace ndb
{
    template<class Expr>
    class expression_size
    {
    public:
        static constexpr auto expr_category = expr_category_code::sql;

        bool deduced_source = false;

        constexpr explicit expression_size() :
            value_(0)
        {
            Expr::template static_make(*this);
        }

        constexpr void push_back(char v)
        {
            value_++;
        }

        constexpr void push_back_value()
        {
            value_ += 2;
        }

        constexpr void push_back(const char *v)
        {
            value_ += cx_str_len(v);
        }

        constexpr auto value() const
        {
            return value_;
        }

    private:
        size_t value_;
    };

    template<class Expr>
    class sql_expression
    {
    public:
        bool deduced_source = false;

        static constexpr auto expr_category = expr_category_code::sql;
        static constexpr auto Capacity = expression_size<Expr>{}.value();

        constexpr explicit sql_expression() :
            size_{ 0 },
            value_index_{ 1 },
            data_{}
        {
            Expr::template static_make(*this);
        }

        constexpr char& operator[](unsigned int n)
        {
            return data_[n];
        }

        constexpr const char& at(unsigned int n) const
        {
            return data_.at(n);
        }

        constexpr size_t capacity() const
        {
            return Capacity;
        }

        constexpr void push_back(char v)
        {
            data_[size_] = v;
            size_++;
        }

        constexpr void push_back_value()
        {
            data_[size_] = '?';
            data_[size_ + 1] = '0' + value_index_;
            size_ += 2;
            value_index_++;
        }

        constexpr void push_back(const char* v)
        {
            auto s = cx_str_len(v);
            for (int i = 0; i != s; i++) data_[size_ + i] = v[i];
            size_ += s;
        }

        constexpr const char* c_str() const
        {
            return data_.data();
        }

        std::string to_string() const
        {
            std::string result;
            for (int i = 0; i != Capacity; i++)
            {
                if (data_.at(i) == 0) result.push_back('_');
                else result.push_back(data_.at(i));
            }

            return result;
        }

    private:
        size_t size_;
        unsigned int value_index_;
        std::array<char, Capacity> data_;
    };
} // ndb

#endif // EXPRESSION_SQL_H_NDB
