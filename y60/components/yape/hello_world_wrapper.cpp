#include <iostream>
#include <sstream>
#include <asl/base/Logger.h>

//=== Some stuff to wrap ======================================================
const char * greet() {
    return "Hello World.";
}

void greet2() {
    std::cout << "Hello World." << std::endl;
}

void greet_n(int n) {
    std::cout << "Hello World: " << n << std::endl;
}

std::string greet_more_args(const std::string & msg, const int n) {
    std::ostringstream os;
    for(int i = 0; i < n; ++i) {
        os << "Hello World: " << msg << std::endl;
    }
    return os.str();
}

struct foo {
    const char * bar() { return "foobar\n"; }
};

//=== The code to wrap this stuff =============================================
#include "yape.h"

Y60_APE_MODULE( y60HelloWorld ) {
    using namespace y60::ape;

    function<FPTR(greet)>("greet");
    function<FPTR(greet2)>("greet2");
    function<FPTR(greet_n)>("greet_n");
    function<FPTR(greet_more_args)>("greet_more_args");

    class_<foo>("foo").
        function<FPTR(foo::bar)>("bar");
}
