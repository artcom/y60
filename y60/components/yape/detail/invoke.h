#if !defined(BOOST_PP_IS_ITERATING)
#   ifndef Y60_APE_INVOKE_INCLUDED
#       define Y60_APE_INVOKE_INCLUDED

#       include <boost/mpl/front.hpp>
#       include <boost/type_traits.hpp>
#       include <boost/preprocessor/cat.hpp>
#       include <boost/preprocessor/iterate.hpp>
#       include <boost/preprocessor/arithmetic/add.hpp>
#       include <boost/preprocessor/iterate.hpp>
#       include <boost/preprocessor/enum_params.hpp>

#       include <js/spidermonkey/jsapi.h>

#       include <asl/base/Exception.h>
#       include <asl/base/logger.h>
#       include <y60/jsbase/JScppUtils.h>

#       include <y60/components/yape/detail/signature_utils.h>
#       include <y60/components/yape/detail/preprocessor.h>

namespace y60 { namespace ape { namespace detail {

template <bool returns_void, int arity> struct invoke_tag {};

template <typename Sig>
struct dispatch {
    typedef invoke_tag< returns_void<Sig>::value, arity<Sig>::value > type;
};

DEFINE_EXCEPTION( bad_arguments, asl::Exception );

template <typename T>
inline
void
from_jsval(JSContext * cx, jsval const& v, T & out) {
    if ( ! jslib::convertFrom(cx, v, out )) {
        std::ostringstream os;
        throw bad_arguments("Kaputt.", PLUS_FILE_LINE);
    }
}

template <typename FuncPtrT,
          FuncPtrT Func,
          typename Sig,
          typename Tag = typename dispatch<Sig>::type>
struct invoker {
    static
    JSBool
    invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        BOOST_STATIC_ASSERT(sizeof(FuncPtrT) == 0);
        return JS_FALSE;
    }
};

#       define Y60_APE_DECL_ARG(z, n, Sig )                       \
                typename argument_storage<Sig,n>::type arg ## n = \
                        typename argument_storage<Sig,n>::type() ;

#       define Y60_APE_INIT_ARG(z, n, Sig) \
                from_jsval(cx, argv[n], arg ## n );

#       define BOOST_PP_ITERATION_PARAMS_1 \
            (3, (0, Y60_APE_MAX_ARITY, <y60/components/yape/detail/invoke.h>))
#       include BOOST_PP_ITERATE()

#       undef Y60_APE_DECL_ARG

}}} // end of namespace detail, ape, y60

#   endif // Y60_APE_INVOKE_INCLUDED

#else // BOOST_PP_IS_ITERATING

#   define N BOOST_PP_ITERATION()

template <typename FuncPtrT,
          FuncPtrT Func,
          typename Sig>
struct invoker<FuncPtrT, Func, Sig, invoke_tag<false, N> > {
    static
    JSBool
    invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        try {
            AC_TRACE << "invoke: non_void " << N << " args";
            typedef boost::function_traits<
                typename boost::remove_pointer<FuncPtrT>::type > func_trait;
            BOOST_PP_REPEAT( N, Y60_APE_DECL_ARG, Sig )
            *rval = jslib::as_jsval(cx, Func( BOOST_PP_ENUM_PARAMS(N, arg) ));
        } HANDLE_CPP_EXCEPTION;
        return JS_TRUE;
    }
};

template <typename FuncPtrT,
          FuncPtrT Func,
          typename Sig>
struct invoker<FuncPtrT, Func, Sig, invoke_tag<true, N> > {
    static
    JSBool
    invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        try {
            AC_TRACE << "invoke: void " << N << " args";
            typedef boost::function_traits<
                typename boost::remove_pointer<FuncPtrT>::type > func_trait;
            BOOST_PP_REPEAT( N, Y60_APE_DECL_ARG, Sig );
            BOOST_PP_REPEAT( N, Y60_APE_INIT_ARG, Sig );
            Func( BOOST_PP_ENUM_PARAMS(N, arg) );
            *rval = JSVAL_VOID;
        } HANDLE_CPP_EXCEPTION;
        return JS_TRUE;
    }
};

#endif // BOOST_PP_IS_ITERATING
