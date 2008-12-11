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

#include "BSpline.h"
#include "numeric_functions.h"

#include <asl/base/UnitTest.h>
#include <iostream>

using namespace std;
using namespace asl;

template <class T>
class BSplineTest : public TemplateUnitTest {
public:
    BSplineTest(const char * theTemplateArgument)
        : TemplateUnitTest("BSpline", theTemplateArgument)
    {
    }

    void run() {
        Vector3<T> myStart(0,0,0);
        Vector3<T> myEnd(2,0,0);

        BSpline<T> myBSpline(myStart, Vector3<T>(0,1,0),
                             myEnd, Vector3<T>(2,1,0));
        ENSURE(myBSpline.evaluate(-1) == myStart);
        ENSURE(myBSpline.evaluate(0) == myStart);
        ENSURE(myBSpline.evaluate(1) == myEnd);
        ENSURE(myBSpline.evaluate(2) == myEnd);

        myBSpline.calculate(10);
        DPRINT(myBSpline.getArcLength());
        ENSURE(myBSpline.getArcLength() > 0);

        Vector3<T> myMidPoint(0.5,0,0);
        myBSpline.setControlPoints(myStart, sum(myStart, myMidPoint),
                                   myEnd, difference(myEnd, myMidPoint));

        DPRINT(myBSpline.evaluate(0.5));
        ENSURE(almostEqual(myBSpline.evaluate(0.5), Vector3<T>(1,0,0)) == true);

        myBSpline.calculate(10);
        DPRINT(myBSpline.getArcLength());
        ENSURE(myBSpline.getArcLength() == 2);

        std::vector< Vector3<T> > myResult = myBSpline.calculate(4);
        ENSURE(myResult.size() == 5);
        ENSURE(myResult[0] == myStart);
        DPRINT(myResult[1]);
        ENSURE(myResult[2] == Vector3<T>(1,0,0));
        DPRINT(myResult[3]);
        ENSURE(myResult[4] == myEnd);
        
        // Test getPosition
        {
            Vector3<T> myStart(0,0,0);
            Vector3<T> myEnd(2,0,0);
    
            BSpline<T> myBSpline(myStart, Vector3<T>(1,2,0),
                                 myEnd, Vector3<T>(1,2,0));
            
            ENSURE_EXCEPTION(myBSpline.getArcLength(), asl::Exception);
            ENSURE_EXCEPTION(myBSpline.getPosition(1), asl::Exception);
            myBSpline.calculate(2);
            Vector3<T> myMidPoint = myBSpline.getResult()[1];
            T mySegmentLength = T(sqrt(3.25));
            ENSURE(almostEqual(myBSpline.getArcLength(), 2 * mySegmentLength));                                 
            ENSURE(almostEqual(myBSpline.getPosition(0), myStart));
            ENSURE(almostEqual(myBSpline.getPosition(100), myEnd));
            ENSURE(almostEqual(myBSpline.getPosition(mySegmentLength), myMidPoint));
            ENSURE(almostEqual(myBSpline.getPosition(mySegmentLength / 2), myMidPoint / 2));
            ENSURE(almostEqual(myBSpline.getPosition(mySegmentLength * T(1.5)), (myEnd + myMidPoint) / 2));
        }
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {
    }

    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new BSplineTest<float>("<float>"));
        addTest(new BSplineTest<double>("<double>"));
    }
};

int main(int argc, char *argv[]) {
    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();
	std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << endl;
    return mySuite.returnStatus();
}
