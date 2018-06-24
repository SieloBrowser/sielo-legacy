cmake_minimum_required(VERSION 3.6)
project(sielo-compiler)

include_directories(${CMAKE_SOURCE_DIR}/SNCompiler)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64bit
    set (CMAKE_PREFIX_PATH "C:\\Qt\\5.11.1\\msvc2017_64\\")
elseif(WIN32)
    set (CMAKE_PREFIX_PATH "C:\\Qt\\5.11.0\\msvc2015\\")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (CMAKE_PREFIX_PATH "/Users/feldrise/Qt/5.11.1/clang_64/")


endif()

if(GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

file(
        GLOB_RECURSE
        SOURCE_FILES
        Main.cpp
        Application.hpp
        Application.cpp
)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)

add_executable(sielo-compiler ${SOURCE_FILES})

target_link_libraries(sielo-compiler Qt5::Widgets)
target_link_libraries(sielo-compiler Qt5::WebEngine)
target_link_libraries(sielo-compiler Qt5::WebEngineWidgets)
