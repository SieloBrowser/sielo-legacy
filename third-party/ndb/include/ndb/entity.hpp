#ifndef ENTITY_H_NDB
#define ENTITY_H_NDB

//! \brief represent a database entity
//! \details entity<field1, field2>, fields inherit from field_base

#include <tuple>
#include <utility>
#include <cstddef>
#include <ndb/utility.hpp>

namespace ndb
{
    template<class... Fields>
    struct entity
    {
        template<size_t N>
        using item_at = std::tuple_element_t<N, std::tuple<Fields...>>;

        static constexpr auto count()
        {
            return sizeof...(Fields);
        }

        static constexpr auto size()
        {
            std::size_t sum = 0;
            using expand = std::size_t[];
            (void)expand{(sum += Fields::Detail_::size)...};
            return sum;
        }

        template<size_t N>
        static constexpr auto item_size()
        {
            return item_at<N>{}.detail_.size;
        }

        template<std::size_t N>
        static constexpr auto offset()
        {
            static_assert(N < sizeof...(Fields), "Error : Index is higher than parameter pack");
            std::size_t sum = 0;
            ndb::for_each<Fields...>([&sum](auto&& index, auto&& field)
            {
                if (index >= N) return;
                sum += field.detail_.size;
            });
            return sum;
        }
    };
} // ndb

#endif // ENTITY_H_NDB
