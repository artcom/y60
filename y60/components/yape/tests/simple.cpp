#include <y60/components/yape/yape.h>

namespace {

const char * func() { return "func()"; }

struct simple_class {
    const char * mem_func() { return "simple_class::mem_func()"; }
};

Y60_APE_MODULE( ape_test_simple ) {
    
    Y60_APE_NS_SCOPE()
        . function( func, "func" );

    class_<simple_class>("simple_class")
        . function( & simple_class::mem_func, "mem_func");
}

}

