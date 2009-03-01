#ifndef Y60_JAVASCRIPT_PREPROCESSOR
#define Y60_JAVASCRIPT_PREPROCESSOR

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#define Y60_JAVASCRIPT_APPLY(x) BOOST_PP_CAT(Y60_JAVASCRIPT_APPLY_, x)

#define Y60_JAVASCRIPT_APPLY_Y60_JAVASCRIPT_ITEM(v) v
#define Y60_JAVASCRIPT_APPLY_Y60_JAVASCRIPT_NIL

#ifndef Y60_JAVASCRIPT_MAX_ARITY
#   define Y60_JAVASCRIPT_MAX_ARITY 15
#endif

#define Y60_JAVASCRIPT_CV_QUALIFIER(i)                               \
        Y60_JAVASCRIPT_APPLY(                                        \
            BOOST_PP_TUPLE_ELEM(4, i, Y60_JAVASCRIPT_CV_QUALIFIER_I) \
        )

#define Y60_JAVASCRIPT_CV_COUNT 4

#define Y60_JAVASCRIPT_CV_QUALIFIER_I               \
            (                                       \
                Y60_JAVASCRIPT_NIL,                 \
                Y60_JAVASCRIPT_ITEM(const),         \
                Y60_JAVASCRIPT_ITEM(volatile),      \
                Y60_JAVASCRIPT_ITEM(const volatile) \
            )

#endif // Y60_JAVASCRIPT_PREPROCESSOR
