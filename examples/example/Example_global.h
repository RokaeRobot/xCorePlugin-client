#ifndef EXAMPLE_GLOBAL_H
#define EXAMPLE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(EXAMPLE_LIBRARY)
#  define EXAMPLE_EXPORT Q_DECL_EXPORT
#else
#  define EXAMPLE_EXPORT Q_DECL_IMPORT
#endif

#endif // EXAMPLE_GLOBAL_H
