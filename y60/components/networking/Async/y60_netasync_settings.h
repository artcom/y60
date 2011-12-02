#ifndef Y60_NETASYNC_SETTING_INCLUDED
#define Y60_NETASYNC_SETTING_INCLUDED

#include <asl/base/settings.h>

#ifdef _WINSOCKAPI_
#	undef _WINSOCKAPI_
#endif

#ifdef HttpServer
#   define Y60_NETASYNC_DECL AC_DLL_EXPORT
#else
#   define Y60_NETASYNC_DECL AC_DLL_IMPORT
#endif

#endif // Y60_NETASYNC_SETTING_INCLUDED
