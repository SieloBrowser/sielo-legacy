#ifndef RESULT_H_NDB
#define RESULT_H_NDB

#include <ndb/line.hpp>
#include <vector>

namespace ndb
{
    template<class T, class Engine>
    struct result_encoder;

    // nop encoder
    template<class Database>
    struct result_encoder<ndb::line<Database>, Database>
    {
        static auto& decode(const ndb::line<Database>& line)
        {
            return line;
        }
    };

    template<class Engine, class T = ndb::line<Engine>>
    class result
    {
        using iterator = typename std::vector<T>::iterator;

    private:
        std::vector<T> line_list_;
        int add_id_;

    public:
        result() : add_id_{ 0 } {}

        void add(T l)
        {
            line_list_.push_back(std::move(l));
        }

        int add_id() const
        {
            return add_id_;
        }

        size_t size() const
        {
            return line_list_.size();
        }

        bool has_result() const
        {
            return size() > 0;
        }

        T& operator[](int index)
        {
            return line_list_.at(index);
        }

        // structured binding
        template<int i>
        auto get()
        {
            return line_list_[0];
        }

        iterator begin() { return line_list_.begin(); }
        iterator end() { return line_list_.end(); }
    };
} // ndb

namespace std
{
    template<class T, class Engine>
    struct tuple_size< ::ndb::result<Engine, T>> : std::integral_constant<std::size_t, 1> {};

    template<class T, class Engine>
    struct tuple_element<0, ::ndb::result<Engine, T>> { using type = T; };
} // std

#endif // RESULT_H_NDB