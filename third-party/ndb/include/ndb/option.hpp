#ifndef OPTION_H_NDB
#define OPTION_H_NDB

namespace ndb
{
    enum class connection_flag
    {
        default_ = 0,
        read_only = 1
    };

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

    template<class Needle, class Haystack>
    class has_option;

    template<class Needle, template<class...> class Haystack, class T1, class... T>
    struct has_option<Needle, Haystack<T1, T...>> { static constexpr bool value = has_option<Needle, Haystack<T...>>::value; };

    template<class Needle, template<class...> class Haystack>
    struct has_option<Needle, Haystack<>> : std::false_type {};

    template<class Needle, template<class...> class Haystack, class... T>
    struct has_option<Needle, Haystack<Needle, T...>> : std::true_type {};

    template<class Needle, class Haystack>
    constexpr auto has_option_v = has_option<Needle, Haystack>::value;

} // ndb

#endif // OPTION_H_NDB