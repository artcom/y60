#if !defined(BOOST_PP_IS_ITERATING)
#   ifndef Y60_APE_INVOKE_INCLUDED
#       define Y60_APE_INVOKE_INCLUDED

#       include "y60_ape_settings.h"

#       include <boost/preprocessor/iterate.hpp>
#       include <boost/preprocessor/repetition/enum.hpp>
#       include <boost/type_traits/is_member_function_pointer.hpp>

#       include <js/spidermonkey/jsapi.h>

#       include <asl/base/TraceUtils.h>
#       include <y60/jsbase/JScppUtils.h>

#       include "exception.h"
#       include "arguments.h"
#       include "class.h"

namespace y60 { namespace ape { namespace detail {

template <bool returns_void, int arity> struct deduce_tag {};

/** A discriminator for the diffrent call semantics. Used to distinguish
 *  between void and non-void calls and the diffrent arities.
 */ 
template <typename F, typename Sig>
struct invoke_discriminator {
    typedef deduce_tag<
        returns_void<Sig>::value,
        arity<F>::value > type;
};

/** Generic invoker template. Just throws a compiletime exception if
 *  the maximum arity is exceeded
 */
template <typename F, typename Id, typename Sig,
          typename DeduceTag = typename invoke_discriminator<F,Sig>::type>
class invoker {
    public:
        static
        JSBool
        invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            // COMPILE TIME ASSERTION: Y60_APE_MAX_ARITY exceeded.
            // You probably got here because you try to wrap a function with
            // more arguments than the current value of Y60_APE_MAX_ARITY.
            // Either define Y60_APE_MAX_ARITY to a higher value before
            // including any ape headers or rethink your design.
            BOOST_STATIC_ASSERT(sizeof(F) == 0);
            return JS_FALSE;
        }
};

template <typename Class, typename F, typename Id, typename Sig,
          typename DeduceTag = typename invoke_discriminator<F,Sig>::type>
class member_invoker {
    public:
        static
        JSBool
        invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            // COMPILE TIME ASSERTION: Y60_APE_MAX_ARITY exceeded.
            // You probably got here because you try to wrap a function with
            // more arguments than the current value of Y60_APE_MAX_ARITY.
            // Either define Y60_APE_MAX_ARITY to a higher value before
            // including any ape headers or rethink your design.
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

// XXX: Hendrik is right. This is too much duplicated code.
/** invoker specialization for calls to free functions of arity N with return
 *  value
 */
template <typename F, typename Id, typename Sig>
class invoker<F, Id, Sig, deduce_tag<false, N> > {
    public:
        static
        JSBool
        invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            try {
                arguments<F,Sig> args(cx, argc, argv);
                *rval = jslib::as_jsval(cx,
                        f( BOOST_PP_ENUM( N, Y60_APE_ARG_REF, args ) ) );
                return JS_TRUE;
            } Y60_APE_CATCH_BLOCKS;
        }
        static void init(F func) {
            if (f != 0) { 
                throw ape_error("duplicate initialization of function ptr",
                        PLUS_FILE_LINE);
            }
            f = func; 
        }
    private:
        static F f;
};

template <typename F, typename Id, typename Sig>
F invoker<F, Id, Sig, deduce_tag<false, N> >::f = 0;

/** invoker specialization for calls to free functions of arity N without 
 * return value
 */
template <typename F, typename Id, typename Sig>
class invoker<F, Id, Sig, deduce_tag<true, N> > {
    public:
        static
        JSBool
        invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            try {
                arguments<F, Sig> args(cx, argc, argv);
                f( BOOST_PP_ENUM( N, Y60_APE_ARG_REF, args  ) );
                return JS_TRUE;
            } Y60_APE_CATCH_BLOCKS;
        }
        static void init(F func) {
            if (f != 0) { 
                throw ape_error("duplicate initialization of function ptr",
                        PLUS_FILE_LINE);
            }
            f = func;
        }
    private:
        static F f;
};

template <typename F, typename Id, typename Sig>
F invoker<F, Id, Sig, deduce_tag<true, N> >::f = 0;

template <typename> class class_binding;
/** invoker specialization for calls to member functions of arity N with return
 *  value
 */
template <typename Class, typename F, typename Id, typename Sig>
class member_invoker<Class, F, Id, Sig, deduce_tag<false, N> > {
    public:
        static
        JSBool
        invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            typedef class_binding<Class> cb;
            try {

                if ( ! JS_InstanceOf(cx, obj, & cb::js_class, argv)) {
                    throw ape_error("object is not of this class", PLUS_FILE_LINE);
                }
                Class * n = static_cast<Class*>( JS_GetPrivate( cx, obj));
                if ( ! n ) {
                    throw ape_error("failed to get native instance", PLUS_FILE_LINE);
                }
                arguments<F,Sig> args(cx, argc, argv);
                *rval = jslib::as_jsval(cx,
                        (*n.*f)( BOOST_PP_ENUM( N, Y60_APE_ARG_REF, args ) ) );
                return JS_TRUE;
            } Y60_APE_CATCH_BLOCKS;
        }
        static void init(F func) {
            if (f != 0) { 
                throw ape_error("duplicate initialization of function ptr",
                        PLUS_FILE_LINE);
            }
            f = func; 
        }
    private:
        static F f;
};

template <typename Class, typename F, typename Id, typename Sig>
F member_invoker<Class, F, Id, Sig, deduce_tag<false, N> >::f = 0;

/** invoker specialization for calls to member functions of arity N without 
 * return value
 */
template <typename Class, typename F, typename Id, typename Sig>
class member_invoker<Class, F, Id, Sig, deduce_tag<true, N> > {
    public:
        typedef F function_type;
        typedef Sig signature_type;

        static
        JSBool
        invoke(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            try {

                arguments<F,Sig> args(cx, argc, argv);
                f( BOOST_PP_ENUM( N, Y60_APE_ARG_REF, args  ) );
                return JS_TRUE;
            } Y60_APE_CATCH_BLOCKS;
        }
        static void init(F * func) {
            if (f != 0) { 
                throw ape_error("duplicate initialization of function ptr",
                        PLUS_FILE_LINE);
            }
            f = func;
        }
    private:
        static F f;
};

template <typename Class, typename F, typename Id, typename Sig>
F member_invoker<Class, F, Id, Sig, deduce_tag<true, N> >::f = 0;

#   undef Y60_APE_ARG_REF

#endif // BOOST_PP_IS_ITERATING
