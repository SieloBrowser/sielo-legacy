cmake_minimum_required(VERSION 3.6)
project(Core)

include_directories(${CMAKE_SOURCE_DIR}/Core)
include_directories(${OPENSSL_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(
        GLOB_RECURSE
        SOURCE_FILES
        AdBlock/*.hpp
        AdBlock/*.cpp
        Bookmarks/*.cpp
        Bookmarks/*.hpp
        Download/*.cpp
        Download/*.hpp
        History/*.cpp
        History/*.hpp
        Network/*.cpp
        Network/*.hpp
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

find_package(OpenSSL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)

add_library(Core ${SOURCE_FILES} ../icons.qrc/)
target_link_libraries(Core ${OPENSSL_LIBRARIES})
target_link_libraries(Core Qt5::Widgets)
target_link_libraries(Core Qt5::Sql)
target_link_libraries(Core Qt5::WebEngine)
target_link_libraries(Core Qt5::WebEngineWidgets)