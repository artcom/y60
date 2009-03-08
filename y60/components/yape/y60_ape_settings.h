#ifndef Y60_APE_SETTING_INCLUDED
#define Y60_APE_SETTING_INCLUDED

#include <asl/base/settings.h>

// Maximum number of function arguments
#ifndef Y60_APE_MAX_ARITY
#   define Y60_APE_MAX_ARITY 3
#endif

// XXX yape really is header-only
#ifdef yape_EXPORTS
#   define Y60_APE_EXPORT AC_DLL_EXPORT
#else
#   define Y60_APE_EXPORT AC_DLL_IMPORT
#endif

#endif // Y60_APE_SETTING_INCLUDED
