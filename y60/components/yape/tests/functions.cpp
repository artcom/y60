#include <y60/components/yape/yape.h>
#include <iostream>

namespace {

typedef const char * const_char_ptr;

void           void_func()                  { std::cout << "void_func()" << std::endl; }
const_char_ptr const_char_ptr_func()        { return "const_char_ptr_func()"; }
int            int_func_int(int i)          { return i; }
int            int_func_intcr(int const& i) { return i; }

//==== Module Code =============================================================

Y60_APE_MODULE( ape_test_functions ) {
    Y60_APE_NS_SCOPE()
        . function( void_func,           "void_func")
        . function( const_char_ptr_func, "const_char_ptr_func")
        
        . function( int_func_int,        "int_func_int")
        . function( int_func_intcr,      "int_func_intcr");
}

}
