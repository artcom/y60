#include <iostream>
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

struct foo {
    const char * bar() { return "foobar\n"; }
};

//=== The code to wrap this stuff =============================================
#include "yape.h" // Woah ... well, we will see ...

#define SIG(func) typeof(&func), & func

Y60_JAVASCRIPT_MODULE( y60HelloWorld ) {
    using namespace y60::ape;

    function<SIG(greet)>("greet");
    function<SIG(greet2)>("greet2");
//    function<SIG(greet_n)>("greet_n");

    class_<foo>("foo").
        function("bar", & foo::bar );
}
