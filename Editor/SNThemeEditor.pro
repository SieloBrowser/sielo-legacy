QT+=widgets
QT += webenginewidgets

SOURCES += \
    src/Widgets/ToolBar.cpp \
    src/Main.cpp \
    src/MainWindow.cpp \
    src/ThemeManager.cpp

HEADERS += \
    includes/Widgets/ToolBar.hpp \
    includes/MainWindow.hpp \
    includes/ThemeManager.hpp

RESOURCES += \
    ../Utils/ressources/data.qrc
