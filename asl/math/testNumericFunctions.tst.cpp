/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSnumeric: test_numeric_functions.tst.cpp,v $
//
//   $Revision: 1.9 $
//
// Description: unit test for numeric functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "numeric_functions.h"

#include <asl/UnitTest.h>
#include <iostream>

#include <limits.h>

#ifdef LINUX
#include <values.h>
#else
#include <float.h>
#endif

using namespace std;
using namespace asl;

class numeric_functions_UnitTest : public UnitTest {
    public:
        numeric_functions_UnitTest() : UnitTest("numeric_functions_UnitTest") {  }
        void run() {
            testPowerOfTwo();
            testShiftToRange();
            testInterpolate();

            ENSURE(NumericTraits<char>::min() == -128);
            ENSURE(NumericTraits<char>::max() == 127);
            ENSURE(NumericTraits<unsigned int>::min() == 0);
            ENSURE(NumericTraits<unsigned int>::max() == UINT_MAX);
            ENSURE(NumericTraits<int>::min() == INT_MIN);
            ENSURE(NumericTraits<int>::max() == INT_MAX);
            ENSURE(NumericTraits<float>::min() == -FLT_MAX);
            ENSURE(NumericTraits<float>::max() == FLT_MAX);
            
            ENSURE(asl::round(0.0f)  == 0);
            ENSURE(asl::round(0.49f) == 0);
            ENSURE(asl::round(0.5f)  == 1);
            ENSURE(asl::round(0.51f) == 1);
            ENSURE(asl::round(1.0f)  == 1);
            ENSURE(asl::round(0.0)   == 0);
            ENSURE(asl::round(0.49)  == 0);
            ENSURE(asl::round(0.5)   == 1);
            ENSURE(asl::round(0.51)  == 1);
            ENSURE(asl::round(1.0)   == 1);
            ENSURE(asl::round(-1.0)  == -1);
            ENSURE(asl::round(-1.4)  == -1);            
            ENSURE(asl::round(-1.6)  == -2);
            ENSURE(asl::round(-2.0)  == -2);

        }

        void testShiftToRange() {
            ENSURE(shiftToRange(PI) == PI);
            ENSURE(shiftToRange(0) == 0);
            ENSURE(shiftToRange(-PI) == -PI);
            ENSURE(shiftToRange(2*PI) == 0);
            ENSURE(shiftToRange(3*PI) == PI);
            ENSURE(shiftToRange(-3*PI) == -PI);
            ENSURE(shiftToRange(PI, 0, 2 * PI) == PI);
            ENSURE(shiftToRange(3 * PI, 0, 2 * PI) == PI);
            ENSURE(shiftToRange(- PI_2, 0, 2 * PI) == 3 * PI_2);
        }

        void testPowerOfTwo() {
            ENSURE(powerOfTwo(1));
            ENSURE(powerOfTwo(2));
            ENSURE(powerOfTwo(4));
            ENSURE(powerOfTwo(8));
            ENSURE(powerOfTwo(16));
            ENSURE(powerOfTwo(256));
            ENSURE(powerOfTwo(65536));
            ENSURE(!powerOfTwo(65537));
            ENSURE(!powerOfTwo(3));
            ENSURE(!powerOfTwo(5));
            ENSURE(!powerOfTwo(6));
            ENSURE(!powerOfTwo(7));
            ENSURE(!powerOfTwo(9));
            ENSURE(!powerOfTwo(10));

            ENSURE( nextPowerOfTwo(0) == 1);
            ENSURE( nextPowerOfTwo(1) == 1);
            ENSURE( nextPowerOfTwo(2) == 2);
            ENSURE( nextPowerOfTwo(3) == 4);
            ENSURE( nextPowerOfTwo(255) == 256);
            ENSURE( nextPowerOfTwo(256) == 256);
            ENSURE( nextPowerOfTwo(257) == 512);

            {
                ENSURE(degFromRad(PI) == 180.0);
                ENSURE(radFromDeg(180.0) == PI);

                ENSURE(almostEqual(degFromRad(radFromDeg(45.45)),45.45));
            }
        }

        void testInterpolate() {
            bool myTrue  = true;
            bool myFalse = false;
            ENSURE(interpolate(1, 2, 0.1) == 1);
            ENSURE(interpolate(1, 2, 0.6) == 2);
            ENSURE(interpolate(0, 100, 0.1) == 10);
            ENSURE(interpolate(0, 100, 0.6) == 60);
            ENSURE(interpolate(myTrue, myFalse, 0) == myTrue);
            ENSURE(interpolate(myTrue, myFalse, 1) == myFalse);
            ENSURE(interpolate(myTrue, myFalse, 0.49) == myTrue);
            ENSURE(interpolate(myTrue, myFalse, 0.51) == myFalse);
            ENSURE(almostEqual(interpolate(2.5f, 3.5f, 0.5), 3.0f));
            ENSURE(almostEqual(interpolate(2.5f, 3.5f, 0.0), 2.5f));
            ENSURE(almostEqual(interpolate(2.5f, 3.5f, 1.0), 3.5f));
            ENSURE(almostEqual(interpolate(2.5, 3.5, 0.5), 3.0));
            ENSURE(almostEqual(interpolate(2.5, 3.5, 0.0), 2.5));
            ENSURE(almostEqual(interpolate(2.5, 3.5, 1.0), 3.5));
            // Non-const versions
            float myX = 1.0f;
            float myY = 2.0f;
            ENSURE(almostEqual(interpolate(myX, myY, 0.5), 1.5f));
            int myXi = 10;
            int myYi = 20;
            ENSURE(almostEqual(interpolate(myXi, myYi, 0.5), 15));
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new numeric_functions_UnitTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
