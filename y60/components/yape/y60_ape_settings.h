#ifndef Y60_APE_SETTING_INCLUDED
#define Y60_APE_SETTING_INCLUDED

#include <y60/components/yape/y60_ape_config.h>

#include <asl/base/settings.h>

// Maximum number of function arguments
#ifndef Y60_APE_MAX_ARITY
#   define Y60_APE_MAX_ARITY 10
#endif

//#define Y60_APE_USE_TRACEMONKEY

// See https://bugzilla.mozilla.org/show_bug.cgi?id=438633
//#define Y60_APE_WORKAROUND_SPIDERMONKEY_BUG_438633

#endif // Y60_APE_SETTING_INCLUDED
