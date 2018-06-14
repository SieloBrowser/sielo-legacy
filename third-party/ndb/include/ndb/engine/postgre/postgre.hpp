#ifndef ENGINE_POSTGRE_POSTGRE_H_NDB
#define ENGINE_POSTGRE_POSTGRE_H_NDB

#include <ndb/engine/basic.hpp>
#include <ndb/engine/postgre/connection.hpp>

namespace ndb
{
    template<class Database>
    class postgre_query;

    template<class Database>
    class line;

    class postgre : public basic_engine<postgre>
    {
    public:
        inline postgre() = default;

        template<class Database, class Result_type = ndb::line<Database>>
        inline auto exec(const postgre_query<Database>& query) const;

        template<class Database, class Query_option, class Expr>
        inline auto exec(const Expr& expr) const;

        template<class Database>
        inline void make();
    };
} // ndb

#include <ndb/engine/postgre/postgre.tpp>

#endif // ENGINE_POSTGRE_POSTGRE_H_NDB