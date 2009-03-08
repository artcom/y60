#if !defined(BOOST_PP_IS_ITERATING)
#   ifndef Y60_APE_INVOKE_INCLUDED
#       define Y60_APE_INVOKE_INCLUDED

#       include "y60_ape_settings.h"

#       include <boost/preprocessor/iterate.hpp>
#       include <boost/preprocessor/repetition/enum.hpp>

#       include <js/spidermonkey/jsapi.h>

#       include <asl/base/TraceUtils.h>
#       include <asl/base/Logger.h>
#       include <y60/jsbase/JScppUtils.h>

#       include "exception.h"
#       include "arguments.h"

namespace y60 { namespace ape { namespace detail {

template <bool returns_void, int arity> struct deduce_tag {};

template <typename Signature>
struct invoke_discriminator {
    typedef deduce_tag<
        returns_void<Signature>::value,
                arity<Signature>::value > type;
};

/* Generic invoker template. Just throws a compiletime exception if
 * the maximum arity is exceeded
 */
template <typename F,
          typename Signature,
          typename UniqueId,
          typename DeduceTag = typename invoke_discriminator<Signature>::type>
class invoker {
    public:
        static
        JSBool
        invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            // Y60_APE_MAX_ARITY exceeded
            BOOST_STATIC_ASSERT(sizeof(F) == 0);
            return JS_FALSE;
        }
};

#       define BOOST_PP_ITERATION_PARAMS_1 \
            (3, (0, Y60_APE_MAX_ARITY, <y60/components/yape/invoke.h>))
#       include BOOST_PP_ITERATE()

}}} // end of namespace detail, ape, y60

#   endif // Y60_APE_INVOKE_INCLUDED

#else // BOOST_PP_IS_ITERATING

#   define N BOOST_PP_ITERATION()
#   define Y60_APE_ARG_REF(z, n, var) boost::get< n >( var )

template <typename F,
          typename Signature,
          typename UniqueId>
class invoker<F, Signature, UniqueId, deduce_tag<false, N> > {
    public:
        static
        JSBool
        invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            try {
                arguments<Signature> args(cx, argc, argv);
                *rval = jslib::as_jsval(cx,
                        f( BOOST_PP_ENUM( N, Y60_APE_ARG_REF, args ) ) );
            } HANDLE_CPP_EXCEPTION;
            return JS_TRUE;
        }
        static void init(F * func) { f = func; }
    private:
        static F * f;
};

template <typename F, typename Signature, typename UniqueId>
F * invoker<F, Signature, UniqueId, deduce_tag<false, N> >::f = 0;

template <typename F,
          typename Signature,
          typename UniqueId>
class invoker<F, Signature, UniqueId, deduce_tag<true, N> > {
    public:
        static
        JSBool
        invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            try {
                arguments<Signature> args(cx, argc, argv);
                f( BOOST_PP_ENUM( N, Y60_APE_ARG_REF, args  ) );
                *rval = JSVAL_VOID; // XXX needed?
            } HANDLE_CPP_EXCEPTION;
            return JS_TRUE;
        }
        static void init(F * func) { f = func; }
    private:
        static F * f;
};

template <typename F, typename Signature, typename UniqueId>
F * invoker<F, Signature, UniqueId, deduce_tag<true, N> >::f = 0;

#   undef Y60_APE_ARG_REF

#endif // BOOST_PP_IS_ITERATING
