#ifndef Y60_APE_MODULE_INITIALIZER_INCLUDED
#define Y60_APE_MODULE_INITIALIZER_INCLUDED

#include "y60_ape_settings.h"

#include <vector>

#include <js/spidermonkey/jscntxt.h>

#include <asl/base/Logger.h>
#include <y60/components/yape/signature.h>
#include <y60/components/yape/invoke.h>

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

        template <typename FuncT, FuncT * Func>
        void function(const char * name) {
            JSFunctionSpec js_func;
            js_func.name  = name;
            js_func.call  = detail::get_invoker<FuncT,Func>( detail::get_signature( Func ) );
            js_func.nargs = detail::argument_count( detail::get_signature( Func ));
            js_func.flags = 0;
            js_func.extra = 0; // number of arg slots for local GC roots

            functions_.push_back( js_func );
        }

        template <typename Class>
        class_wrapper<Class> class_(const char * name) { return class_wrapper<Class>(name, *this); }

    protected:
        template <typename Class> friend class class_wrapper;
        JSContext * cx_;
        JSObject * root_;

        std::vector<JSFunctionSpec> functions_;
};

}} // end of namespace ape, y60

#endif // Y60_APE_MODULE_INITIALIZER_INCLUDED
