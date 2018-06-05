#ifndef DATABASE_H_NDB
#define DATABASE_H_NDB

namespace ndb
{
    class database_base{};

    template<class Group, class Model, class Engine>
    struct database : Model, database_base
    {
        using group = Group;
        using model = Model;
        using engine = Engine;
    };

    template<class Entity>
    struct database_detail
    {
        using entity = Entity;
        template<class>
        using default_base = typename Entity::template item_at<0>;
    };
} // ndb

#endif // DATABASE_H_NDB
