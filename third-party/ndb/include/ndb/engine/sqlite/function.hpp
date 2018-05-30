#include <ndb/function/generic.hpp>
#include <ndb/engine.hpp>
#include <ndb/utility.hpp>

namespace ndb::functions
{
    template<class Database, class Table>
    struct clear<ndb::sqlite, Database, Table, ndb::expr_category_code::sql>
    {
        static auto process()
        {
            const auto& engine = ndb::engine<ndb::sqlite>::get();
            auto q = "DELETE FROM `T" + std::to_string(ndb::table_id<Table>) + "`";
            return engine.template exec<Database>(q);
        }
    };
} // ndb::functions