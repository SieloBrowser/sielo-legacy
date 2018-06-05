cmake_minimum_required(VERSION 3.7)

project(ndb_experimental CXX C)

function(exp_make target_file)
    set(targetname exp_${target_file})

    add_executable(${targetname} ${target_file}.cpp)
    target_link_libraries(${targetname} lib_ndb)
    add_dependencies(${targetname} lib_ndb)
    set_target_properties(${targetname} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/experimental"
            OUTPUT_NAME  ${target_file})

endfunction()

exp_make(main)
exp_make(query)
exp_make(cx_deduction)
exp_make(postgre)




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