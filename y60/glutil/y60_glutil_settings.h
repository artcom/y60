#ifndef Y60_GLUTIL_SETTING_INCLUDED
#define Y60_GLUTIL_SETTING_INCLUDED

#include <asl/base/settings.h>

#ifdef y60glutil_EXPORTS
#   define Y60_GLUTIL_DECL AC_DLL_EXPORT
#else
#   define Y60_GLUTIL_DECL AC_DLL_IMPORT
#endif

#ifdef OSX
#   define AC_IGNORE_NV_MEMORY_MANAGEMENT
#   define AC_USE_OSX_CGL
#endif

#ifdef _WIN32
#    define NOMINMAX
#    ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN 1
#    endif
#    include <windows.h>
#    undef WIN32_LEAN_AND_MEAN
#endif

#if defined(LINUX)
#   define AC_USE_X11
#endif


#endif // Y60_GLUTIL_SETTING_INCLUDED
