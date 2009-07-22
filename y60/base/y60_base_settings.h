#ifndef Y60_BASE_SETTING_INCLUDED
#define Y60_BASE_SETTING_INCLUDED

#include <asl/base/settings.h>

#ifdef y60base_EXPORTS
#   define Y60_BASE_DECL AC_DLL_EXPORT
#else
#   define Y60_BASE_DECL AC_DLL_IMPORT
#endif

#define Y60_BASE_UNIQUE_IDS
#undef  Y60_BASE_COUNTER_IDS

#endif // Y60_BASE_SETTING_INCLUDED
