#ifndef Y60_JAVASCRIPT_INVOKE_INCLUDED
#define Y60_JAVASCRIPT_INVOKE_INCLUDED

#include <boost/mpl/front.hpp>
#include <boost/type_traits.hpp>

#include <js/spidermonkey/jsapi.h>

#include <asl/base/logger.h>
#include <y60/jsbase/JScppUtils.h>

#include <y60/components/yape/detail/signature_utils.h>

namespace y60 { namespace ape { namespace detail {

template <typename FuncPtrT,
          FuncPtrT Func,
          typename Sig,
          bool returns_void = returns_void<Sig>::value>
struct invoker {
    static
    JSBool
    invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        try {
            *rval = jslib::as_jsval(cx, Func() );
        } HANDLE_CPP_EXCEPTION;
        return JS_TRUE;
    }
};

template <typename FuncPtrT,
          FuncPtrT Func,
          typename Sig>
struct invoker<FuncPtrT, Func, Sig, true> {
    static
    JSBool
    invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        try {
            Func();
            *rval = JSVAL_VOID;
        } HANDLE_CPP_EXCEPTION;
        return JS_TRUE;
    }
};

}}} // end of namespace detail, ape, y60

#endif // Y60_JAVASCRIPT_INVOKE_INCLUDED
