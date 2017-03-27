cmake_minimum_required(VERSION 3.6)
project(Display)

include_directories(${CMAKE_SOURCE_DIR}/Display)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(
        GLOB_RECURSE
        SOURCE_FILES
        View/*.cpp
        View/*.hpp
        History/*.cpp
        History/*.hpp
)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)

add_library(Display ${SOURCE_FILES})
target_link_libraries(Display Qt5::Widgets)
target_link_libraries(Display Qt5::WebEngine)
target_link_libraries(Display Qt5::WebEngineWidgets)