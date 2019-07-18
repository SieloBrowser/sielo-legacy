add_definitions(-DSIELO_SHAREDLIBRARY)
cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_SOURCE_DIR}/Core)
include_directories(${CMAKE_SOURCE_DIR}/WebEngines)
include_directories(${CMAKE_SOURCE_DIR}/third-party/includes)

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
	    SharedDefines.hpp
)

file(
        GLOB_RECURSE
        QRC_FILES
        ${CMAKE_SOURCE_DIR}/*.qrc
)

add_library(SieloCore SHARED ${SOURCE_FILES} ${QRC_FILES} ${QM_FILES})

set(SCORE_LIBS SieloWebEngine ${OPENSSL_LIBRARIES} Qt5::Widgets Qt5::Network Qt5::Sql Qt5::WebChannel)
if(WIN32)
    set(SCORE_LIBS ${SCORE_LIBS} dwmapi uxtheme)
endif()

target_link_libraries(SieloCore PUBLIC ${SCORE_LIBS})

if (WIN32)
    # deploy
    get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
    set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

    add_custom_command(TARGET SieloCore POST_BUILD
        COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR} $<TARGET_FILE_DIR:SieloCore>/SieloCore.dll)
		file(GLOB OPENSSL_CRYPTO_DLLS "${OPENSSL_DIR}/bin/*.dll")
        file(COPY ${OPENSSL_CRYPTO_DLLS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
        file(COPY ${OPENSSL_CRYPTO_DLLS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
endif()

foreach(_source IN ITEMS ${SOURCE_FILES})
	get_filename_component(_source_path "${_source}" PATH)
	file(RELATIVE_PATH _source_path_rel "${CMAKE_SOURCE_DIR}/Core" "${_source_path}")
	string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	source_group("${_group_path}" FILES "${_source}")
endforeach()

