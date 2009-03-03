#ifndef Y60_APE_MODULE_LOADER_INCLUDED
#define Y60_APE_MODULE_LOADER_INCLUDED

#include "y60_ape_settings.h"

namespace y60 { namespace ape {

namespace properties {
    const char * const MODULES = "modules";
}

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
            register_module(cx, ns, module_name);
        }
    private:

        void register_module(JSContext * cx, JSObject * obj,
                std::string const& name)
        {
            jsval mod_list_val;
            JS_GetProperty(cx, obj, properties::MODULES, & mod_list_val );
            JSObject * mod_list = 0;
            if ( mod_list_val == JSVAL_VOID) {
                mod_list = JS_NewArrayObject(cx, 0, 0);
                mod_list_val = OBJECT_TO_JSVAL( mod_list );
                JS_SetProperty(cx, obj, properties::MODULES, & mod_list_val);
            } else if ( JSVAL_IS_OBJECT( mod_list_val )) {
                mod_list = JSVAL_TO_OBJECT( mod_list_val );
            }

            JSString * str = JS_NewStringCopyZ(cx, name.c_str());
            jsval name_val = STRING_TO_JSVAL(str);

            jsval module_val;
            JS_GetProperty(cx, mod_list, name.c_str(), & module_val );
            if ( module_val == JSVAL_VOID ) {
                JS_SetProperty(cx, mod_list, name.c_str(), & name_val );

                if ( JS_IsArrayObject(cx, mod_list )) {
                    jsuint len = 0;
                    JS_GetArrayLength( cx, mod_list, & len);
                    JS_SetArrayLength(cx, mod_list, len + 1);
                    JS_SetElement(cx, mod_list, len, & name_val);
                }
            }
        }
};

}} // end of namespace ape, y60

#endif // Y60_APE_MODULE_LOADER_INCLUDED
