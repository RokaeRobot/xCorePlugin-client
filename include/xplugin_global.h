#ifndef XPLUGIN_GLOBAL_H
#define XPLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPLUGIN_LIBRARY)
#  define XPLUGIN_EXPORT Q_DECL_EXPORT
#else
#  define XPLUGIN_EXPORT Q_DECL_IMPORT
#endif

#endif // XPLUGIN_GLOBAL_H
