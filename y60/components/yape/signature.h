#if ! defined( BOOST_PP_IS_ITERATING )

#   ifndef Y60_APE_SIGNATURE_INCLUDED
#       define Y60_APE_SIGNATURE_INCLUDED

#       include "y60_ape_settings.h"

#       include <boost/preprocessor/repeat.hpp>
#       include <boost/preprocessor/enum.hpp>
#       include <boost/preprocessor/enum_params.hpp>
#       include <boost/preprocessor/repetition/enum_trailing_params.hpp>

#       include "typelist.h"


#       define Y60_APE_LIST_TYPE(n)        \
                BOOST_PP_CAT(boost::mpl::vector, BOOST_PP_INC(n))

namespace y60 { namespace ape { namespace detail {

#   define BOOST_PP_ITERATION_PARAMS_1                                   \
            (3, (0, Y60_APE_MAX_ARITY, <y60/components/yape/signature.h>))
#   include BOOST_PP_ITERATE()
#   undef Y60_APE_LIST_TYPE

}}} // end of namespace detail, ape, y60

#   endif // Y60_APE_SIGNATURE_INCLUDED

#else // BOOST_PP_IS_ITERATING

#   define N BOOST_PP_ITERATION()

template < class RT BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, class T)>
inline
Y60_APE_LIST_TYPE(N)< RT BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, T)>
get_signature(RT(*)(BOOST_PP_ENUM_PARAMS_Z(1, N, T)), void* = 0) {
    return Y60_APE_LIST_TYPE(N)< RT BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, T) >();
}

#endif // BOOST_PP_IS_ITERATING
