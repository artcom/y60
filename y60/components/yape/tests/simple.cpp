#include <y60/components/yape/yape.h>

namespace {

const char * func() { return "func()"; }

struct simple_class {
    const char * mem_func() { return "simple_class::mem_func()"; }
    static const char * static_mem_func() { return "simple_class::static_mem_func()"; }

    int data_member;
};

Y60_APE_MODULE( ape_test_simple ) {
    
    Y60_APE_NS_SCOPE()
        . function( "func", func );

    class_<simple_class>("simple_class")
        . function( "mem_func",        & simple_class::mem_func)
        . function( "static_mem_func", & simple_class::static_mem_func)
        . property( "data_member",     & simple_class::data_member)
        ;
}

}

