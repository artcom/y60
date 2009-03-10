#include "hello_world.h"

#include <y60/components/yape/yape.h>

namespace {

Y60_APE_MODULE( ape_hello_world ) {
    using namespace y60::ape;
    
    FUNCTIONS()
        . function( greet,           "greet" )
        . function( greet2,          "greet2")
        . function( greet_n,         "greet_n")
        . function( greet_more_args, "greet_more_args");

    class_<foo>("foo")
        . function( & foo::bar, "bar");/*
        . function( & foo::foobar, "foobar");*/
}

}

