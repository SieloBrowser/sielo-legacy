#ifndef INITIALIZER_H_NDB
#define INITIALIZER_H_NDB

#include <tuple>

namespace ndb
{
    template<class... Es>
    class initializer
    {
    public:
        initializer() = default;

    private:
        std::tuple<Es...> engine_list_;
    };
} // ndb

#endif // INITIALIZER_H_NDB
