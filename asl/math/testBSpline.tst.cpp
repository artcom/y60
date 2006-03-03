//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "BSpline.h"
#include "numeric_functions.h"

#include <asl/UnitTest.h>
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

        DPRINT(myBSpline.getArcLength());
        ENSURE(myBSpline.getArcLength() > 0);

        Vector3<T> myMidPoint(0.5,0,0);
        myBSpline.setControlPoints(myStart, sum(myStart, myMidPoint),
                                   myEnd, difference(myEnd, myMidPoint));

        DPRINT(myBSpline.evaluate(0.5));
        ENSURE(almostEqual(myBSpline.evaluate(0.5), Vector3<T>(1,0,0)) == true);

        DPRINT(myBSpline.getArcLength());
        ENSURE(myBSpline.getArcLength() == 2);

        std::vector< Vector3<T> > myResult = myBSpline.calculate(4);
        ENSURE(myResult.size() == 5);
        ENSURE(myResult[0] == myStart);
        DPRINT(myResult[1]);
        ENSURE(myResult[2] == Vector3<T>(1,0,0));
        DPRINT(myResult[3]);
        ENSURE(myResult[4] == myEnd);
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
