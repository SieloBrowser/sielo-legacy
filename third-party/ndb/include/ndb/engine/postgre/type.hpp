#ifndef ENGINE_POSTGRE_TYPE_H_NDB
#define ENGINE_POSTGRE_TYPE_H_NDB

#pragma once

#include <ndb/engine/type.hpp>
#include <ndb/type.hpp>

#include <postgresql/server/catalog/pg_type.h>

namespace ndb
{
    class postgre;

    ndb_engine_type_id_map(postgre, null_, VOIDOID);
    ndb_engine_type_id_map(postgre, int_, INT8OID);
    ndb_engine_type_id_map(postgre, double_, FLOAT8OID);
    ndb_engine_type_id_map(postgre, string_, VARCHAROID);
    ndb_engine_type_id_map(postgre, byte_array_, BYTEAOID);
} // ndb


#endif // ENGINE_POSTGRE_TYPE_H_NDB
