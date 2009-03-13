#if !defined(BOOST_PP_IS_ITERATING)
#   ifndef Y60_APE_INVOKE_INCLUDED
#       define Y60_APE_INVOKE_INCLUDED

#       include "y60_ape_settings.h"

#       include <boost/tuple/tuple.hpp>
#       include <boost/type_traits/is_member_function_pointer.hpp>
#       include <boost/preprocessor/iterate.hpp>
#       include <boost/preprocessor/repetition/enum.hpp>

#       include <y60/jsbase/JScppUtils.h>

#       include "meta.h"

namespace y60 { namespace ape { namespace detail {

template <bool returns_void, bool is_memberfunction, int arity>
struct invoke_tag_ {};

template <class F>
struct invoke_tag : invoke_tag_<
                    returns_void<F>::value,
                    boost::is_member_function_pointer<F>::value,
                    arity<F>::value > {}; 

template <typename C> class class_wrapper;

#       define Y60_APE_ARG_REF(z, n, var) boost::get< n >( var )

#       define BOOST_PP_ITERATION_PARAMS_1 \
            (3, (0, Y60_APE_MAX_ARITY, <y60/components/yape/invoke.h>))
#       include BOOST_PP_ITERATE()


}}} // end of namespace detail, ape, y60

#   endif // Y60_APE_INVOKE_INCLUDED
#else // BOOST_PP_IS_ITERATING

#   define N BOOST_PP_ITERATION()

template <typename F, typename A>
inline
void
invoke(invoke_tag_<false,false,N>, F f, A & args, JSContext * cx, JSObject * obj, jsval * rval) {
    *rval = jslib::as_jsval(cx, f( BOOST_PP_ENUM( N, Y60_APE_ARG_REF, args  ) ));
}

template <typename F, typename A>
inline
void
invoke(invoke_tag_<true,false,N>, F f, A & args, JSContext * cx, JSObject * obj, jsval * rval) {
    f( BOOST_PP_ENUM( N, Y60_APE_ARG_REF, args  ) );
}

template <typename F, typename A>
inline
void
invoke(invoke_tag_<false,true,N>, F f, A & args, JSContext * cx, JSObject * obj, jsval * rval) {
    typedef typename get_member_function_class2<F>::type C;
    C * n = class_wrapper<C>::unwrap_native(cx, obj);
    *rval = jslib::as_jsval(cx, (*n.*f)( BOOST_PP_ENUM( N, Y60_APE_ARG_REF, args  ) ));
}

template <typename F, typename A>
inline
void
invoke(invoke_tag_<true,true,N>, F f, A & args, JSContext * cx, JSObject * obj, jsval * rval) {
    typedef typename get_member_function_class2<F>::type C;
    C * n = class_wrapper<C>::unwrap_native(cx, obj);
    (*n.*f)( BOOST_PP_ENUM( N, Y60_APE_ARG_REF, args  ) );
}

#endif // BOOST_PP_IS_ITERATING
