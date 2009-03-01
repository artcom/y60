#ifndef Y60_JAVASCRIPT_INCLUDED
#define Y60_JAVASCRIPT_INCLUDED

#include <vector>
#include <typeinfo>
#include <boost/mpl/size.hpp>
#include <js/spidermonkey/jscntxt.h>

#include <asl/base/Logger.h>
#include <y60/components/yape/detail/signature.h>
#include <y60/components/yape/detail/module_loader.h>
#include <y60/components/yape/detail/invoke.h>

namespace y60 { namespace ape {

template <typename Class>
class js_class_wrapper {
    public:
        template <typename FuncPtrT>
        void function(const char * name, FuncPtrT f) {}
};

namespace detail {

template < typename FuncPtrT, FuncPtrT Func, typename Sig >
JSNative
get_invoker(Sig const& sig) {
    return & detail::invoker<FuncPtrT,Func,Sig>::invoke; 
}

} // end of namespace detail

class module_initializer {
    public:
        module_initializer(JSContext * cx, JSObject * root) :
            cx_(cx), root_(root) {}

        ~module_initializer() {
            JSFunctionSpec js_fs_end;
            js_fs_end.name = 0;
            functions_.push_back(js_fs_end);

            if ( ! JS_DefineFunctions(cx_, root_, &(*functions_.begin()))) {
                AC_PRINT << "Failed to init functions";
            }
        }

        template <typename FuncPtrT, FuncPtrT Func>
        void function(const char * name) {
            JSFunctionSpec js_func;
            js_func.name  = name;
            js_func.call  = detail::get_invoker<FuncPtrT,Func>( detail::get_signature( Func ) );
            js_func.nargs = detail::argument_count( detail::get_signature( Func ));
            js_func.flags = 0;
            js_func.extra = 0; // number of arg slots for local GC roots

            functions_.push_back( js_func );
        }

        template <typename Class>
        js_class_wrapper<Class> class_(const char * name) { return js_class_wrapper<Class>(); }

    private:
        JSContext * cx_;
        JSObject * root_;

        std::vector<JSFunctionSpec> functions_;
};

}}

#if defined(WIN32)
#   define Y60_JAVASCRIPT_MODULE_DECL __declspec( dllexport )
#else
#   define Y60_JAVASCRIPT_MODULE_DECL 
#endif

#define Y60_JAVASCRIPT_MODULE( name )                                       \
                                                                            \
struct name ## _module_initializer :                                        \
        public y60::ape::module_initializer                          \
{                                                                           \
    name ## _module_initializer(JSContext * cx, JSObject * root) :          \
            y60::ape::module_initializer(cx,root) {}                 \
    void init_module();                                                     \
};                                                                          \
                                                                            \
extern "C" Y60_JAVASCRIPT_MODULE_DECL                                       \
asl::PlugInBase * name ## _instantiatePlugIn(asl::DLHandle theDLHandle) {   \
    using y60::ape::detail::module_loader;                           \
    return new module_loader<name ## _module_initializer>(theDLHandle);     \
}                                                                           \
                                                                            \
void name ## _module_initializer::init_module()

#endif // Y60_JAVASCRIPT_INCLUDED
