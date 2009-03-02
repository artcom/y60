#ifndef Y60_APE_MODULE_INIT_INCLUDED
#define Y60_APE_MODULE_INIT_INCLUDED


#include <js/spidermonkey/jsapi.h>

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IJSModuleLoader.h>

#include "module_initializer.h"

namespace y60 { namespace ape {

template <typename Initializer>
class module_loader : public asl::PlugInBase,
                      public jslib::IJSModuleLoader
{
    public:
        module_loader(asl::DLHandle theDLHandle) : asl::PlugInBase( theDLHandle ) {}

        // implement IJSModuleLoader initClasses()
        void initClasses( JSContext * cx, JSObject * global, JSObject * ns,
                const std::string & module_name )
        {
            Initializer mi(cx, ns);
            mi.init_module();
        }
    private:
        void register_module( JSContext * cx, JSObject * global, JSObject * ns) {
            JSObject * array = JS_NewArrayObject(cx, 0, 0);
            JSString * str = JS_NewStringCopyZ(cx, "foo");
            jsval name_val = STRING_TO_JSVAL(str);
            JS_SetElement(cx, array, 0, & name_val);
            jsval array_val = OBJECT_TO_JSVAL(array);
            JS_SetProperty(cx, global, "all_modules", & array_val );
            JS_SetProperty(cx, ns, "all_modules", & array_val );


        }
};

}} // end of namespace ape, y60

#define FPTR(func) typeof(&func), & func

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
