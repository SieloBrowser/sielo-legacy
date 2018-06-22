cmake_minimum_required(VERSION 3.6)
project(Core)

include_directories(${CMAKE_SOURCE_DIR}/Core)
include_directories(${CMAKE_SOURCE_DIR}/third-party/includes)

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64bit
    set (CMAKE_PREFIX_PATH "C:\\Qt\\5.11.1\\msvc2017_64\\")
    set (ENV{OPENSSL_ROOT_DIR} "${CMAKE_SOURCE_DIR}\\third-party\\openssl\\windows\\x64\\")
    set (ENV{OPENSSL_CRYPTO_LIBRARY} "${CMAKE_SOURCE_DIR}\\third-party\\openssl\\windows\\x64\\")
elseif(WIN32)
    set (CMAKE_PREFIX_PATH "C:\\Qt\\5.11.0\\msvc2015\\")
    set (ENV{OPENSSL_ROOT_DIR} "${CMAKE_SOURCE_DIR}\\third-party\\openssl\\windows\\x86\\")
    set (ENV{OPENSSL_CRYPTO_LIBRARY} "${CMAKE_SOURCE_DIR}\\third-party\\openssl\\windows\\x86\\")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (CMAKE_PREFIX_PATH "/Users/feldrise/Qt/5.11.1/clang_64/")
    set (ENV{OPENSSL_ROOT_DIR} "${CMAKE_SOURCE_DIR}/third-party/openssl/macos/")
    set (ENV{OPENSSL_CRYPTO_LIBRARY} "${CMAKE_SOURCE_DIR}/third-party/openssl/macos/")
endif()

if(GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# ndb
set(NDB_ENGINE_SQLITE ON)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/ndb ${CMAKE_SOURCE_DIR}/third-party/ndb/cmake-build)

set(CMAKE_CXX_STANDARD 17)
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
        Database/*.hpp
        Database/*.cpp
        Download/*.cpp
        Download/*.hpp
        History/*.cpp
        History/*.hpp
		MaquetteGrid/*.cpp
		MaquetteGrid/*.hpp
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
find_package(Qt5WebEngine 5.8 REQUIRED)
find_package(Qt5WebEngineWidgets 5.8 REQUIRED)

add_library(Core ${SOURCE_FILES} ../icons.qrc/ ${QM_FILES})

foreach(_source IN ITEMS ${SOURCE_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_SOURCE_DIR}/Core" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

set(SIELO_LIBS ${OPENSSL_LIBRARIES} Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)
target_link_libraries(Core LINK_PUBLIC ${SIELO_LIBS})
target_link_libraries(Core PUBLIC lib_ndb)