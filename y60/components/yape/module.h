#ifndef Y60_APE_MODULE_INCLUDED
#define Y60_APE_MODULE_INCLUDED

#include "y60_ape_settings.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include <asl/base/PlugInBase.h>
#include <asl/base/begin_end.h>
#include <y60/jsbase/IJSModuleLoader.h>

#include "exception.h"
#include "class.h"
#include "invoke.h"
#include "signature.h"
#include "arguments.h"
#include "function.h"
#include "monkey_utilities.h"

namespace y60 { namespace ape {

template <typename UserModule>
class module {
    private:
        template <typename LocalUniqueId>
        struct unique_id : boost::mpl::vector2<LocalUniqueId, UserModule> {};

    protected:
        module(const char * name) : name_(name) {}

        template <typename LocalUniqueId>
        detail::function_helper< typename unique_id<LocalUniqueId>::type >
        functions() {
            typedef typename unique_id<LocalUniqueId>::type id;
            return detail::function_helper<id>( module_description_ );
        }

        template <typename Class>
        detail::class_helper< typename unique_id<Class>::type >
        class_(const char * name) {
            typedef typename unique_id<Class>::type id;
            typedef boost::shared_ptr<detail::class_desc<Class> > cp;
            cp c( new detail::class_desc<Class>(name) );
            module_description_.push_back( c );
            return detail::class_helper<id>( * c );
        }

        void
        import(JSContext * cx, JSObject * ns) {
            static_cast<UserModule&>( *this ).init();
            for(size_t i = 0; i < module_description_.size(); ++i) {
                module_description_[i]->import( cx, ns, free_functions_ );
            }
            register_free_functions(cx, ns);
        }

        void
        register_free_functions(JSContext * cx, JSObject * ns)  {
            JS_DefineFunctions(cx, ns, free_functions_.ptr());
        }

        const char * get_name() { return name_; }

        template <typename Class> friend class class_wrapper;
    private:
        const char *         name_;
        detail::js_functions free_functions_;
        detail::ape_list   module_description_;
};


// XXX namespace pollution
const char * const prop_modules = "modules";

template <typename Module>
class module_loader : public asl::PlugInBase,
                      public jslib::IJSModuleLoader
{
        typedef boost::shared_ptr<Module> module_ptr_type;
    public:
        module_loader(asl::DLHandle theDLHandle) : asl::PlugInBase( theDLHandle ) {}

        // implement IJSModuleLoader initClasses()
        void initClasses(JSContext * cx, JSObject * global, JSObject * ns) {
            static module_ptr_type module = init_module(cx, global, ns);
        }
    private:
        module_ptr_type init_module(JSContext * cx, JSObject * global, JSObject * ns) {
            module_ptr_type module( new Module() );
            module->import(cx, ns);
            // TODO: policy-fy
            announce_module_in_namespace(cx, ns, module->get_name());
            return module;
        }

        void announce_module_in_namespace(JSContext * cx, JSObject * obj,
                std::string const& name)
        {
            jsval mod_list_val;
            JS_GetProperty(cx, obj, prop_modules, & mod_list_val );
            JSObject * mod_list = 0;
            if ( mod_list_val == JSVAL_VOID) {
                mod_list = JS_NewArrayObject(cx, 0, 0);
                mod_list_val = OBJECT_TO_JSVAL( mod_list );
                JS_SetProperty(cx, obj, prop_modules, & mod_list_val);
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

// some workaround macros for current deficiencies of the implementation

#define FUNCTIONS() \
        functions<y60::ape::detail::line_number_tag<__LINE__> >()

#define FUNCTION_ALIAS( f, name )                            \
        function( f, #name, ape_tag< __LINE__ >() )

#define FUNCTION( f ) FUNCTION_ALIAS( f, f )


#if defined(WIN32)
#   define Y60_APE_MODULE_DECL __declspec( dllexport )
#else
#   define Y60_APE_MODULE_DECL 
#endif

#define Y60_APE_MODULE( name )                                                  \
template <int Counter> struct ape_tag {};                                       \
                                                                                \
class name ## _module : public y60::ape::module<name ## _module> {              \
    public:                                                                     \
        friend class y60::ape::module_loader< name ## _module >;                \
        void init();                                                            \
    protected:                                                                  \
        name ## _module() : y60::ape::module<name ## _module>(#name) {}         \
};                                                                              \
                                                                                \
extern "C" Y60_APE_MODULE_DECL                                                  \
asl::PlugInBase * name ## _instantiatePlugIn(asl::DLHandle theDLHandle) {       \
    using y60::ape::module_loader;                                              \
    return new module_loader<name ## _module>(theDLHandle);                     \
}                                                                               \
                                                                                \
void name ## _module::init()

#endif // Y60_APE_MODULE_INCLUDED
