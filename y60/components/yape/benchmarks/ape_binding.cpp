#include <y60/components/yape/yape.h>

#include "bench_functions.h"

namespace {

Y60_APE_MODULE( ape_bench_ape_binding ) {
    using namespace y60::ape;
    using namespace y60::ape::bench;
    Y60_APE_NS_SCOPE()
        . function( "ape_add", add )
        . function( "ape_add8", add8)
        ;

    class_<adder>("ape_adder")
        . function("set", & adder::set )
        . function("add", & adder::add )
        . function("add8", & adder::add8 )
        ;
}

}
