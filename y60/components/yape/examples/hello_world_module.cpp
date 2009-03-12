#include <y60/components/yape/yape.h>

const char * greet() { return "Hello World."; }

Y60_APE_MODULE( ape_ex_hello_world ) {
    Y60_APE_NS_SCOPE()
        . function( greet, "greet" );
}

