QT+=widgets

SOURCES += \
    src/Main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Core/ -lCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Core/ -lCored
else:unix: LIBS += -L$$OUT_PWD/../Core/ -lCore

INCLUDEPATH += $$PWD/../Core
DEPENDPATH += $$PWD/../Core
