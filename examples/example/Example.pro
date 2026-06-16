QT += widgets

TEMPLATE = lib
DEFINES += XPLUGINDEMO_LIBRARY

CONFIG += c++11

DESTDIR = ../

if (contains(DEFINES, WIN64)) {
    LIBS += -L$$PWD/../../lib/windows -lxplugin
} else {
    isEqual(QMAKE_CC, gcc) {
        LIBS += -L$$PWD/../../lib/amd64 -lxplugin
    } else {
        LIBS += -L$$PWD/../../lib/aarch64 -lxplugin
    }
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    CONFIG += plugin
}

#user defines
DEFINES += PLUGIN_NAME=\\\"Example\\\"
CONFIG(debug, debug|release) {
DEFINES += LOG_ENABLE_DETAILS
} else {
DEFINES += LOG_MIN_FILE_LEVEL=XPLUGIN_LOG_LEVEL_DEBUG
}

INCLUDEPATH += ../../include

SOURCES += \
    Example.cpp

HEADERS += \
    Example.h \
    Example_global.h
# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

