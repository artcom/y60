#ifndef Y60_APE_MODULE_INCLUDED
#define Y60_APE_MODULE_INCLUDED

#include "y60_ape_settings.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>

#include <js/spidermonkey/jsapi.h>

#include <asl/base/PlugInBase.h>
#include <asl/base/begin_end.h>
#include <y60/jsbase/IJSModuleLoader.h>

#include "exception.h"
#include "class.h"
#include "invoke.h"
#include "signature.h"
#include "arguments.h"

namespace y60 { namespace ape {

namespace detail {

inline
JSFunctionSpec
init_js_function_spec_terminator() {
    JSFunctionSpec terminator;
    terminator.name  = 0;
    terminator.call  = 0;
    terminator.nargs = 0;
    terminator.flags = 0;
    terminator.extra = 0;
    return terminator;
}

inline
JSFunctionSpec const&
js_function_spec_terminator() {
    static JSFunctionSpec terminator = init_js_function_spec_terminator();
    return terminator;
}

}

template <typename Derived> // XXX think about a better name
class module {
    protected:
        module(const char * name) : name_(name) {}

        void
        import(JSContext * cx, JSObject * global, JSObject * ns) {
            if ( ! functions_.empty() ) {
                std::cout << "importing " << num_functions() << " function(s)" << std::endl;
                if ( ! JS_DefineFunctions(cx, ns, asl::begin_ptr(functions_))) {
                    std::ostringstream os;
                    os << "Failed to init functions for module " << name_;
                    throw internal_monkey_error( os.str(), PLUS_FILE_LINE);
                }
            }
        }

        size_t
        num_functions() const {
            return functions_.empty() ? 0 : functions_.size() - 1;
        }

        void
        init_module() {
            static_cast<Derived&>( *this ).init();
            if ( ! functions_.empty() ) {
                functions_.push_back( detail::js_function_spec_terminator() );
            }
        }

        template <typename F, typename UniqueId>
        void
        function( F * f, const char * name, UniqueId const& id) {
            JSFunctionSpec js_func;
            js_func.name  = name;
            js_func.call  = get_invoker(f, detail::get_signature( f ), id);
            js_func.nargs = get_arity( detail::get_signature( f )) ;
            js_func.flags = 0;
            js_func.extra = 0; // number of arg slots for local GC roots

            functions_.push_back( js_func );
        }

        template <typename Class>
        class_wrapper<Class>
        class_(const char * name) { return class_wrapper<Class>(name); }

        std::string const & get_name() { return name_; }

        template <typename Class> friend class class_wrapper;
    private:

        template <typename F, typename Signature, typename UniqueId>
        inline
        JSNative
        get_invoker(F * f, Signature const& sig, UniqueId const& id) {
            typedef detail::invoker<F,Signature,UniqueId> invoker_type;
            invoker_type::init( f );
            return & invoker_type::invoke;
        }

        template <typename Signature>
        inline
        uintN
        get_arity(Signature const& sig) {
            return detail::arity<Signature>::value;
        }

        const std::string name_;

        std::vector<JSFunctionSpec> functions_;
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
            static module_ptr_type module = init_module();
            module->import(cx, global, ns);
            // TODO: policy-fy
            announce_module_in_namespace(cx, ns, module->get_name());
        }
    private:
        module_ptr_type init_module() {
            module_ptr_type module( new Module() );
            module->init_module();
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
#define FUNCTION_ALIAS( f, name )                            \
        function( f, #name, ape_tag<__LINE__>() )

#define FUNCTION( f ) FUNCTION_ALIAS( f, f )


#if defined(WIN32)
#   define Y60_APE_MODULE_DECL __declspec( dllexport )
#else
#   define Y60_APE_MODULE_DECL 
#endif

#define Y60_APE_MODULE( name )                                                  \
template <int LineNo> struct ape_tag {};                                        \
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
