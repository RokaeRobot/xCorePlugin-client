QT += widgets

TEMPLATE = lib
DEFINES += XPLUGINDEMO_LIBRARY

#CONFIG += c++11
CONFIG += c++17
#MAKE_CXXFLAGS += /std:c++17
CONFIG += no_plugin_name_prefix  # 禁用自动添加的"lib"前缀

DESTDIR = ../

if (contains(DEFINES, WIN64)) {
    LIBS += -L$$PWD/../../lib/windows -lxplugin

} else {
    isEqual(QMAKE_CC, gcc) {
        LIBS += -L$$PWD/../../lib/amd64 -l:xplugin.so
    } else {
        LIBS += -L$$PWD/../../lib/aarch64 -l:xplugin.so
    }
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    CONFIG += plugin
}

#user defines
DEFINES += PLUGIN_NAME=\\\"xplugindemo\\\"
CONFIG(debug, debug|release) {
DEFINES += LOG_ENABLE_DETAILS
} else {
DEFINES += LOG_MIN_FILE_LEVEL=XPLUGIN_LOG_LEVEL_DEBUG
}

INCLUDEPATH += $$PWD/../../include

SOURCES += \
    ShowWidget.cpp \
    expandwidget.cpp \
    rl/rlcustominsertwidget.cpp \
    rl/rlcustompropretywidget.cpp \
    showstatusbar.cpp \
    xplugindemo.cpp

HEADERS += \
    ShowWidget.h \
    expandwidget.h \
    rl/rlcustominsertwidget.h \
    rl/rlcustompropretywidget.h \
    showstatusbar.h \
    xplugindemo_global.h \
    xplugindemo.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    ShowWidget.ui \
    expandwidget.ui \
    rl/rlcustominsertwidget.ui \
    rl/rlcustompropretywidget.ui \
    showstatusbar.ui

RESOURCES += \
    xplugindemo.qrc

TRANSLATIONS += \
    language/xplugindemo_cn.ts
