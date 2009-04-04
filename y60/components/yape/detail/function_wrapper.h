#ifndef Y60_APE_FUNCTION_WRAPPER_INCLUDED
#define Y60_APE_FUNCTION_WRAPPER_INCLUDED

#include <y60/components/yape/y60_ape_settings.h>

#include "invoke.h"
#include "arguments.h"

namespace y60 { namespace ape { namespace detail {

template <typename F, typename Id, typename Sig>
class function_wrapper {
    public:
        static
        JSBool
        call(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            try {
                arguments<F, Sig> args(cx, argc, argv);
                invoke( invoke_tag<F>(), f_, args, cx, obj, rval );
                return JS_TRUE;
            } Y60_APE_CATCH_BLOCKS;
        }
        static void init(F func) {
            if (f_ != 0) { 
                throw ape_error("duplicate initialization of function pointer",
                        PLUS_FILE_LINE);
            }
            f_ = func;
        }
    private:
        static F f_;
};

template <typename F, typename Id, typename Sig> F function_wrapper<F, Id, Sig>::f_ = 0;

template <typename F, typename Id, typename Sig>
JSNative
get_function_wrapper( F f, Sig const& sig) {
    typedef function_wrapper<F,Id,Sig> function_wrapper_type;
    function_wrapper_type::init( f );
    return & function_wrapper_type::call;
}

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_FUNCTION_WRAPPER_INCLUDED
