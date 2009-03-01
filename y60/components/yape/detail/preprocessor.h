#ifndef Y60_APE_PREPROCESSOR
#define Y60_APE_PREPROCESSOR

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#define Y60_APE_APPLY(x) BOOST_PP_CAT(Y60_APE_APPLY_, x)

#define Y60_APE_APPLY_Y60_APE_ITEM(v) v
#define Y60_APE_APPLY_Y60_APE_NIL

#ifndef Y60_APE_MAX_ARITY
#   define Y60_APE_MAX_ARITY 1
#endif

#define Y60_APE_CV_QUALIFIER(i)                               \
        Y60_APE_APPLY(                                        \
            BOOST_PP_TUPLE_ELEM(4, i, Y60_APE_CV_QUALIFIER_I) \
        )

#define Y60_APE_CV_COUNT 4

#define Y60_APE_CV_QUALIFIER_I               \
            (                                       \
                Y60_APE_NIL,                 \
                Y60_APE_ITEM(const),         \
                Y60_APE_ITEM(volatile),      \
                Y60_APE_ITEM(const volatile) \
            )

#endif // Y60_APE_PREPROCESSOR
