cmake_minimum_required(VERSION 3.6)
project(Core)

include_directories(${CMAKE_SOURCE_DIR}/Core)
set(CMAKE_CXX_STANDARD 11)
file(
        GLOB_RECURSE
        SOURCE_FILES
        Application.cpp
        Application.hpp
        Utils/*.cpp
        Utils/*.hpp
        Web/*.cpp
        Web/*.hpp
        Plugins/*.cpp
        Plugins/*.hpp
)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)

add_library(Core ${SOURCE_FILES})
target_link_libraries(Core Qt5::Widgets)
target_link_libraries(Core Qt5::WebEngine)
target_link_libraries(Core Qt5::WebEngineWidgets)