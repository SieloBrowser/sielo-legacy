cmake_minimum_required(VERSION 3.6)
project(sielo-browser)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(SNCompiler)

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (OPENSSL_DIR "${CMAKE_SOURCE_DIR}\\third-party\\openssl\\windows\\x64\\")
elseif(WIN32)
    set (OPENSSL_DIR "${CMAKE_SOURCE_DIR}\\third-party\\openssl\\windows\\x86\\")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (OPENSSL_DIR "${CMAKE_SOURCE_DIR}/third-party/openssl/macos/")
endif()

# user config config file
include("${CMAKE_SOURCE_DIR}\\user-config.cmake")

set (ENV{OPENSSL_ROOT_DIR} ${OPENSSL_DIR})
set (ENV{OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_DIR})

find_package(OpenSSL 1.1.0 REQUIRED)
find_package(Qt5 5.11.2 REQUIRED COMPONENTS Core Widgets WebEngine WebEngineWidgets Sql Network)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(ICON_NAME "icon.icns")
    set(ICON_PATH "${CMAKE_SOURCE_DIR}/${ICON_NAME}")
endif()

include_directories(${CMAKE_SOURCE_DIR}/Core)
include_directories(${CMAKE_SOURCE_DIR}/WebEngines)
include_directories(${CMAKE_SOURCE_DIR}/third-party/includes)

add_subdirectory(WebEngines/QWebEngine)
add_subdirectory(Core)

set(SOURCE_FILES Main.cpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (WIN32)
	set (SIELO_LIBS d3d9)
    add_executable(sielo-browser WIN32 ${SOURCE_FILES} ic_sielo.rc)
elseif(APPLE)
    add_executable(sielo-browser MACOSX_BUNDLE ${SOURCE_FILES} ${ICON_PATH})
    set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(sielo-browser PROPERTIES MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})
else()
    add_executable(sielo-browser ${SOURCE_FILES})
endif()

set (SIELO_LIBS ${SIELO_LIBS} SieloCore SieloWebEngine)

target_link_libraries(sielo-browser ${SIELO_LIBS})

