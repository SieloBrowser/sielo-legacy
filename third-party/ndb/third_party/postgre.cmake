if (NDB_ENGINE_POSTGRE)

    set(NDB_PQ_LIB_NAMES libpq libpq5 pq pq5)

    # user defiened path
    if (NDB_PQ_PATH)
        find_library(NDB_PQ_LIB NAMES ${NDB_PQ_LIB_NAMES} HINTS "${NDB_PQ_PATH}/lib")
        find_path(NDB_PQ_INCLUDE NAMES libpq-fe.h HINTS ${NDB_PQ_PATH}/include)
    endif()

    # system path
    if (NOT NDB_PQ_LIB)
        find_library(NDB_PQ_LIB NAMES ${NDB_PQ_LIB_NAMES})
        if (NDB_PQ_LIB)
            find_path(NDB_PQ_INCLUDE NAMES libpq-fe.h)
        else()
            # use third_party lib

        endif()
    endif()


    if (NOT NDB_PQ_LIB)
        message(FATAL_ERROR  "PostgreSQL not found")
    endif()

    list(APPEND NDB_ENGINE_INCLUDE ${NDB_PQ_INCLUDE})
    list(APPEND NDB_ENGINE_LIB ${NDB_PQ_LIB})

endif()