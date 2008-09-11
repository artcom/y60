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
//    $RCSfile: testFixedVector.tst.cpp,v $
//
//   $Revision: 1.8 $
//
// Description: unit test template file - change FixedVector to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "FixedVector.h"
#include "linearAlgebra.h"
#include "numeric_functions.h"

#include <asl/base/UnitTest.h>

#include <string>
#include <iostream>

using namespace std;
using namespace asl;

template <class T>
class FixedVectorTemplateUnitTest : public TemplateUnitTest {
public:
        typedef asl::FixedVector<2,T> Vec2T;
        Vec2T Vec2(T a, T b) {
            Vec2T result;
            result[0] = a;
            result[1] = b;
            return result;
        }
    FixedVectorTemplateUnitTest(const char * theTemplateArgument)
        : TemplateUnitTest("FixedVectorTemplateUnitTest",theTemplateArgument) {}

    void run() {
        {
            Vec2T myVec(Vec2(2,3));
            ENSURE(myVec[0]==2);
            ENSURE(myVec[1]==3);
            ENSURE(myVec.size()==2);

            Vec2T myOtherVec = myVec;
            ENSURE(myOtherVec[0]==2);
            ENSURE(myOtherVec[1]==3);

            ENSURE(*myVec.begin() == 2);
            ENSURE(*(myVec.begin()+1) == 3);
            ENSURE(myVec.end() - myVec.begin() == 2);
        }
       {
            const Vec2T myVec(Vec2(2,3));
            ENSURE(myVec[0]==2);
            ENSURE(myVec[1]==3);
            ENSURE(myVec.size()==2);

            const Vec2T myOtherVec = myVec;
            ENSURE(myOtherVec[0]==2);
            ENSURE(myOtherVec[1]==3);

            ENSURE(*myVec.begin() == 2);
            ENSURE(*(myVec.begin()+1) == 3);
            ENSURE(myVec.end() - myVec.begin() == 2);
        }
        Vec2T myVec = Vec2(2,3);
        myVec.sub(myVec);
        ENSURE(asl::almostEqual(myVec, Vec2(0,0)));
        myVec.add(Vec2(1,2));
        ENSURE(asl::almostEqual(myVec, Vec2(1,2)));
        myVec.add(Vec2(3,4));
        ENSURE(asl::almostEqual(myVec, Vec2(4,6)));
        myVec.div(2);
        ENSURE(asl::almostEqual(myVec, Vec2(2,3)));
        myVec.mult(3);
        ENSURE(asl::almostEqual(myVec, Vec2(6,9)));
        myVec.mult(Vec2(3,4));
        ENSURE(asl::almostEqual(myVec, Vec2(18,36)));
        myVec.div(Vec2(3,4));
        ENSURE(asl::almostEqual(myVec, Vec2(6,9)));
        myVec.negate();
        ENSURE(asl::almostEqual(myVec, Vec2(-6,-9)));
        myVec.add(12);
        ENSURE(asl::almostEqual(myVec, Vec2(6,3)));
        myVec.sub(2);
        ENSURE(asl::almostEqual(myVec, Vec2(4,1)));

        // Test almost equal
        {
            asl::FixedVector<3, double> myVector1;
            asl::FixedVector<3, double> myVector2;
            myVector1[0] = myVector1[1] = myVector1[2] = 1;
            myVector2[0] = myVector2[1] = myVector2[2] = 1 + 1e-6;
            ENSURE(asl::almostEqual(myVector1, myVector2, 1e-5));
            ENSURE(!asl::almostEqual(myVector1, myVector2, 1e-7));
            myVector1[0] = myVector1[1] = myVector1[2] = 0;
            myVector2[0] = myVector2[1] = myVector2[2] = 1e-6;
            ENSURE(asl::almostEqual(myVector1, myVector2, 1e-5));
            ENSURE(!asl::almostEqual(myVector1, myVector2, 1e-7));
            myVector1[0] = myVector1[1] = myVector1[2] = 1e10;
            myVector2[0] = myVector2[1] = myVector2[2] = 1e10 +1;
            ENSURE(asl::almostEqual(myVector1, myVector2, 1e-9));
            ENSURE(!asl::almostEqual(myVector1, myVector2, 1e-11));

            myVector1[0] = myVector1[1] = myVector1[2] = 0;
            myVector2[0] = myVector2[1] = myVector2[2] = -1.7484e-7;
            ENSURE(asl::almostEqual(myVector1, myVector2));
        }
        // Test getLength and getSquaredLength
        {
            typedef asl::FixedVector<3, double> Vec3d;
            Vec3d UnitX;
            UnitX[0] = 1.0;
            UnitX[1] = 0.0;
            UnitX[2] = 0.0;
            ENSURE(almostEqual(UnitX.getSquaredLength(), 1.0));
            ENSURE(almostEqual(UnitX.getLength(), 1.0));

            Vec3d v1;
            v1[0] = 3.0;
            v1[1] = 4.0;
            v1[2] = 5.0;
            ENSURE(almostEqual(v1.getSquaredLength(), 50.0));
            ENSURE(almostEqual(v1.getLength(), sqrt(50.0)));
        }
        {
            // projection test
            Vector3d myExpectedResult;
            Vector3d myVec1(1.0, 0.0, 0.0);
            Vector3d myVec2(1.0, 0.0, 0.0);

            myExpectedResult = Vector3d(1.0, 0.0, 0.0);
            ENSURE(almostEqual(projection(myVec1, myVec2), myExpectedResult));

            myVec1 = Vector3d(0.0, 1.0, 0.0);
            myExpectedResult = Vector3d(0.0, 0.0, 0.0);
            ENSURE(almostEqual(projection(myVec1, myVec2), myExpectedResult));

            myVec1 = Vector3d(2.0, 0.0, 0.0);
            myExpectedResult = Vector3d(2.0, 0.0, 0.0);
            ENSURE(almostEqual(projection(myVec1, myVec2), myExpectedResult));

            myVec1 = Vector3d(1.0, 1.0, 1.0);
            myExpectedResult = Vector3d(1.0, 0.0, 0.0);
            ENSURE(almostEqual(projection(myVec1, myVec2), myExpectedResult));

            myVec1 = Vector3d(2.0, 0.0, 0.0);
            myVec2 = Vector3d(2.0, 2.0, 2.0);
            myExpectedResult = Vector3d(2./3, 2./3, 2./3);
            cerr << projection(myVec1, myVec2) << endl;
            ENSURE(almostEqual(projection(myVec1, myVec2), myExpectedResult));
        }
    }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new FixedVectorTemplateUnitTest<float>("<float>"));
        addTest(new FixedVectorTemplateUnitTest<double>("<double>"));
        // DS: doesn't work with int because of fabs() etc ...
        //addTest(new FixedVectorTemplateUnitTest<int>("<int>"));
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
