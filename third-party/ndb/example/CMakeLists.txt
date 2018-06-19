cmake_minimum_required(VERSION 3.7)

project(ndb_example CXX C)

function(example_make target_file)
    set(targetname example_${target_file})

    add_executable(${targetname} ${target_file}.cpp)
    target_link_libraries(${targetname} lib_ndb)
    add_dependencies(${targetname} lib_ndb)
    set_target_properties(${targetname} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${NDB_BIN_ROOT}/example"
            OUTPUT_NAME  ${target_file})

endfunction()

example_make(basic)
example_make(object_query)