#ifndef ASL_UNUSED_SETTING_INCLUDED
#define ASL_UNUSED_SETTING_INCLUDED

#include <asl/base/settings.h>

#ifdef aslunused_EXPORTS
#   define ASL_UNUSED_DECL AC_DLL_EXPORT
#else
#   define ASL_UNUSED_DECL AC_DLL_IMPORT
#endif

#endif // ASL_UNUSED_SETTING_INCLUDED
