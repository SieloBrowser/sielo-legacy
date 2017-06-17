cmake_minimum_required(VERSION 3.6)
project(sielo-browser)

add_subdirectory(Core)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Core)
include_directories(${CMAKE_SOURCE_DIR}/Display)

set(SOURCE_FILES Main.cpp)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)

add_executable(sielo-browser ${SOURCE_FILES} icons.qrc data.qrc)
target_link_libraries(sielo-browser Core)
target_link_libraries(sielo-browser Qt5::Widgets)
target_link_libraries(sielo-browser Qt5::WebEngine)
target_link_libraries(sielo-browser Qt5::WebEngineWidgets)
