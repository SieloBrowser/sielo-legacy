cmake_minimum_required(VERSION 3.7)

project(ndb_example CXX C)

function(target_make target_type target_file)
    set(targetname ${target_type}_${target_file})

    add_executable(${targetname} ${target_file}.cpp)
    target_link_libraries(${targetname} lib_ndb)
    add_dependencies(${targetname} lib_ndb)
    set_target_properties(${targetname} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/${target_type}"
            OUTPUT_NAME  ${target_file})

endfunction()

target_make(example basic)
target_make(example object_query)