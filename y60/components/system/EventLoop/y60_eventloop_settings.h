#ifndef Y60_EVENTLOOP_SETTING_INCLUDED
#define Y60_EVENTLOOP_SETTING_INCLUDED

#include <asl/base/settings.h>

#ifdef EventLoop_EXPORTS
#   define Y60_EVENTLOOP_DECL AC_DLL_EXPORT
#else
#   define Y60_EVENTLOOP_DECL AC_DLL_IMPORT
#endif

#endif // Y60_EVENTLOOP_SETTING_INCLUDED