#ifndef I_JS_MODULE_LOADER_INCLUDED
#define I_JS_MODULE_LOADER_INCLUDED

// only needed while the real module loader resides in components/jswrapper_test/*

#ifdef USE_LEGACY_SPIDERMONKEY
#include <js/spidermonkey/jsapi.h>
#else
#include <js/jsapi.h>
#endif

namespace jslib {

class IJSModuleLoader {
    public:
        virtual void initClasses( JSContext * cx, JSObject * ns) = 0;
        virtual ~IJSModuleLoader() {}
};

typedef asl::Ptr<IJSModuleLoader> IJSModuleLoaderPtr;


}

#endif // I_JS_MODULE_LOADER_INCLUDED
