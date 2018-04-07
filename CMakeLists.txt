cmake_minimum_required(VERSION 3.6)
project(sielo-browser)

add_subdirectory(Core)
add_subdirectory(SNCompiler)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Core)
include_directories(${CMAKE_SOURCE_DIR}/third-party/includes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(SOURCE_FILES Main.cpp)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenSSL 1.1.0 REQUIRED)
find_package(Qt5Widgets 5.8 REQUIRED)
find_package(Qt5Sql 5.8 REQUIRED)
find_package(Qt5WebEngine 5.8 REQUIRED)
find_package(Qt5WebEngineWidgets 5.8 REQUIRED)

add_executable(sielo-browser ${SOURCE_FILES} icons.qrc data.qrc)

target_link_libraries(sielo-browser Core)
target_link_libraries(sielo-browser ${OPENSSL_LIBRARIES})
target_link_libraries(sielo-browser Qt5::Widgets)
target_link_libraries(sielo-browser Qt5::Sql)
target_link_libraries(sielo-browser Qt5::WebEngine)
target_link_libraries(sielo-browser Qt5::WebEngineWidgets)
