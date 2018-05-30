cmake_minimum_required(VERSION 3.7)

project(test_ndb)

add_library(lib_gtest OBJECT ${THIRD_PARTY_ROOT}/gtest/source/gtest-all.cc)
set(GTEST_LIB $<TARGET_OBJECTS:lib_gtest>)

include_directories(${THIRD_PARTY_ROOT}/gtest/include ../)

file(GLOB_RECURSE SOURCES "source/*.cpp")
add_executable(test_ndb ${SOURCES})
add_dependencies(test_ndb lib_ndb lib_gtest)
target_link_libraries(test_ndb lib_ndb ${GTEST_LIB})


set_target_properties(test_ndb PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/test"
        OUTPUT_NAME  "test_ndb")

add_test(NAME test_ndb COMMAND "${NDB_BIN_ROOT}/test/test_ndb")