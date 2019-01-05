add_definitions(-DSIELO_SHAREDLIBRARY)
cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_SOURCE_DIR}/WebEngine/QWebEngine)
include_directories(${CMAKE_SOURCE_DIR}/third-party/includes)

file(
        GLOB_RECURSE
        SOURCE_FILES
        *.hpp
        *.cpp
)

file(
        GLOB_RECURSE
        QRC_FILES
        ${CMAKE_SOURCE_DIR}/WebEngines/QWebEngine/*.qrc
)

add_library(SieloWebEngine STATIC ${SOURCE_FILES} ${QRC_FILES} ${QM_FILES})

set(SENGINE_LIBS Qt5::Widgets Qt5::WebEngine Qt5::Sql Qt5::WebEngineWidgets)
if(WIN32)
    set(SENGINE_LIBS ${SENGINE_LIBS} dwmapi uxtheme)
endif()

target_link_libraries(SieloWebEngine PRIVATE ${SENGINE_LIBS})

foreach(_source IN ITEMS ${SOURCE_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_SOURCE_DIR}/WebEngines/QWebEngine" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

