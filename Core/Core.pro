TEMPLATE = lib

QT += widgets
QT += webenginewidgets

HEADERS += \
    Web/LoadRequest.hpp \
    Application.hpp \
    Web/WebPage.hpp \
    Utils/CheckBoxDialog.hpp \
    Plugins/PluginInterface.hpp \
    Web/WebHitTestResult.hpp \
    Plugins/Plugins.hpp

SOURCES += \
    Application.cpp \
    Utils/CheckBoxDialog.cpp \
    Web/WebHitTestResult.cpp \
    Plugins/Plugins.cpp
