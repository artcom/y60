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
#include "signature.h"
#include "arguments.h"
#include "function.h"
#include "monkey_utilities.h"
#include "ape_thing.h"
#include "scope.h"

namespace y60 { namespace ape {

template <typename UserModule>
class binding {
    private:
        template <typename LocalUniqueId>
        struct unique_id : boost::mpl::vector2<LocalUniqueId, UserModule> {};

    protected:
        template <typename LocalUniqueId>
        detail::namespace_helper< typename unique_id<LocalUniqueId>::type >
        namespace_scope() {
            typedef typename unique_id<LocalUniqueId>::type id;
            return detail::namespace_helper<id>();
        }

};

template <typename Class>
detail::class_helper<Class>
class_(const char * name) {
    typedef boost::shared_ptr<detail::class_desc<Class> > cp;
    // XXX ugly
    cp cd( new detail::class_desc<Class>(name,
                    detail::current_scope->property_flags()));
    detail::current_scope->add( cd );
    detail::scope_lock class_scope_lock( new detail::scope(cd));
    return detail::class_helper<Class>( class_scope_lock );
}

void enumerate(bool flag) {
    detail::current_scope->enumerate(flag);
}

template <typename Binding>
class module : public detail::ape_thing {
    public:
        module(const char * name) : detail::ape_thing(detail::ape_module, name) {}
        void import(JSContext * cx, JSObject * ns) {
            import(cx, ns, ns_scope);
        }
        void import(JSContext * cx, JSObject * ns, detail::monkey_data & /*ape_ctx*/) {
            static bool imported = false;
            if ( imported ) {
                std::ostringstream os;
                os << "module '" << get_name() << "' is already imported.";
                throw ape_error(os.str(), PLUS_FILE_LINE);
            }
            {
                Binding b;
                b.bind();
            }
            import_children(cx, ns, ns_scope);
            if ( ! ns_scope.static_functions.empty()) {
                if ( ! JS_DefineFunctions(cx, ns, ns_scope.static_functions.ptr())) {
                    throw monkey_error("failed to define functions", PLUS_FILE_LINE);
                }
            }
            imported = true;
        }
    private:
        static detail::monkey_data ns_scope;
};

template <typename Binding>
detail::monkey_data module<Binding>::ns_scope;
// XXX namespace pollution
const char * const prop_modules = "modules";

template <typename ModuleBinding>
class module_loader : public asl::PlugInBase,
                      public jslib::IJSModuleLoader
{
    public:
        typedef module<ModuleBinding> module_type;
        typedef boost::shared_ptr<module_type> module_ptr_type;

        module_loader(const char * name, asl::DLHandle theDLHandle) :
            asl::PlugInBase( theDLHandle ), module_name_( name ) {}

        // implement IJSModuleLoader
        void initClasses(JSContext * cx, JSObject * global, JSObject * ns) {
            init_module(cx, global, ns);
        }
    private:
        void
        init_module(JSContext * cx, JSObject * global, JSObject * ns) {
            module_ptr_type mod(new module<ModuleBinding>(module_name_));
            detail::scope module_scope(mod);

            mod->import(cx, ns);
            // TODO: policy-fy
            announce_module_in_namespace(cx, ns, detail::current_scope->get_name());
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
        const char * module_name_;
};

}} // end of namespace ape, y60

#define Y60_APE_NS_SCOPE() \
        namespace_scope<y60::ape::detail::line_number<__LINE__> >()

#if defined(WIN32)
#   define Y60_APE_MODULE_DECL __declspec( dllexport )
#else
#   define Y60_APE_MODULE_DECL 
#endif

#define Y60_APE_BNAME( name ) \
    name ## _binding

#define Y60_APE_MODULE( name )                                                  \
class Y60_APE_BNAME(name) : public y60::ape::binding< Y60_APE_BNAME(name) > {   \
    public:                                                                     \
        friend class y60::ape::module< Y60_APE_BNAME(name) >;                   \
        void bind();                                                            \
    protected:                                                                  \
        Y60_APE_BNAME(name)() :                \
                y60::ape::binding< Y60_APE_BNAME(name) >() {}                \
};                                                                              \
                                                                                \
extern "C" Y60_APE_MODULE_DECL                                                  \
asl::PlugInBase * name ## _instantiatePlugIn(asl::DLHandle theDLHandle) {       \
    using y60::ape::module_loader;                                              \
    return new module_loader< Y60_APE_BNAME(name) >(#name, theDLHandle);        \
}                                                                               \
                                                                                \
void Y60_APE_BNAME( name ) ::bind()

#endif // Y60_APE_MODULE_INCLUDED
