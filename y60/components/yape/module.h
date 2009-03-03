#ifndef Y60_APE_MODULE_INIT_INCLUDED
#define Y60_APE_MODULE_INIT_INCLUDED

#include "y60_ape_settings.h"

#include <js/spidermonkey/jsapi.h>

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IJSModuleLoader.h>

#include "module_initializer.h"
#include "module_loader.h"

#if defined(WIN32)
#   define Y60_APE_MODULE_DECL __declspec( dllexport )
#else
#   define Y60_APE_MODULE_DECL 
#endif

#define Y60_APE_MODULE( name )                                              \
                                                                            \
struct name ## _module_initializer :                                        \
        public y60::ape::module_initializer                                 \
{                                                                           \
    name ## _module_initializer(JSContext * cx, JSObject * root) :          \
            y60::ape::module_initializer(cx,root) {}                        \
    void init_module();                                                     \
};                                                                          \
                                                                            \
extern "C" Y60_APE_MODULE_DECL                                              \
asl::PlugInBase * name ## _instantiatePlugIn(asl::DLHandle theDLHandle) {   \
    using y60::ape::module_loader;                                          \
    return new module_loader<name ## _module_initializer>(theDLHandle);     \
}                                                                           \
                                                                            \
void name ## _module_initializer::init_module()

#endif // Y60_APE_MODULE_INIT_INCLUDED
