cmake_minimum_required(VERSION 3.6)
project(Core)

include_directories(${CMAKE_SOURCE_DIR}/Core)
include_directories(${CMAKE_SOURCE_DIR}/third-party/includes)

if(WIN64 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64bit
    set (CMAKE_PREFIX_PATH "C:\\Qt\\5.10.1\\msvc2017_64\\")
    set (ENV{OPENSSL_ROOT_DIR} "${CMAKE_SOURCE_DIR}\\third-party\\openssl\\windows\\x64\\")
    set (ENV{OPENSSL_CRYPTO_LIBRARY} "${CMAKE_SOURCE_DIR}\\third-party\\openssl\\windows\\x64\\")
elseif(WIN32)
    set (CMAKE_PREFIX_PATH "C:\\Qt\\5.10.1\\msvc2015\\")
    set (ENV{OPENSSL_ROOT_DIR} "${CMAKE_SOURCE_DIR}\\third-party\\openssl\\windows\\x86\\")
    set (ENV{OPENSSL_CRYPTO_LIBRARY} "${CMAKE_SOURCE_DIR}\\third-party\\openssl\\windows\\x86\\")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (CMAKE_OSX_SYSROOT "${CMAKE_SOURCE_DIR}/MacOSX10.11.sdk/")
    set (CMAKE_OSX_DEPLOYMENT_TARGET "10.11")
    set (CMAKE_PREFIX_PATH "/Users/feldrise/Qt/5.10.1/clang_64/")
    set (ENV{OPENSSL_ROOT_DIR} "${CMAKE_SOURCE_DIR}/third-party/openssl/macos/")
    set (ENV{OPENSSL_CRYPTO_LIBRARY} "${CMAKE_SOURCE_DIR}/third-party/openssl/macos/")
endif()

if(GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

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

# Qt5LinguistTools
find_package(Qt5LinguistTools)
FILE(GLOB TS_FILES "${CMAKE_SOURCE_DIR}/translations/*.ts")
QT5_create_translation(QM_FILES ${SOURCE_FILES} ${TS_FILES})

add_library(Core ${SOURCE_FILES} ../icons.qrc/ ${QM_FILES})

target_link_libraries(Core ${OPENSSL_LIBRARIES})
target_link_libraries(Core Qt5::Widgets)
target_link_libraries(Core Qt5::Sql)
target_link_libraries(Core Qt5::WebEngine)
target_link_libraries(Core Qt5::WebEngineWidgets)

