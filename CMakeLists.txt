cmake_minimum_required(VERSION 3.6)
project(SieloBrowser)

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

add_executable(SieloBrowser ${SOURCE_FILES} icons.qrc)
target_link_libraries(SieloBrowser Core)
target_link_libraries(SieloBrowser Qt5::Widgets)
target_link_libraries(SieloBrowser Qt5::WebEngine)
target_link_libraries(SieloBrowser Qt5::WebEngineWidgets)
