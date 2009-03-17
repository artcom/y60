#include <y60/components/yape/yape.h>
#include <string>
#include <iostream>

namespace {

typedef const char * const_char_ptr;

void           void_func()                  { std::cout << "void_func()" << std::endl; }
const_char_ptr const_char_ptr_func()        { return "const_char_ptr_func()"; }
int            int_func_int(int i)          { return i; }
int            int_func_intcr(int const& i) { return i; }

void void_func_intcr_stringcr(int const& i, std::string const& m) {
    std::cout << m << i << std::endl;
}

//==== Module Code =============================================================

Y60_APE_MODULE( ape_test_functions ) {

    Y60_APE_NS_SCOPE()
        . function( "void_func",                void_func)
        . function( "const_char_ptr_func",      const_char_ptr_func)
        
        . function( "int_func_int",             int_func_int)
        . function( "int_func_intcr",           int_func_intcr)
        . function( "void_func_intcr_stringcr", void_func_intcr_stringcr)
        ;
}

}
