#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(IWKATS_CORE_LIB)
#  define IWKATS_CORE_EXPORT Q_DECL_EXPORT
# else
#  define IWKATS_CORE_EXPORT Q_DECL_IMPORT
# endif
#else
# define IWKATS_CORE_EXPORT
#endif


#ifndef IWKATS_NAMESPACE_BEGIN
#define IWKATS_NAMESPACE_BEGIN namespace iwkats{
#endif // IWKATS_NAMESPACE_BEGIN

#ifndef IWKATS_NAMESPACE_END
#define IWKATS_NAMESPACE_END } /*namespace iwkats*/
#endif // IWKATS_NAMESPACE_END
