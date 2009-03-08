#include "hello_world.h"

#include <y60/components/yape/yape.h>

#include <asl/base/TraceUtils.h>

namespace { // anonymous namespace is NOT optional. It is a requirement.

Y60_APE_MODULE( ape_hello_world ) {
    using namespace y60::ape;

    FUNCTION( greet );
    FUNCTION( greet2 );
    FUNCTION( greet_n );
    FUNCTION( greet_more_args );
    
//    function< const char* (), greet> ("greet");
//    function< void (),        greet2>("greet2");
//    function< void (int),     greet_n>("greet_n");
//    function< std::string (const std::string &, const int), greet_more_args>("greet_more_args");

    class_<foo>("foo").
        function< const char* (foo::*)(), & foo::bar>("bar"); // hmmm ... foo what?
}

}

