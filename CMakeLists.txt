cmake_minimum_required(VERSION 3.6)
project(sielo-browser)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

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
include_directories(${CMAKE_SOURCE_DIR}/third-party/includes)

add_subdirectory(Core)
add_subdirectory(Plugins)

set(SOURCE_FILES Main.cpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

file(
        GLOB_RECURSE
        QRC_FILES
        *.qrc
)

if (WIN32)
    add_executable(sielo-browser WIN32 ${SOURCE_FILES} ${QRC_FILES} ic_sielo.rc)

    # deploy
    get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
    set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

elseif(APPLE)
    add_executable(sielo-browser MACOSX_BUNDLE ${SOURCE_FILES} ${ICON_PATH} ${QRC_FILES})
    set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(sielo-browser PROPERTIES MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})
else()
    add_executable(sielo-browser ${SOURCE_FILES} ${QRC_FILES})
endif()

if(WIN32)
    if (CMAKE_BUILD_TYPE MATCHES DEBUG)
		add_custom_command(TARGET sielo-browser POST_BUILD
            COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR} $<TARGET_FILE_DIR:sielo-browser>)
			file(GLOB OPENSSL_CRYPTO_DLLS "${OPENSSL_DIR}/bin/*.dll")
			file(COPY ${OPENSSL_CRYPTO_DLLS} DESTINATION Debug)
	else()
		add_custom_command(TARGET sielo-browser POST_BUILD
            COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR} $<TARGET_FILE_DIR:sielo-browser>)
			file(GLOB OPENSSL_CRYPTO_DLLS "${OPENSSL_DIR}/bin/*.dll")
            file(COPY ${OPENSSL_CRYPTO_DLLS} DESTINATION Release)
    endif()
endif()

target_link_libraries(sielo-browser Core)

