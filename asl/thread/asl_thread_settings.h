#ifndef ASL_THREAD_SETTINGS_INCLUDED
#define ASL_THREAD_SETTINGS_INCLUDED

#include <asl/base/settings.h>

#ifdef aslthread_EXPORTS
#   define ASL_THREAD_DECL AC_DLL_EXPORT
#else
#   define ASL_THREAD_DECL AC_DLL_IMPORT
#endif

#endif // ASL_THREAD_SETTINGS_INCLUDED
