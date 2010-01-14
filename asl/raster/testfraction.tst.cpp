/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include <assert.h>
#include <iomanip>

#include "fraction.h"
#include <asl/base/UnitTest.h>


using namespace std;
using namespace asl;

template <class Int>
class GCDTest : public UnitTest {
public:
    GCDTest(string myTypeName)
        : UnitTest(strdup(string(string("GCDTest<")+myTypeName+">").c_str())) {  }
        void run() {
            Int a = 20451;
            Int b = 6035;
            Int c = 17;

            //Int zero(0);
            Int one(1);
            Int two(2);
            Int three(3);
            Int four(4);
            //Int five(5);
            //Int six(6);
            //Int seven(7);
            Int eight(8);
            //Int nine(9);
            //Int ten(10);
            Int twelve(12);

            //Int minusone(-1);
            //Int minustwo(-2);
            //Int minusthree(-3);

            ENSURE(gcd(a, b) == c);
            ENSURE(gcd(one, one) == one);
            ENSURE(gcd(two, two) == two);
            ENSURE(gcd(three, three) == three);
            ENSURE(gcd(twelve, eight) == four);
            ENSURE(gcd(eight, twelve) == four);
        }
private:
};

template <class fraction>
class FractionTest : public UnitTest {
public:
    FractionTest(string myTypeName)
        : UnitTest(strdup(string(string("FractionTest<")+myTypeName+">").c_str()))
    {  }
    void run() {
        fraction zero(0);
        fraction one(1);
        fraction two(2);
        fraction three(3);
        fraction four(4);
        fraction five(5);
        fraction six(6);
        fraction seven(7);
        fraction eight(8);
        fraction nine(9);
        fraction ten(10);
        fraction twelve(12);

        fraction minusone(-1);
        fraction minustwo(-2);
        fraction minusthree(-3);

        fraction minushalf(-1, 2);

        fraction half(1, 2);
        fraction threehalf(3, 2);
        fraction twothird(2, 3);
        fraction onethird(1, 3);
        fraction quarter(1, 4);
        fraction onefifth(1, 5);
        fraction onesixth(1, 6);
        fraction onetenth(1, 10);
        fraction onetwelvth(1, 12);

        ENSURE(zero == zero);
        ENSURE(one == one);
        ENSURE(zero + zero == zero);
        ENSURE(zero + one == one);
        ENSURE(one + zero == one);
        ENSURE(one + one == two);
        ENSURE(one + two == three);
        ENSURE(two + one == three);
        ENSURE(two + two == four);

        ENSURE(minusone == -one);

        ENSURE(zero + one == one);
        ENSURE(one - zero == one);
        ENSURE(one - one == zero);
        ENSURE(one - two == minusone);
        ENSURE(two - one == one);
        ENSURE(two - three == -one);

        ENSURE(one * one == one);
        ENSURE(one * zero == zero);
        ENSURE(one * two == two);
        ENSURE(two * two == four);
        ENSURE(three * two == six);

        ENSURE(one / one == one);
        ENSURE(zero / one == zero);
        ENSURE(one / two == half);
        ENSURE(two / two == one);
        ENSURE(one / half == two);

        ENSURE(one / half == two);

        ENSURE(one * minusone == minusone);
        ENSURE(minusone * minusone == one);

        ENSURE(half + half == one);
        ENSURE(onethird + onethird + onethird  == one);
        ENSURE(six * onesixth  == one);
        ENSURE(onesixth * six  == one);
        ENSURE(half * half  == quarter);
        ENSURE(half / half  == one);
        ENSURE(one / three == onethird);
        ENSURE(two / three == twothird);
        ENSURE(minusone * half == minushalf);
        ENSURE(onetwelvth + onetwelvth == onesixth);
        ENSURE(onesixth + twothird + onesixth == one);
        ENSURE(minushalf + one == half);
        ENSURE(twothird - one == -onethird);

        ENSURE(half != onethird);
        ENSURE(half > onethird);
        ENSURE(twothird > half);
        ENSURE(one > minusone);
        ENSURE(threehalf + onetenth > one + half + onetwelvth);

        ENSURE(!(half < onethird));
        ENSURE(!(twothird < half));
        ENSURE(!(one < minusone));
        ENSURE(!(threehalf + onetenth < one + half + onetwelvth));

        ENSURE(floor(half) == 0);
        ENSURE(floor(zero) == 0);
        ENSURE(floor(one) == 1);
        ENSURE(floor(minusone) == -1);
        ENSURE(floor(minushalf) == -1);
        ENSURE(floor(-threehalf) == -2);
        ENSURE(floor(twothird) == 0);
        ENSURE(floor(-twothird) == -1);

        ENSURE(ceil(half) == 1);
        ENSURE(ceil(zero) == 0);
        ENSURE(ceil(one) == 1);
        ENSURE(ceil(minusone) == -1);
        ENSURE(ceil(minushalf) == 0);
        ENSURE(ceil(-threehalf) == -1);
        ENSURE(ceil(twothird) == 1);
        ENSURE(ceil(-twothird) == 0);

        ENSURE(trunc(half) == 0);
        ENSURE(trunc(zero) == 0);
        ENSURE(trunc(one) == 1);
        ENSURE(trunc(minusone) == -1);
        ENSURE(trunc(minushalf) == 0);
        ENSURE(trunc(-threehalf) == -1);
        ENSURE(trunc(twothird) == 0);

        ENSURE(round(half) == 1);
        ENSURE(round(zero) == 0);
        ENSURE(round(one) == 1);
        ENSURE(round(minusone) == -1);
        ENSURE(round(minushalf) == 0);
        ENSURE(round(-threehalf) == -1);
        ENSURE(round(twothird) == 1);
        ENSURE(round(-twothird) == -1);

        ENSURE(floor_fraction(twothird) == twothird);
        ENSURE(floor_fraction(-twothird) == onethird);

        ENSURE(ceil_fraction(twothird) == onethird);
        ENSURE(ceil_fraction(-twothird) == twothird);

        ENSURE(trunc_fraction(twothird) == twothird);
        ENSURE(trunc_fraction(-twothird) == -twothird);

        ENSURE(round_fraction(twothird) == -onethird);
        ENSURE(round_fraction(-twothird) == onethird);

        int q;
        int p = 999;
        double dq = 0;
        fraction result(0);
        for (q = 1; q<=999; q++) {
            dq += q/(double)p;
            result += fraction(q, p);
            //	    cerr << setprecision(10) << "result = " << result << endl;
            //	    cerr << setprecision(10) << "as_double(result) = " << as_double(result) << endl;
            //	    cerr << setprecision(10) << "dq = " << dq << endl;
        }
        ENSURE(result == fraction(500));
    }
private:
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message

        addTest(new GCDTest<int>("<int>"));
        addTest(new FractionTest<base_frac<int> >("base_frac<int>"));
        addTest(new FractionTest<frac<int> >("base_frac<int>"));

    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
