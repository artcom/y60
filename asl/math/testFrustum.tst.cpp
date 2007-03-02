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
//    $RCSfile: testFrustum.tst.cpp,v $
//
//   $Revision: 1.1 $
//
// Description: unit test template file - change Scene to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include <asl/UnitTest.h>

#include "Frustum.h"

#include <string>
#include <iostream>

using namespace std; 
using namespace asl;

class FrustumUnitTest : public UnitTest {
public:
    FrustumUnitTest() 
        : UnitTest("FrustumUnitTest") {}

    void testCorners(Frustum& theFrustum) {
        Point3f LTF;
        Point3f RBF;
        Point3f RTF;
        Point3f LBF;
        Point3f LTBK;
        Point3f RBBK;
        Point3f RTBK;
        Point3f LBBK;

        Matrix4f myCameraMatrix;
        myCameraMatrix.makeIdentity();
        theFrustum.updatePlanes(myCameraMatrix, myCameraMatrix);
        theFrustum.getCorners(LTF, RBF, RTF, LBF, LTBK, RBBK, RTBK, LBBK);

        Matrix4f myProjection;
        theFrustum.getProjectionMatrix(myProjection);
        ENSURE(almostEqual(Point3f(-1,1,-1),  LTF * myProjection));
        ENSURE(almostEqual(Point3f(1,-1,-1),  RBF * myProjection));
        ENSURE(almostEqual(Point3f(1,1,-1),   RTF * myProjection));
        ENSURE(almostEqual(Point3f(-1,-1,-1), LBF * myProjection));
        ENSURE(almostEqual(Point3f(-1,1,1),  LTBK * myProjection));
        ENSURE(almostEqual(Point3f(1,-1,1),  RBBK * myProjection));
        ENSURE(almostEqual(Point3f(1,1,1),   RTBK * myProjection));
        ENSURE(almostEqual(Point3f(-1,-1,1), LBBK * myProjection));
    }
    
    void testPlane(const Plane<float>& thePlane, const Vector3f& theNormal, float theOffset, const std::string & theMsg) {
        ENSURE_MSG(almostEqual(thePlane.normal, theNormal), theMsg.c_str());
        ENSURE_MSG(almostEqual(thePlane.offset, theOffset), theMsg.c_str());
    }

    void testBoxIntersection(const Frustum & theFrustum, bool box1, bool box2, bool box3 , bool box4) {
        bool isOverlapping;
        asl::Box3f myBox1(Point3f(-4.5, -1, -4.5), Point3f(-4, 1, -4));
        ENSURE( box1 == intersection(myBox1, theFrustum, isOverlapping));

        asl::Box3f myBox2(Point3f(4, -1, 4), Point3f(4.5, 1, 4.5));
        ENSURE( box2 == intersection(myBox2, theFrustum, isOverlapping));

        asl::Box3f myBox3(Point3f(11, 11.5, 9), Point3f(12, 12, 10));
        ENSURE( box3 == intersection(myBox3, theFrustum, isOverlapping));
        
        asl::Box3f myBox4(Point3f(0.5, -1, 4), Point3f(1, 1, 4.5));
        ENSURE( box4 == intersection(myBox4, theFrustum, isOverlapping));

    }

    void testPlanes(Frustum& theFrustum) {
        Matrix4f myCameraMatrix;
        myCameraMatrix.makeIdentity();
        theFrustum.updatePlanes(myCameraMatrix, myCameraMatrix);

        static const float myHS2 = float(sqrt(2.0)/2);
        Plane<float> myPlane = theFrustum.getLeftPlane();
        testPlane(myPlane, Vector3f(myHS2,0,-myHS2), 0, "identity left");
        myPlane = theFrustum.getRightPlane();
        testPlane(myPlane, Vector3f(-myHS2,0,-myHS2), 0, "identity right");
        myPlane = theFrustum.getTopPlane();
        testPlane(myPlane, Vector3f(0,-myHS2,-myHS2), 0, "identity top");
        myPlane = theFrustum.getBottomPlane();
        testPlane(myPlane, Vector3f(0,myHS2,-myHS2), 0, "identity bottom");
        myPlane = theFrustum.getNearPlane();
        testPlane(myPlane, Vector3f(0,0,-1), -1, "identity near");
        myPlane = theFrustum.getFarPlane();
        testPlane(myPlane, Vector3f(0,0,1), 5, "identity far");

        testBoxIntersection(theFrustum, true, false, false, false);

        myCameraMatrix.makeYRotating( - float(asl::PI/2));
        Matrix4f myInvertedCameraMatrix (myCameraMatrix);
        myInvertedCameraMatrix.invert();
        theFrustum.updatePlanes(myCameraMatrix, myInvertedCameraMatrix);
        myPlane = theFrustum.getLeftPlane();
        DPRINT(myPlane.normal);
        testPlane(myPlane, Vector3f(myHS2,0,myHS2), 0, "rotating left");
        myPlane = theFrustum.getRightPlane();
        testPlane(myPlane, Vector3f(myHS2,0,-myHS2), 0, "rotating right");
        myPlane = theFrustum.getTopPlane();
        testPlane(myPlane, Vector3f(myHS2,-myHS2,0), 0, "rotating top");
        myPlane = theFrustum.getBottomPlane();
        testPlane(myPlane, Vector3f(myHS2,myHS2,0), 0, "rotating bottom");
        myPlane = theFrustum.getNearPlane();
        testPlane(myPlane, Vector3f(1,0,0), -1, "rotating near");
        myPlane = theFrustum.getFarPlane();
        testPlane(myPlane, Vector3f(-1,0,0), 5, "rotating far");

        testBoxIntersection(theFrustum, false, true, false, false);

        myCameraMatrix.makeTranslating(Vector3f(12, 12, 12));
        myInvertedCameraMatrix = myCameraMatrix;
        myInvertedCameraMatrix.invert();
        theFrustum.updatePlanes(myCameraMatrix, myInvertedCameraMatrix);

        static const float myS2 = float(sqrt(2.0));
        myPlane = theFrustum.getLeftPlane();
        testPlane(myPlane, Vector3f(myHS2,0,-myHS2), 0, "translating left");
        myPlane = theFrustum.getRightPlane();
        testPlane(myPlane, Vector3f(-myHS2,0,-myHS2), 12 * myS2, "translating right");
        myPlane = theFrustum.getTopPlane();
        testPlane(myPlane, Vector3f(0,-myHS2,-myHS2), 12 * myS2, "translating top");
        myPlane = theFrustum.getBottomPlane();
        testPlane(myPlane, Vector3f(0,myHS2,-myHS2), 0, "translating bottom");
        myPlane = theFrustum.getNearPlane();
        testPlane(myPlane, Vector3f(0,0,-1), 11, "translating near");
        myPlane = theFrustum.getFarPlane();
        testPlane(myPlane, Vector3f(0,0,1), -7, "translating far");

        testBoxIntersection(theFrustum, false, false, true, false);
        
        myCameraMatrix.makeYRotating(float(asl::PI / 2));
        myCameraMatrix.translate(Vector3f(5, 0, 0));
        myInvertedCameraMatrix = myCameraMatrix;
        myInvertedCameraMatrix.invert();
        theFrustum.updatePlanes(myCameraMatrix, myInvertedCameraMatrix);

        myPlane = theFrustum.getLeftPlane();
        DPRINT(myPlane.normal);
        DPRINT(myPlane.offset);
        testPlane(myPlane, Vector3f(-myHS2,0,- myHS2), 5*myHS2, "tr/rot left");
        myPlane = theFrustum.getRightPlane();
        DPRINT(myPlane.normal);
        DPRINT(myPlane.offset);
        testPlane(myPlane, Vector3f(-myHS2,0,myHS2), 5 * myHS2, "tr/rot right");
        myPlane = theFrustum.getTopPlane();
        DPRINT(myPlane.normal);
        DPRINT(myPlane.offset);
        testPlane(myPlane, Vector3f(-myHS2,-myHS2,0), 5 * myHS2, "tr/rot top");
        myPlane = theFrustum.getBottomPlane();
        DPRINT(myPlane.normal);
        DPRINT(myPlane.offset);
        testPlane(myPlane, Vector3f(-myHS2,myHS2,0), 5*myHS2, "tr/rot bottom");
        myPlane = theFrustum.getNearPlane();
        testPlane(myPlane, Vector3f(-1,0,0), 4, "tr/rot near");
        myPlane = theFrustum.getFarPlane();
        testPlane(myPlane, Vector3f(1,0,0), 0, "tr/rot far");

        testBoxIntersection(theFrustum, false, false, false, true);
    }

    void testAsymmetricFrustum(Frustum theFrustum) {
        ENSURE( theFrustum.getHShift() == 0);
        float myOldHeight = theFrustum.getHeight();
        ENSURE( theFrustum.getVShift() == 0);
        theFrustum.setVShift( 0.5 );
        ENSURE( theFrustum.getHeight() == myOldHeight );
        theFrustum.setHShift( 0.75 );
        ENSURE( theFrustum.getVShift() == 0.5);
        ENSURE( theFrustum.getHShift() == 0.75);
    }
       
    void run() {
        Frustum myFrustum(-1,1,-1,1,1,5);
        asl::Matrix4f myProjection;
        myFrustum.getProjectionMatrix(myProjection);
        ENSURE (Point3f(0,0,-1) == Point3f(0,0,-1)*myProjection);
        ENSURE (Point3f(0,0,1) == Point3f(0,0,-5)*myProjection);
        ENSURE (almostEqual(Point3f(1,1,-1), Point3f(1,1,-1)*myProjection));
        ENSURE (almostEqual(Point3f(-0.2f, -0.2f, 1.0f), Point3f(-1.0f,-1.0f,-5.0f)*myProjection));


        testPlanes(myFrustum);
        testCorners(myFrustum);

        testAsymmetricFrustum( myFrustum );
        Frustum myFrustum2(-0.5, 0.5, -0.5, 0.5, 1, 5);
        testAsymmetricFrustum( myFrustum2 );

        Frustum myAsymFrustum( -0.5, 0.5, -1.0, 0.0, 1, 5);
        ENSURE( myAsymFrustum.getVShift() == 0.5);

        myAsymFrustum.setHeight( 5.0 );
        ENSURE( myAsymFrustum.getHeight() == 5.0);
        ENSURE( myAsymFrustum.getVShift() == 0.5);

        myAsymFrustum.setVShift( 1.0 );
        ENSURE( myAsymFrustum.getHeight() == 5.0 );
        ENSURE( myAsymFrustum.getVShift() == 1.0 );


        myAsymFrustum.setVShift( 1.0 );
        ENSURE( myAsymFrustum.getVShift() == 1.0);

        myAsymFrustum.setHeight(25.0);
        myAsymFrustum.setVShift( 10.0 );
        ENSURE( myAsymFrustum.getVShift() == 10.0);
        ENSURE( myAsymFrustum.getHeight() == 25.0);
        ENSURE( myAsymFrustum.getVShift() == 10.0);
        myAsymFrustum.setHeight(50.0);
        ENSURE( myAsymFrustum.getHeight() == 50.0);
        ENSURE( myAsymFrustum.getVShift() == 10.0);

        myAsymFrustum.setVShift( 0 );
        testAsymmetricFrustum( myAsymFrustum );
        
    }
    
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new FrustumUnitTest());
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
