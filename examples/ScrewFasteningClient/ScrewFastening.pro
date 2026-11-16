# 插件基本配置
QT += widgets
CONFIG += c++11 plugin no_plugin_name_prefix
TEMPLATE = lib

DEFINES += PLUGIN_NAME=\\\"ScrewFastening\\\"
DEFINES += LOG_MIN_FILE_LEVEL=XPLUGIN_LOG_LEVEL_DEBUG

# 客户端框架依赖
XPLUGIN_SDK_ROOT = $$clean_path($$PWD/../..)
INCLUDEPATH += "$$XPLUGIN_SDK_ROOT/include"

win32 {
    LIBS += "-L$$XPLUGIN_SDK_ROOT/lib/windows" -lxplugin
}

unix:!macx {
    LIBS += "-L$$XPLUGIN_SDK_ROOT/lib/aarch64" -l:xplugin.so
    # 保留 $ORIGIN，供加载器在运行时从插件目录查找依赖库。
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    QMAKE_LIBS_OPENGL_ES2 =
    QMAKE_LIBDIR_OPENGL_ES2 =
}

# 客户端插件源码
# 显式指定 MSVC 源文件编码，避免无 BOM 的 UTF-8 源码被按本地代码页读取。
win32-msvc*: QMAKE_CXXFLAGS += /utf-8

INCLUDEPATH += $$PWD/src
HEADERS += $$files($$PWD/src/*.h, true)
SOURCES += $$files($$PWD/src/*.cpp, true)

# Qt 资源与分发文件
RESOURCES += ScrewFastening.qrc
DISTFILES += ScrewFastening.json
