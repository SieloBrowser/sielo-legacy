add_definitions(-DNDB_ENGINE_MONGO)

if (WIN32)
    set(MONGOC_LIB_WINDOWS ws2_32 Secur32 Crypt32 BCrypt)
endif()

# BSON
set(BSON_SEARCH_LIB libbson-static-1.0 bson-static-1.0 libbson-1.0 bson-1.0)

if (NOT BSON_LIB)
    find_library(BSON_LIB NAMES ${BSON_SEARCH_LIB})
    if (BSON_LIB)
        find_path(BSON_INCLUDE NAMES libbson-1.0/bcon.h)
    else()
        # use third_party lib
        find_library(BSON_LIB NAMES ${BSON_SEARCH_LIB} HINTS "${THIRD_PARTY_ROOT}/libbson-1.0/lib")
        find_path(BSON_INCLUDE NAMES libbson-1.0/bcon.h HINTS ${THIRD_PARTY_ROOT}/libbson-1.0/include)
    endif()
endif()

# MONGOC
set(MONGOC_SEARCH_LIB libmongoc-static-1.0 mongoc-static-1.0 libmongoc-1.0 mongoc-1.0)

if (NOT MONGOC_LIB)
    find_library(MONGOC_LIB NAMES ${MONGOC_SEARCH_LIB})
    if (MONGOC_LIB)
        find_path(MONGOC_INCLUDE NAMES libmongoc-1.0/mongoc.h)
    else()
        # use third_party lib
        find_library(MONGOC_LIB NAMES ${MONGOC_SEARCH_LIB} HINTS "${THIRD_PARTY_ROOT}/libmongoc-1.0/lib")
        find_path(MONGOC_INCLUDE NAMES libmongoc-1.0/mongoc.h HINTS ${THIRD_PARTY_ROOT}/libmongoc-1.0/include)
    endif()
endif()

set(MONGO_LIB ${MONGOC_LIB} ${BSON_LIB} ${MONGOC_LIB_WINDOWS})
set(MONGO_INCLUDE "${MONGOC_INCLUDE}/libmongoc-1.0 " "${BSON_INCLUDE}/libbson-1.0 ")

list(APPEND ENGINE_LIB ${MONGO_LIB})
list(APPEND ENGINE_INCLUDE ${MONGO_INCLUDE})

