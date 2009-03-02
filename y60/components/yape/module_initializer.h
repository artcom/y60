#ifndef Y60_APE_MODULE_INITIALIZER_INCLUDED
#define Y60_APE_MODULE_INITIALIZER_INCLUDED

#include <vector>

#include <js/spidermonkey/jscntxt.h>

#include <asl/base/Logger.h>
#include <y60/components/yape/detail/signature.h>
#include <y60/components/yape/detail/module_loader.h>
#include <y60/components/yape/detail/invoke.h>

namespace y60 { namespace ape {

template <typename Class> class class_wrapper;

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
        class_wrapper<Class> class_(const char * name) { return class_wrapper<Class>(*this); }

    protected:
        template <typename Class> friend class class_wrapper;
        JSContext * cx_;
        JSObject * root_;

        std::vector<JSFunctionSpec> functions_;
};

}} // end of namespace ape, y60

#endif // Y60_APE_MODULE_INITIALIZER_INCLUDED
