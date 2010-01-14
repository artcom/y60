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

#include "SvgPath.h"

#include <asl/base/UnitTest.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace asl;

class SvgPathTest : public UnitTest {
public:
    SvgPathTest() : UnitTest("SvgPath") {
    }

    void run() {

        {
            SvgPath myPath;
            myPath.move(Vector3f(0,10,0));
            myPath.line(Vector3f(0,20,0), true);
            ENSURE(myPath.getNumElements() == 1);
            ENSURE(myPath.getLength() == 20.0f);
        }

        {
            struct {
                char * definition;
                unsigned numElements;
            } myPaths[] = {
                { "M 0 0 l 0 50 l 10 50 l -10 50 l -50 0 z", 5 },
                { "M 0 0 h 100 v 100 Z", 3 },
                { "M 50 40 l 50 50 l 30 -20 Z", 3 },
            };

            for (unsigned i = 0; i < sizeof(myPaths) / sizeof(myPaths[0]); ++i) {
                SvgPath myPath(myPaths[i].definition);
                DPRINT(myPath.getNumElements());
                if (myPaths[i].numElements) {
                    ENSURE(myPath.getNumElements() == myPaths[i].numElements);
                }
            }
        }

        {
            SvgPath myPath("M0,0h100l50,50");
            ENSURE(almostEqual(myPath.getLength(), 170.71067f));

            DPRINT(myPath.nearest(Vector3f(42,2.3f,0)));
            ENSURE(almostEqual(myPath.nearest(Vector3f(42,2.3f,0)), Vector3f(42,0,0)));

            SvgPath::PathNormal myNormal = myPath.normal(Vector3f(130,20,0));
            ENSURE(almostEqual(myNormal.normal, normalized(Vector3f(-1,1,0))));
            ENSURE(almostEqual(myNormal.nearest, Vector3f(125,25,0)));

            SvgPath * myPerPath = myPath.createPerpendicularPath(Vector3f(23,0,0), 100.0f);
            ENSURE(myPerPath->getLength() == 100.0f);
            ENSURE(almostEqual(myPerPath->pointOnPath(0.0f), Vector3f(23,0,0)));
            ENSURE(almostEqual(myPerPath->pointOnPath(0.5f), Vector3f(23,50,0)));
            ENSURE(almostEqual(myPerPath->pointOnPath(1.0f), Vector3f(23,100,0)));

            SvgPath * mySubPath = myPath.createSubPath(Vector3f(10,0,0), Vector3f(90,0,0));
            DPRINT(mySubPath->getLength());
            ENSURE(mySubPath->getLength() == 80.0f);

            SvgPath myBezierPath;
            myBezierPath.move(Vector3f(0,0,0));
            myBezierPath.cbezier(Vector3f( 10, 10, 0), Vector3f( 90, 10, 0), Vector3f(100,0,0));
            DPRINT(myBezierPath.getLength());
            ENSURE(almostEqual(myBezierPath.getLength(), 102.275f));
            myBezierPath.cbezier(Vector3f(110,-10, 0), Vector3f(190,-10, 0), Vector3f(200,0,0));
            DPRINT(myBezierPath.getLength());
            ENSURE(almostEqual(myBezierPath.getLength(), 204.55f));

            SvgPath myBezierFromPointsPath;
            float myHandleLength = sqrt(10.0f * 10.0f + 10.0f * 10.0f);
            myBezierFromPointsPath.move(Vector3f(0,0,0));
            myBezierFromPointsPath.cbezierFromPoints(Vector3f(0,-50,0), Vector3f(100,0,0), Vector3f(200,50,0), Vector3f(300,150,0), myHandleLength);
            DPRINT(myBezierFromPointsPath.getLength());
            ENSURE(almostEqual(myBezierFromPointsPath.getLength(), 207.781f));
            myBezierFromPointsPath.cbezierFromPoints(Vector3f(100,0,0), Vector3f(200,50,0), Vector3f(300,150,0), Vector3f(400,100,0), myHandleLength);
            DPRINT(myBezierFromPointsPath.getLength());
            ENSURE(almostEqual(myBezierFromPointsPath.getLength(), 349.56f));


        }
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {
    }

    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new SvgPathTest());
    }
};

int main(int argc, char *argv[]) {
    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();
	std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << endl;
    return mySuite.returnStatus();
}
