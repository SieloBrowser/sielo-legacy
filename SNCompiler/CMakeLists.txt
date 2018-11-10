cmake_minimum_required(VERSION 3.6)
project(sielo-compiler)

include_directories(${CMAKE_SOURCE_DIR}/SNCompiler)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# user config config file
include("${CMAKE_SOURCE_DIR}\\user-config.cmake")

file(
        GLOB_RECURSE
        SOURCE_FILES
        Main.cpp
        Application.hpp
        Application.cpp
)

find_package(Qt5Widgets 5.11.2 REQUIRED)
find_package(Qt5WebEngine 5.11.2 REQUIRED)
find_package(Qt5WebEngineWidgets 5.11.2 REQUIRED)

add_executable(sielo-compiler ${SOURCE_FILES})

target_link_libraries(sielo-compiler Qt5::Widgets)
target_link_libraries(sielo-compiler Qt5::WebEngine)
target_link_libraries(sielo-compiler Qt5::WebEngineWidgets)
