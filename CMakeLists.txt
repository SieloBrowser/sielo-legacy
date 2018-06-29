cmake_minimum_required(VERSION 3.6)
project(sielo-browser)

add_subdirectory(Core)
add_subdirectory(SNCompiler)
include_directories(${CMAKE_SOURCE_DIR})
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
    set(ICON_NAME "icon.icns")
    set(ICON_PATH "${CMAKE_SOURCE_DIR}/${ICON_NAME}")
    set (CMAKE_PREFIX_PATH "/Users/feldrise/Qt/5.11.1/clang_64/")
    set (ENV{OPENSSL_ROOT_DIR} "${CMAKE_SOURCE_DIR}/third-party/openssl/macos/")
    set (ENV{OPENSSL_CRYPTO_LIBRARY} "${CMAKE_SOURCE_DIR}/third-party/openssl/macos/")
endif()

if(GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

set(SOURCE_FILES Main.cpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(
        GLOB_RECURSE
        QRC_FILES
		*.qrc
)

find_package(OpenSSL 1.1.0 REQUIRED)
find_package(Qt5Widgets 5.8 REQUIRED)
find_package(Qt5WebEngine 5.8 REQUIRED)
find_package(Qt5WebEngineWidgets 5.8 REQUIRED)

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

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_custom_command(TARGET sielo-browser POST_BUILD
            COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR} $<TARGET_FILE_DIR:sielo-browser>)
			file(GLOB OPENSSL_CRYPTO_DLLS "${CMAKE_SOURCE_DIR}/third-party/openssl/windows/x64/bin/*.dll")
			file(COPY ${OPENSSL_CRYPTO_DLLS} DESTINATION Release)
elseif(WIN32)
	add_custom_command(TARGET sielo-browser POST_BUILD
            COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR} $<TARGET_FILE_DIR:sielo-browser>)
			file(GLOB OPENSSL_CRYPTO_DLLS "${CMAKE_SOURCE_DIR}/third-party/openssl/windows/x86/bin/*.dll")
			file(COPY ${OPENSSL_CRYPTO_DLLS} DESTINATION Release)
endif()

set(SIELO_LIBS Core ${OPENSSL_LIBRARIES} Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)
target_link_libraries(sielo-browser LINK_PUBLIC ${SIELO_LIBS})