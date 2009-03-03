#include "hello_world.h"

#include <y60/components/yape/yape.h>

#include <asl/base/TraceUtils.h>

Y60_APE_MODULE( y60HelloWorld ) {
    using namespace y60::ape;

    function< const char* (), greet> ("greet");
    function< void (),        greet2>("greet2");
    function< void (int),     greet_n>("greet_n");
    function< std::string (const std::string &, const int), greet_more_args>("greet_more_args");

    class_<foo>("foo").
        function< const char* (foo::*)(), & foo::bar>("bar"); // hmmm ... foo what?
}
