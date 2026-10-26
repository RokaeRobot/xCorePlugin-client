QT += widgets

TEMPLATE = lib
DEFINES += XPLUGINDEMO_LIBRARY

# ---------------------------------------------------------------------------
# Rokae SDK 附属能力: 默认关闭
# 需要使用SDK示例时, 取消下一行注释后 qmake/编译
# DEFINES += HAVE_ROKAE_SDK
# ---------------------------------------------------------------------------

CONFIG += c++11
CONFIG += no_plugin_name_prefix

DESTDIR = ../

# ---- xplugin 运行时库 (主依赖) ----
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

# ---- Rokae SDK (仅 HAVE_ROKAE_SDK 时链接) ----
contains(DEFINES, HAVE_ROKAE_SDK) {
    # SDK 头文件需要 C++17
    CONFIG -= c++11
    CONFIG += c++17

    if (contains(DEFINES, WIN64)) {
        LIBS += -L$$PWD/3rdparty/rokaesdk/lib/Windows/Release/64bit/
        LIBS += -lxCoreSDK_static -lxMateModel
        PRE_TARGETDEPS += $$PWD/3rdparty/rokaesdk/lib/Windows/Release/64bit/xCoreSDK_static.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/rokaesdk/lib/Windows/Release/64bit/xMateModel.lib
        INCLUDEPATH += $$PWD/3rdparty/rokaesdk/include/rokae
        DEPENDPATH += $$PWD/3rdparty/rokaesdk/include/rokae
        INCLUDEPATH += $$PWD/3rdparty/rokaesdk/external
    } else {
        isEqual(QMAKE_CC, gcc) {
            LIBS += -L$$PWD/3rdparty/rokaesdk/lib/Linux/x86_64/
            LIBS += -lxCoreSDK -lxMateModel
            PRE_TARGETDEPS += $$PWD/3rdparty/rokaesdk/lib/Linux/x86_64/libxCoreSDK.a
            PRE_TARGETDEPS += $$PWD/3rdparty/rokaesdk/lib/Linux/x86_64/libxMateModel.a
            INCLUDEPATH += $$PWD/3rdparty/rokaesdk/include/rokae
            DEPENDPATH += $$PWD/3rdparty/rokaesdk/include/rokae
            INCLUDEPATH += $$PWD/3rdparty/rokaesdk/external
        } else {
            LIBS += -L$$PWD/3rdparty/rokaesdk/lib/Linux/aarch64/
            LIBS += -lxCoreSDK
            PRE_TARGETDEPS += $$PWD/3rdparty/rokaesdk/lib/Linux/aarch64/libxCoreSDK.a
            INCLUDEPATH += $$PWD/3rdparty/rokaesdk/include/rokae
            DEPENDPATH += $$PWD/3rdparty/rokaesdk/include/rokae
            INCLUDEPATH += $$PWD/3rdparty/rokaesdk/external
        }
    }

    SOURCES += sdk/demosdkbridge.cpp
    HEADERS += sdk/demosdkbridge.h
}

# user defines
DEFINES += PLUGIN_NAME=\\\"xplugindemo\\\"
CONFIG(debug, debug|release) {
    DEFINES += LOG_ENABLE_DETAILS
} else {
    DEFINES += LOG_MIN_FILE_LEVEL=XPLUGIN_LOG_LEVEL_DEBUG
}

INCLUDEPATH += $$PWD/../../include
INCLUDEPATH += $$PWD

SOURCES += \
    expandwidget.cpp \
    pages/common/apiresultpanel.cpp \
    pages/demoshell.cpp \
    pages/xplugin/pagehmishell.cpp \
    pages/xplugin/pageioregister.cpp \
    pages/xplugin/pagenetworkeventlog.cpp \
    pages/xplugin/pageprojectpointvar.cpp \
    pages/xplugin/pagerobotstatus.cpp \
    pages/xplugin/pagexservicerc.cpp \
    rl/rlcustominsertwidget.cpp \
    rl/rlcustompropretywidget.cpp \
    showstatusbar.cpp \
    xplugindemo.cpp

HEADERS += \
    expandwidget.h \
    pages/common/apiresultpanel.h \
    pages/common/demoformat.h \
    pages/common/demopagelayout.h \
    pages/demoshell.h \
    pages/xplugin/pagehmishell.h \
    pages/xplugin/pageioregister.h \
    pages/xplugin/pagenetworkeventlog.h \
    pages/xplugin/pageprojectpointvar.h \
    pages/xplugin/pagerobotstatus.h \
    pages/xplugin/pagexservicerc.h \
    rl/rlcustominsertwidget.h \
    rl/rlcustompropretywidget.h \
    showstatusbar.h \
    xplugindemo_global.h \
    xplugindemo.h

unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    rl/rlcustominsertwidget.ui \
    rl/rlcustompropretywidget.ui \
    showstatusbar.ui

RESOURCES += \
    xplugindemo.qrc

TRANSLATIONS += \
    language/xplugindemo_cn.ts
