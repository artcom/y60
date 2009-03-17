#ifndef Y60_APE_CV_QUALIFIER_MACROS_INCLUDED
#define Y60_APE_CV_QUALIFIER_MACROS_INCLUDED

#include "y60_ape_settings.h"

# define Y60_APE_APPLY(x) BOOST_PP_CAT(Y60_APE_APPLY_, x)

# define Y60_APE_APPLY_Y60_APE_ITEM(v) v
# define Y60_APE_APPLY_Y60_APE_NIL

#   define Y60_APE_CV_QUALIFIER( i ) \
            Y60_APE_APPLY( BOOST_PP_TUPLE_ELEM(4, i, Y60_APE_CV_QUALIFIER_I))

#   define Y60_APE_CV_QUALIFIER_I               \
            (                                   \
                Y60_APE_NIL,                    \
                Y60_APE_ITEM(const),            \
                Y60_APE_ITEM(volatile),         \
                Y60_APE_ITEM(const volatile)    \
            )

#endif // Y60_APE_CV_QUALIFIER_MACROS_INCLUDED
