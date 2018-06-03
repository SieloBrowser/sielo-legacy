#include <ndb/initializer.hpp>
#include <ndb/engine/sqlite/sqlite.hpp>
#include <ndb/engine/sqlite/type.hpp>
#include <ndb/preprocessor.hpp>

ndb_table(window,
ndb_field_id,
    ndb_field(name, std::string, ndb::size<255>),
    ndb_field(x, int),
    ndb_field(y, int)
)

ndb_model(nxi_model, window)

ndb_project(nxi_project,
ndb_database(db_zeta, nxi_model, ndb::sqlite)
)

using db_zeta = ndb::databases::nxi_project::db_zeta_;
namespace ndb
{
    template<class Database>
    void make()
    {
        using Model = typename Database::model;

        std::string output;

        ndb::for_each_entity<Model>([&output](auto&& index, auto&& table)
                                    {
                                        using Table = std::decay_t<decltype(table)>;

                                        output += "\ncreate table if not exists `T" + std::to_string(ndb::table_id<Table>) + "` (";

                                        ndb::for_each_entity(table, [&output](auto&& i, auto&& field)
                                        {
                                            using Field = std::decay_t<decltype(field)>;

                                            if constexpr (decltype(i){} != 0) output += ",";

                                            // field id
                                            output += "\nF" + std::to_string(ndb::field_id<Field>);
                                            bool need_size = false;

                                            // field type
                                            using native_value_type = typename native_type<sqlite, typename std::decay_t<decltype(field)>::value_type>::type;
                                            if constexpr (std::is_same_v<int, native_value_type>) output += " integer ";
                                            if constexpr (std::is_same_v<double, native_value_type>) output += " float ";
                                            if constexpr (std::is_same_v<std::string, native_value_type>)
                                            {
                                                output += " text ";
                                                need_size = true;
                                            }

                                            // field size
                                            if (field.detail_.size > 0 && need_size) output += "(" + std::to_string(field.detail_.size) + ")";

                                            // field option
                                            if (field.detail_.is_primary || field.detail_.is_oid) output += " primary key";
                                            if (field.detail_.is_auto_increment) output += " autoincrement";
                                            if (field.detail_.is_not_null) output += " not null";
                                            if (field.detail_.is_unique) output += " unique";

                                            // field entity
                                            if constexpr (ndb::is_field_entity<Field>)
                                            {
                                                /*
                                                auto store_type = typename Field::value_type{};

                                                output += " integer ";
                                                output += "(" + std::to_string(sizeof(int)) + ")";
                                                output += "\n";
                                                output += typeid(store_type).name();*/

                                                //output += "\n\tentity type " + std::string(store_type.detail_.name);
                                                //output += "\n\tentity count : " + std::to_string(field.detail_.size);
                                                //output += "\n\ttotal size : " + std::to_string(store_type.detail_.size);

                                                //if constexpr (ndb::is_field_entity_vector<Field>) output += " VECTOR";
                                            }
                                        });
                                        output += ");";

                                        // exec create table
                                        std::cout << output;

                                        output = "";
                                    });
    }
} // ndb


int main()
{
    try
    {
        ndb::initializer<ndb::sqlite> init;
        ndb::connect<db_zeta, ndb::sqlite>();

        ndb::make<db_zeta>();

    } catch(const std::exception& e)
    {
        std::cout << e.what();
    }


    return 1;
}