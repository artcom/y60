#include <y60/components/yape/yape.h>

#include <boost/preprocessor/repeat.hpp>

#include "benchmark_config.h"

#define TFUNC(z, n, name) void name ## n () {};
#define WTFUNC(z, n, name) . function( #name #n , name ## n )

namespace {

BOOST_PP_REPEAT( NUM_FUNCTIONS, TFUNC, f  )

Y60_APE_MODULE( ape_test_hundred_functions ) {

    Y60_APE_NS_SCOPE()
        BOOST_PP_REPEAT( NUM_FUNCTIONS, WTFUNC, f  );

} // end of ape module

} // end of namespace
