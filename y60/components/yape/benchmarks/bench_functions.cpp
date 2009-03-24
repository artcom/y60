#include "bench_functions.h"

namespace y60 { namespace ape { namespace bench {

int add(int a, int b) {
    return a + b;
}

int add8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) {
    return a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8;
}

void adder::set(int a) { a_ = a; }
int adder::add(int b, int c) { return a_ + b + c; }

int adder::add8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) {
    return a_ + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8;
}


}}}
