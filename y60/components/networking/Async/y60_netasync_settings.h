#ifndef Y60_NETASYNC_SETTING_INCLUDED
#define Y60_NETASYNC_SETTING_INCLUDED

#include <asl/base/settings.h>

#ifdef _WINSOCKAPI_
#   undef _WINSOCKAPI_
#endif

#include <boost/version.hpp>

#ifdef LINUX
#   if BOOST_VERSION  >= 104800 and BOOST_VERSION < 104900
#       define BOOST_ASIO_DISABLE_EPOLL
#   endif
#endif

#ifdef NetAsync_EXPORTS
#   define Y60_NETASYNC_DECL AC_DLL_EXPORT
#else
#   define Y60_NETASYNC_DECL AC_DLL_IMPORT
#endif

#endif // Y60_NETASYNC_SETTING_INCLUDED
