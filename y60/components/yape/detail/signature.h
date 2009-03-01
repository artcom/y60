#if ! defined( BOOST_PP_IS_ITERATING )

#ifndef Y60_APE_SIGNATURE_INCLUDED
#   define Y60_APE_SIGNATURE_INCLUDED

#   include <boost/preprocessor/repeat.hpp>
#   include <boost/preprocessor/enum.hpp>
#   include <boost/preprocessor/enum_params.hpp>
#   include <boost/preprocessor/empty.hpp>
#   include <boost/preprocessor/arithmetic/sub.hpp>
#   include <boost/preprocessor/iterate.hpp>

#   include <boost/preprocessor/debug/line.hpp>
#   include <boost/preprocessor/arithmetic/sub.hpp>
#   include <boost/preprocessor/arithmetic/inc.hpp>
#   include <boost/preprocessor/repetition/enum_trailing_params.hpp>

#   include <boost/mpl/if.hpp>
#   include <boost/mpl/vector/vector20.hpp> // XXX choose size based on arity
#   include <boost/type_traits/is_convertible.hpp>

#   include "preprocessor.h"


#   define Y60_APE_LIST_INC(n)        \
            BOOST_PP_CAT(boost::mpl::vector, BOOST_PP_INC(n))

namespace y60 { namespace ape { namespace detail {

// A metafunction returning C1 if C1 is derived from C2, and C2
// otherwise
template <class C1, class C2>
struct most_derived : public boost::mpl::if_< boost::is_convertible<C1*,C2*>, C1, C2 > {};
                            

#   define BOOST_PP_ITERATION_PARAMS_1                                   \
            (3, (0, Y60_APE_MAX_ARITY, <y60/components/yape/detail/signature.h>))
#   include BOOST_PP_ITERATE()
#   undef Y60_APE_LIST_INC

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_SIGNATURE_INCLUDED

#elif BOOST_PP_ITERATION_DEPTH() == 1

#   define N BOOST_PP_ITERATION()

template <
class RT BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, class T)>
inline Y60_APE_LIST_INC(N)<
    RT BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, T)>
get_signature(RT(*)(BOOST_PP_ENUM_PARAMS_Z(1, N, T)), void* = 0)
{
    return Y60_APE_LIST_INC(N)<
        RT BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, T)
        >();
}

#   undef N
#   define BOOST_PP_ITERATION_PARAMS_2 \
            (3, (0, 3, <y60/components/yape/detail/signature.h>))
#   include BOOST_PP_ITERATE()

#else 

#   define N BOOST_PP_RELATIVE_ITERATION(1)
#   define Q Y60_APE_CV_QUALIFIER(BOOST_PP_ITERATION())

#if 0
template <
class RT, class ClassT BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, class T)>
inline Y60_APE_LIST_INC(BOOST_PP_INC(N))<
    RT, ClassT& BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, T)>
get_signature(RT(ClassT::*)(BOOST_PP_ENUM_PARAMS_Z(1, N, T)) Q)
{
    return Y60_APE_LIST_INC(BOOST_PP_INC(N))<
        RT, ClassT& BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, T)
        >();
}

template <
class Target
, class RT
    , class ClassT
BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, class T)
    >
    inline Y60_APE_LIST_INC(BOOST_PP_INC(N))<
    RT
    , typename most_derived<Target, ClassT>::type&
BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, T)
    >
    get_signature(
            RT(ClassT::*)(BOOST_PP_ENUM_PARAMS_Z(1, N, T)) Q
            , Target*
            )
{
    return Y60_APE_LIST_INC(BOOST_PP_INC(N))<
        RT
        , BOOST_DEDUCED_TYPENAME most_derived<Target, ClassT>::type&
        BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, T)
        >();
}
#endif

# undef Q
# undef N

#endif // BOOST_PP_IS_ITERATING
