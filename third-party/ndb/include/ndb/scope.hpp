#ifndef SCOPE_H_NDB
#define SCOPE_H_NDB

namespace ndb
{
    namespace scope
    {
        struct system;
        struct global;

        template<class Database>
        struct database;

        template<class Group>
        struct group;

        template<class Engine>
        struct engine;
    } // scope

    namespace internal
    {
        struct scope_not_found;

        // type scope
        template<class Database, class Scope>
        struct next_scope;

        template<class Database>
        struct next_scope<Database, scope::system>
        {
            using type = scope_not_found;
        };
        template<class Database>
        struct next_scope<Database, scope::global>
        {
            using type = scope::system;
        };
        template<class Database, class Engine>
        struct next_scope<Database, scope::engine<Engine>>
        {
            using type = scope::global;
        };
        template<class Database, class Group>
        struct next_scope<Database, scope::group<Group>>
        {
            using type = scope::engine<typename Database::engine>;
        };
        template<class Database>
        struct next_scope<Database, scope::database<Database>> {
        using type = scope::group<typename Database::group>; };

        // spe for unspecified database
        template<>
        struct next_scope<void, scope::database<void>> {
        using type = scope::global; };
        template<class Group>
        struct next_scope<void, scope::group<Group>> {
        using type = scope::global; };
    } // internal
} // ndb

#endif // SCOPE_H_NDB