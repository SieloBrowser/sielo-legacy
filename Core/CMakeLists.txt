cmake_minimum_required(VERSION 3.6)
project(Core)

include_directories(${CMAKE_SOURCE_DIR}/Core)
include_directories(${CMAKE_SOURCE_DIR}/third-party/includes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(
        GLOB_RECURSE
        SOURCE_FILES
        3rdparty/*.h
        3rdparty/*.cpp
        AdBlock/*.hpp
        AdBlock/*.cpp
        Bookmarks/*.cpp
        Bookmarks/*.hpp
        Cookies/*.hpp
        Cookies/*.cpp
        Download/*.cpp
        Download/*.hpp
        History/*.cpp
        History/*.hpp
        Network/*.cpp
        Network/*.hpp
        Password/*.cpp
        Password/*.hpp
        Plugins/*.cpp
        Plugins/*.hpp
        Utils/*.cpp
        Utils/*.hpp
        View/*.cpp
        View/*.hpp
        Web/*.cpp
        Web/*.hpp
        Widgets/*.cpp
        Widgets/*.hpp
        Application.cpp
        Application.hpp
        BrowserWindow.hpp
        BrowserWindow.cpp
)

find_package(OpenSSL 1.1.0 REQUIRED)
find_package(Qt5Widgets 5.8 REQUIRED)
find_package(Qt5Sql 5.8 REQUIRED)
find_package(Qt5WebEngine 5.8 REQUIRED)
find_package(Qt5WebEngineWidgets 5.8 REQUIRED)

add_library(Core ${SOURCE_FILES} ../icons.qrc/)

target_link_libraries(Core ${OPENSSL_LIBRARIES})
target_link_libraries(Core Qt5::Widgets)
target_link_libraries(Core Qt5::Sql)
target_link_libraries(Core Qt5::WebEngine)
target_link_libraries(Core Qt5::WebEngineWidgets)