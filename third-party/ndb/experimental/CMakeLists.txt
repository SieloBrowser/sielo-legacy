cmake_minimum_required(VERSION 3.7)

project(ndb_experimental CXX C)

# exp_main, global tests
add_executable(exp_main main.cpp)
target_link_libraries(exp_main lib_ndb)
add_dependencies(exp_main lib_ndb)
set_target_properties(exp_main PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/experimental"
        OUTPUT_NAME  "exp_main")

# exp_result
if (ENGINE_MONGO)
    add_executable(exp_result result.cpp)
    target_link_libraries(exp_main lib_ndb)
    add_dependencies(exp_main lib_ndb)
    set_target_properties(exp_result PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/experimental"
            OUTPUT_NAME  "exp_result")
endif()

# exp_preprocessor_macro
add_custom_command(
        OUTPUT preprocessor_result
        COMMAND ${CMAKE_C_COMPILER} -E ${NDB_ROOT}/experimental/preprocessor.hpp -I ${THIRD_PARTY_ROOT}/boost
        MAIN_DEPENDENCY preprocessor.hpp
        COMMENT "Preprocessing..."
        VERBATIM
)
add_custom_target(
        exp_preprocessor_macro ALL
        DEPENDS preprocessor_result
        VERBATIM
)

# exp_cx_deduction
add_executable(exp_cx_deduction cx_deduction.cpp)
target_link_libraries(exp_cx_deduction lib_ndb)
add_dependencies(exp_cx_deduction lib_ndb)
set_target_properties(exp_cx_deduction PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/experimental"
        OUTPUT_NAME "exp_cx_deduction")

# database_creation
add_executable(exp_database_creation database_creation.cpp)
target_link_libraries(exp_database_creation lib_ndb)
add_dependencies(exp_database_creation lib_ndb)
set_target_properties(exp_database_creation PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/experimental"
        OUTPUT_NAME  "exp_database_creation")

# exp_preprocessor
add_executable(exp_preprocessor preprocessor.cpp)
target_link_libraries(exp_preprocessor lib_ndb)
add_dependencies(exp_preprocessor lib_ndb)
set_target_properties(exp_preprocessor PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/experimental"
        OUTPUT_NAME  "exp_preprocessor")

# exp_query
add_executable(exp_query query.cpp)
target_link_libraries(exp_query lib_ndb)
add_dependencies(exp_query lib_ndb)
set_target_properties(exp_query PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/experimental"
        OUTPUT_NAME  "exp_query")

# exp_mysql
if (ENGINE_MYSQL)
    add_executable(exp_mysql mysql.cpp)
    target_link_libraries(exp_mysql ${MYSQL_LIB})
    set_target_properties(exp_mysql PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/experimental"
            OUTPUT_NAME  "exp_mysql")
endif()

# exp_mongo
if (ENGINE_MONGO)
    add_executable(exp_mongo mongo.cpp)
    target_link_libraries(exp_mongo ${MONGO_LIB})
    set_target_properties(exp_mongo PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/experimental"
            OUTPUT_NAME  "exp_mongo")
endif()