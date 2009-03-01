#ifndef Y60_JAVASCRIPT_MODULE_LOADER_INCLUDED
#define Y60_JAVASCRIPT_MODULE_LOADER_INCLUDED

#include <js/spidermonkey/jscntxt.h>

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IJSModuleLoader.h>

namespace y60 { namespace ape { namespace detail {

template <typename Initializer>
class module_loader : public asl::PlugInBase,
                      public jslib::IJSModuleLoader
{
    public:
        module_loader(asl::DLHandle theDLHandle) : asl::PlugInBase( theDLHandle ) {}

        // implement IJSModuleLoader initClasses()
        void initClasses( JSContext * cx, JSObject * ns ) {
            Initializer mi(cx, ns);
            mi.init_module();
        }
};

}}} // end of namespace detail, ape, y60

#endif // Y60_JAVASCRIPT_MODULE_LOADER_INCLUDED
