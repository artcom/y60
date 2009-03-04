#if !defined(BOOST_PP_IS_ITERATING)
#   ifndef Y60_APE_INVOKE_INCLUDED
#       define Y60_APE_INVOKE_INCLUDED

#       include "y60_ape_settings.h"

#       include <boost/mpl/front.hpp>
#       include <boost/type_traits.hpp>
#       include <boost/preprocessor/cat.hpp>
#       include <boost/preprocessor/iterate.hpp>
#       include <boost/preprocessor/arithmetic/add.hpp>
#       include <boost/preprocessor/iterate.hpp>
#       include <boost/preprocessor/enum_params.hpp>

#       include <js/spidermonkey/jsapi.h>

#       include <asl/base/TraceUtils.h>
#       include <asl/base/Logger.h>
#       include <y60/jsbase/JScppUtils.h>

#       include "exception.h"
#       include "signature_utils.h"
#       include "preprocessor.h"

namespace y60 { namespace ape { namespace detail {

template <bool returns_void, int arity> struct invoke_tag {};

template <typename Sig>
struct dispatch {
    typedef invoke_tag< returns_void<Sig>::value, arity<Sig>::value > type;
};

inline
void
check_argument_count( int argc, int N ) {
    if ( argc != N ) {
        std::ostringstream os;
        os << "expected " << N << " arguments but got " << argc;
        throw bad_arguments( os.str(), PLUS_FILE_LINE );
    }
}

template <typename T>
inline
void
arg_from_jsval(JSContext * cx, jsval * const& argv, uintN i, T & out) {
    if ( ! jslib::convertFrom(cx, argv[i], out )) {
        JSString * js_string = JS_ValueToString(cx, argv[0]);
        std::string js_string_rep;
        if(js_string) {
            js_string_rep = JS_GetStringBytes(js_string);
        }
        JS_RemoveRoot(cx, & js_string );
        std::ostringstream os;
        os << "could not convert argument " << i << " with value '"
           << js_string_rep << "' to native type '" 
           << asl::demangled_name<T>() << "'";
        throw bad_arguments(os.str(), PLUS_FILE_LINE);
    }
}

template <typename FuncT,
          FuncT * Func,
          typename Sig,
          typename Tag = typename dispatch<Sig>::type>
struct invoker {
    static
    JSBool
    invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        BOOST_STATIC_ASSERT(sizeof(FuncT) == 0);
        return JS_FALSE;
    }
};

template < typename FuncT, FuncT * Func, typename Sig >
JSNative
get_invoker(Sig const& sig) {
    return & detail::invoker<FuncT,Func,Sig>::invoke; 
}


#       define Y60_APE_DECL_ARG(z, n, Sig )                       \
                typename argument_storage<Sig,n>::type arg ## n = \
                        typename argument_storage<Sig,n>::type() ;

#       define Y60_APE_INIT_ARG(z, n, Sig) \
                arg_from_jsval(cx, argv, n, arg ## n );

#       define BOOST_PP_ITERATION_PARAMS_1 \
            (3, (0, Y60_APE_MAX_ARITY, <y60/components/yape/invoke.h>))
#       include BOOST_PP_ITERATE()

#       undef Y60_APE_DECL_ARG

}}} // end of namespace detail, ape, y60

#   endif // Y60_APE_INVOKE_INCLUDED

#else // BOOST_PP_IS_ITERATING

#   define N BOOST_PP_ITERATION()

template <typename FuncT,
          FuncT * Func,
          typename Sig>
struct invoker<FuncT, Func, Sig, invoke_tag<false, N> > {
    static
    JSBool
    invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        try {
            AC_TRACE << "invoke: non_void " << N << " args";
            check_argument_count( argc, N );
            typedef boost::function_traits<
                typename boost::remove_pointer<FuncT>::type > func_trait;
            BOOST_PP_REPEAT( N, Y60_APE_DECL_ARG, Sig )
            BOOST_PP_REPEAT( N, Y60_APE_INIT_ARG, Sig );
            *rval = jslib::as_jsval(cx, Func( BOOST_PP_ENUM_PARAMS(N, arg) ));
        } HANDLE_CPP_EXCEPTION;
        return JS_TRUE;
    }
};

template <typename FuncT,
          FuncT * Func,
          typename Sig>
struct invoker<FuncT, Func, Sig, invoke_tag<true, N> > {
    static
    JSBool
    invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        try {
            AC_TRACE << "invoke: void " << N << " args";
            check_argument_count( argc, N );
            typedef boost::function_traits<
                typename boost::remove_pointer<FuncT>::type > func_trait;
            BOOST_PP_REPEAT( N, Y60_APE_DECL_ARG, Sig );
            BOOST_PP_REPEAT( N, Y60_APE_INIT_ARG, Sig );
            Func( BOOST_PP_ENUM_PARAMS(N, arg) );
            *rval = JSVAL_VOID;
        } HANDLE_CPP_EXCEPTION;
        return JS_TRUE;
    }
};

#endif // BOOST_PP_IS_ITERATING
