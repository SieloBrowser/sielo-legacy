#ifndef OPTION_H_NDB
#define OPTION_H_NDB

#include <ndb/utility.hpp>

namespace ndb
{
    // template options
    struct query_option
    {
        struct object;
    };

    struct field_option
    {
        struct none { static constexpr auto value = 0; };
        struct unique { static constexpr auto value = 1; };
        struct auto_increment { static constexpr auto value = 2; };
        struct primary { static constexpr auto value = 4; };
        struct not_null { static constexpr auto value = 8; };
        struct oid { static constexpr auto value = primary::value; };
    };

    template<class...>
    class option {};

    template<class T, class Options>
    using has_option = internal::has_type<T, Options>;

    template<class T, class Options>
    static constexpr bool has_option_v = has_type_v<T, Options>;

} // ndb

#endif // OPTION_H_NDB