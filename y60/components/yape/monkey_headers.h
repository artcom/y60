#ifndef Y60_APE_MONKEY_HEADERS_INCLUDED
#define Y60_APE_MONKEY_HEADERS_INCLUDED

#include "y60_ape_settings.h"

#ifdef Y60_APE_USE_TRACEMONKEY
#   include <js/jsapi.h>
#   include <js/jsapi.h>
#   include <js/jsscript.h>
#   include <js/jspubtd.h>
#   include <js/jsstddef.h>
#else
#   include <js/spidermonkey/jsapi.h>
#   include <js/spidermonkey/jsapi.h>
#   include <js/spidermonkey/jsscript.h>
#   include <js/spidermonkey/jspubtd.h>
#   include <js/spidermonkey/jsstddef.h>
#endif

#endif // Y60_APE_MONKEY_HEADERS_INCLUDED
