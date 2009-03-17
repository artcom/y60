#ifndef ASL_IPC_SETTING_INCLUDED
#define ASL_IPC_SETTING_INCLUDED

#include <asl/base/settings.h>

#ifdef aslipc_EXPORTS
#   define ASL_IPC_DECL AC_DLL_EXPORT
#else
#   define ASL_IPC_DECL AC_DLL_IMPORT
#endif

#endif // ASL_IPC_SETTING_INCLUDED
