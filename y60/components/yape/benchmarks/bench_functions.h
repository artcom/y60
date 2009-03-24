#ifndef Y60_APE_BENCH_FUNCTIONS_INCLUDED
#define Y60_APE_BENCH_FUNCTIONS_INCLUDED

namespace y60 { namespace ape { namespace bench {

int add(int a, int b);
int add8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);

struct adder {
    void set(int a);
    int add(int b, int c);
    int add8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);

    int a_;
};

}}}

#endif // Y60_APE_BENCH_FUNCTIONS_INCLUDED
