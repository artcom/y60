#include "hello_world.h"

#include <iostream>
#include <sstream>

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

foo::foo() {
    std::cout << "foo::foo()" << std::endl;
}

foo::~foo() {
    std::cout << "foo::~foo()" << std::endl;
}

const char * 
foo::bar() { return "bar"; }
const char * 
foo::foobar() { return "foobar"; }


