#ifndef Y60_APE_CALLER_INCLUDED
#define Y60_APE_CALLER_INCLUDED

#include "invoke.h"
#include "arguments.h"

namespace y60 { namespace ape { namespace detail {

template <typename F, typename Id, typename Sig>
class caller {
    public:
        static
        JSBool
        call(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            try {
                arguments<F, Sig> args(cx, argc, argv);
                invoke(invoke_tag<F>(), f_, args, cx, obj, rval );
                return JS_TRUE;
            } Y60_APE_CATCH_BLOCKS;
        }
        static void init(F func) {
            if (f_ != 0) { 
                throw ape_error("duplicate initialization of function ptr",
                        PLUS_FILE_LINE);
            }
            f_ = func;
        }
    private:
        static F f_;
};

template <typename F, typename Id, typename Sig> F caller<F, Id, Sig>::f_ = 0;

template <typename F, typename Id, typename Sig>
JSNative
get_caller( F f, Sig const& sig) {
    typedef caller<F,Id,Sig> caller_type;
    caller_type::init( f );
    return & caller_type::call;
}

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_CALLER_INCLUDED
