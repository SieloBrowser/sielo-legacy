cmake_minimum_required(VERSION 3.6)
project(Core)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES
        Plugins/PluginInterface.hpp
        Plugins/Plugins.cpp
        Plugins/Plugins.hpp
        Utils/CheckBoxDialog.cpp
        Utils/CheckBoxDialog.hpp
        Web/LoadRequest.hpp
        Web/WebHitTestResult.cpp
        Web/WebHitTestResult.hpp
        Web/WebPage.hpp
        Application.cpp
        Application.hpp Plugins/PluginProxy.hpp Plugins/PluginProxy.cpp Web/WebPage.cpp Web/WebView.hpp Web/WebView.cpp Utils/DelayedFileWatcher.hpp Utils/DelayedFileWatcher.cpp Utils/SiteInformation.hpp Utils/CertificateInformation.hpp Utils/CertificateInformation.cpp Utils/EllipseLabel.hpp Utils/EllipseLabel.cpp Utils/HorizontalListWidget.hpp Utils/HorizontalListWidget.cpp Utils/SiteInformation.cpp Web/Scripts.hpp Utils/AutoSaver.hpp Utils/AutoSaver.cpp)

include_directories("./")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)

add_library(Core ${SOURCE_FILES})
target_link_libraries(Core Qt5::Widgets)
target_link_libraries(Core Qt5::WebEngine)
target_link_libraries(Core Qt5::WebEngineWidgets)
