#ifndef ASL_ZIP_SETTING_INCLUDED
#define ASL_ZIP_SETTING_INCLUDED

#include <asl/base/settings.h>

#ifdef aslzip_EXPORTS
#   define ASL_ZIP_DECL AC_DLL_EXPORT
#else
#   define ASL_ZIP_DECL AC_DLL_IMPORT
#endif

#endif // ASL_ZIP_SETTING_INCLUDED
