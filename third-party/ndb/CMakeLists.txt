cmake_minimum_required(VERSION 3.7)

project(ndb C CXX)
enable_testing()

message("--------------------------------------------------------
                         NDB
--------------------------------------------------------")

# config file for custom variables
include(config.cmake OPTIONAL)

#-------------------------------------------------------
#                        VARS
#-------------------------------------------------------
# roots
set(NDB_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(NDB_BIN_ROOT ${NDB_ROOT}/bin)
set(THIRD_PARTY_ROOT ${NDB_ROOT}/third_party)

option(NDB_ENGINE_SQLITE "Build sqlite" ${NDB_ENGINE_SQLITE})
option(NDB_ENGINE_MONGO "Build mongo" ${NDB_ENGINE_MONGO})
option(NDB_ENGINE_POSTGRE "Build postgresql" ${NDB_ENGINE_POSTGRE})
option(NDB_BUILD_EXP "Build experimentals" ${NDB_BUILD_EXP})
option(NDB_BUILD_TEST "Build tests" ${NDB_BUILD_TEST})
option(NDB_BUILD_EXAMPLE "Build examples" ${NDB_BUILD_EXAMPLE})

# display config
message("SQLITE : ${NDB_ENGINE_SQLITE}")
message("MONGO : ${NDB_ENGINE_MONGO}")
message("POSTGRESQL : ${NDB_ENGINE_POSTGRE}")
message("BUILD_EXP : ${NDB_BUILD_EXP}")
message("BUILD_TEST : ${NDB_BUILD_TEST}")
message("BUILD_EXAMPLE : ${NDB_BUILD_EXAMPLE}")

add_library(lib_ndb INTERFACE)

#-------------------------------------------------------
#                      COMPILER
#-------------------------------------------------------
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    set(COMPILER_DIRNAME msvc)
    add_compile_options(/wd4710 /wd4251 /std:c++17 /we4309)
endif()

if (MINGW)
    set(COMPILER_DIRNAME mingw)
    set(NDB_LIB stdc++fs)
endif()

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    set(NDB_LIB stdc++fs pthread dl)
endif()

#-------------------------------------------------------
#                       ENGINE
#-------------------------------------------------------
include(third_party/sqlite.cmake)
include(third_party/postgre.cmake)
include(third_party/mongo.cmake)


#-------------------------------------------------------
#                     NDB INTERFACE
#-------------------------------------------------------
target_include_directories(lib_ndb INTERFACE
        include
        ${THIRD_PARTY_ROOT}/boost
        ${THIRD_PARTY_ROOT}/mpl/src
        ${NDB_ENGINE_INCLUDE})
target_link_libraries(lib_ndb INTERFACE ${NDB_ENGINE_LIB} ${NDB_LIB})


#-------------------------------------------------------
#                        SUBDIRS
#-------------------------------------------------------
if (NDB_BUILD_EXP)
    add_subdirectory(experimental)
endif()
if (NDB_BUILD_TEST)
    add_subdirectory(test)
endif()
if (NDB_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()