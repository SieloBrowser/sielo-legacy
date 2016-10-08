QT += widgets
QT += network
QT += webenginewidgets
QT += xml

SOURCES += \
    src/Main.cpp \
    src/SMainWindow.cpp \
    src/SWidgets/SMenu.cpp \
    src/SWidgets/SSearchArea.cpp \
    src/SWidgets/STabWidget.cpp \
    src/SWidgets/SToolBar.cpp \
    src/SWidgets/SUrlArea.cpp \
    src/SWidgets/SWebView.cpp \
    src/SWindows/SHistory.cpp \
    src/SWindows/SHtmlSrcViewver.cpp \
    src/SWindows/SDownload.cpp \
    src/SWindows/SBookmarks.cpp

HEADERS += \
    includes/SMainWindow.hpp \
    includes/SWidgets/SMenu.hpp \
    includes/SWidgets/SSearchArea.hpp \
    includes/SWidgets/STabWidget.hpp \
    includes/SWidgets/SToolBar.hpp \
    includes/SWidgets/SUrlArea.hpp \
    includes/SWidgets/SWebView.hpp \
    includes/SWindows/SHistory.hpp \
    includes/SWindows/SHtmlSrcViewver.hpp \
    includes/SWindows/SDownload.hpp \
    includes/SActions.hpp \
    includes/SWindows/SBookmarks.hpp

