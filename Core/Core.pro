TEMPLATE = lib

QT += widgets
QT += webenginewidgets

HEADERS += \
    Web/LoadRequest.hpp \
    Application.hpp \
    Web/WebPage.hpp \
    Web/WebHitTestResult.hpp \
    Utils/CheckBoxDialog.hpp

SOURCES += \
    Application.cpp \
    Web/WebHitTestResult.cpp \
    Utils/CheckBoxDialog.cpp
