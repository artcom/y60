/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSnumeric: test_linearAlgebra.tst.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: unit test for linear algebra
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Spline.h"
#include "Vector234.h"
#include "CoordSpline.h"
#include "numeric_functions.h"

#include <asl/UnitTest.h>
#include <iostream>

using namespace std;
using namespace asl;

class CoordSplineTest : public UnitTest {
    public:
        CoordSplineTest() : UnitTest("CoordSplineTest") {  }
        void run() {
            testHermiteCoordSpline(5);
            ENSURE(true);
        }
        void testHermiteCoordSpline(int theNumSteps) {
            asl::Vector3f myPos0(0.0f,0.0f,0.0f);
            asl::Quaternionf myOrien(0.0f,0.0f,0.0f,1.0f);
            CoordSpline myCoordSpline;
            QuaternionKeyframe myKeyframe1(myPos0, myOrien, 0.0f, 1.0f);
            QuaternionKeyframe myKeyframe2(myPos0, myOrien, 0.7f, 1.0f);
            QuaternionKeyframe myKeyframe3(myPos0, myOrien, 0.3f, 1.0f);
            vector<QuaternionKeyframe> myFrames;
            myFrames.push_back(myKeyframe1);
            myFrames.push_back(myKeyframe2);
            myFrames.push_back(myKeyframe3);
            float myLength = 0;
            myCoordSpline.init(myFrames, myLength, false);
            cout << "Coord Spline length: " << myLength << endl;
            for (int i = 0; i <= theNumSteps; i ++) {
                float myT = (1.0 / theNumSteps) * i;
                cout << "CoordSpline Value for (" << myT << " ," << myCoordSpline.xyz(myT) << " / " << myCoordSpline.getHPR(myT) << ")" << endl;
            }


            myCoordSpline.print();
        }
};

class SplineTest : public UnitTest {
    public:
        SplineTest() : UnitTest("SplineTest") {  }
        void run() {
            testHermiteSpline(10);
            ENSURE(true);
        }
        void testHermiteSpline(int theNumSteps) {
            {
                Hermite mySpline;
                dvector x; x.push_back(0.0); x.push_back(2.0); x.push_back(3.0);
                dvector y; y.push_back(0.0); y.push_back(2.0);y.push_back(1.0);
                mySpline.init(x, y, x,y);
                cout << "x_first: " << mySpline.x_first() << endl;
                cout << "x_last: " << mySpline.x_last() << endl;
                cout << "y_first: " << mySpline.y_first() << endl;
                cout << "y_last: " << mySpline.y_last() << endl;
                for (int i = 0; i <= theNumSteps; i ++) {
                    float myT = (mySpline.x_last() / theNumSteps) * i;
                    cout << "Spline Value for (" << myT << " ," << mySpline(myT) << ")" << endl;
                }
                mySpline.print();
            }
        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); 
        addTest(new SplineTest);
        addTest(new CoordSplineTest);
 
    }
};

int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

	std::cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

