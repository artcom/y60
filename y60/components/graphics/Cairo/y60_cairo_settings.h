#ifndef Y60_CAIRO_SETTING_INCLUDED
#define Y60_CAIRO_SETTING_INCLUDED

#include <asl/base/settings.h>

#ifdef Cairo_EXPORTS
#   define Y60_CAIRO_DECL AC_DLL_EXPORT
#else
#   define Y60_CAIRO_DECL AC_DLL_IMPORT
#endif

#endif // Y60_CAIRO_SETTING_INCLUDED
