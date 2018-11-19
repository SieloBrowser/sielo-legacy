add_definitions(-DSIELO_SHAREDLIBRARY)
cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR}/Core)
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

add_library(Core SHARED ${SOURCE_FILES} ../icons.qrc/ ${QM_FILES})

set(SIELO_LIBS ${OPENSSL_LIBRARIES} Qt5::Widgets Qt5::WebEngine Qt5::Sql Qt5::WebEngineWidgets)
if(WIN32)
    set(SIELO_LIBS ${SIELO_LIBS} dwmapi uxtheme)
endif()

target_link_libraries(Core ${SIELO_LIBS})

foreach(_source IN ITEMS ${SOURCE_FILES})
	get_filename_component(_source_path "${_source}" PATH)
	file(RELATIVE_PATH _source_path_rel "${CMAKE_SOURCE_DIR}/Core" "${_source_path}")
	string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	source_group("${_group_path}" FILES "${_source}")
endforeach()

