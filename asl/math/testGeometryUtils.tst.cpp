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
//    $RCSnumeric: test_linearAlgebra.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test for linear algebra
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "numeric_functions.h"

#include "linearAlgebra.h"
#include "GeometryUtils.h"

#include <asl/base/UnitTest.h>
#include <iostream>

using namespace std;
using namespace asl;




class GeometryUtilsUnitTest : public UnitTest {

    static void addTriangle(vector<Point3f> & theTriangles,
                            const Point3f &a, const Point3f &b, const Point3f &c)
    {
        theTriangles.push_back(a);
        theTriangles.push_back(b);
        theTriangles.push_back(c);
    }

    public:
        GeometryUtilsUnitTest() : UnitTest("GeometryUtilsUnitTest") {  }
        void run() {

			// test facenormal generation
			Vector3f myResult = generateFaceNormal(Vector3f(1,0,-1), Vector3f(0,1,1), Vector3f(0,0,0));
            ENSURE_MSG(almostEqual(myResult, Vector3f(-0.57735f,0.57735f,-0.57735f)), "generateFaceNormal.");

			Vector3f myResult1 = generateFaceNormal(Vector3f(1,0,0), Vector3f(0,1,0), Vector3f(-1,0,0));
			ENSURE_MSG(almostEqual(myResult1, Vector3f(0,0,-1)), "generateFaceNormal.");

			// test vertexnormal generation
			VertexNormalBuilder<float> myBuilder;
			Vector3f myOrigin(0,1,0);
			long myOriginIndex = myBuilder.addVertex(myOrigin) - 1;
			long myVertex1 = myBuilder.addVertex(Vector3f(1,0,-1)) - 1;
			long myVertex2 = myBuilder.addVertex(Vector3f(0,0,-1)) - 1;
			long myVertex3 = myBuilder.addVertex(Vector3f(1,0,0)) - 1;
			long myVertex4 = myBuilder.addVertex(Vector3f(1,0,1)) - 1;
			long myVertex5 = myBuilder.addVertex(Vector3f(0,0,1)) - 1;
			long myVertex6 = myBuilder.addVertex(Vector3f(-1,0,1)) - 1;
			long myVertex7 = myBuilder.addVertex(Vector3f(-1,0,0)) - 1;
			long myVertex8 = myBuilder.addVertex(Vector3f(-1,0,-1)) - 1;

			myBuilder.addFace(myOriginIndex, myVertex1, myVertex2); // poly 1
			myBuilder.addFace(myOriginIndex, myVertex3, myVertex1); // poly 2
			myBuilder.addFace(myOriginIndex, myVertex4, myVertex3); // poly 3
			myBuilder.addFace(myOriginIndex, myVertex5, myVertex4); // poly 4
			myBuilder.addFace(myOriginIndex, myVertex6, myVertex5); // poly 5
			myBuilder.addFace(myOriginIndex, myVertex7, myVertex6); // poly 6
			myBuilder.addFace(myOriginIndex, myVertex8, myVertex7); // poly 7
			myBuilder.addFace(myOriginIndex, myVertex2, myVertex8); // poly 8

			Vector3f myVertexNormal1 = myBuilder.getVertexNormal(Vector3f(0.,1.,0.), myOriginIndex, radFromDeg(89.0));
            ENSURE_MSG(almostEqual(myVertexNormal1, Vector3f(0,1,0 )), "generate Vertex Normals.");


            //test polyhedra volume
            {
                vector<Point3f> myTetrahedron; //its irregular, but its easy to define

                addTriangle(myTetrahedron,Point3f(0,1,0),Point3f(1,0,0),Point3f(0,0,0));
                addTriangle(myTetrahedron,Point3f(0,1,0),Point3f(0,0,1),Point3f(1,0,0));
                addTriangle(myTetrahedron,Point3f(0,1,0),Point3f(0,0,0),Point3f(0,0,1));
                addTriangle(myTetrahedron,Point3f(0,0,0),Point3f(1,0,0),Point3f(0,0,1));
                double myVolume = calculatePolyhedraVolume(myTetrahedron);

                //the volume of any pyramid is 1/3 * area_base * height...
                //our tetrahedron can be seen as a pyramid with its base given by the first triangle
                double myRealVolume = 1.0/3.0 * 1.0/2.0 * 1;
                // cerr << myVolume << " - " << myRealVolume << endl;
                ENSURE_MSG(almostEqual(myVolume, myRealVolume), "calculatePolyhedraVolume");
            }
            {
                vector<Point3f> myCube;

                //top
                addTriangle(myCube,Point3f(0,1,0),Point3f(0,1,1),Point3f(1,1,1));
                addTriangle(myCube,Point3f(0,1,0),Point3f(1,1,1),Point3f(1,1,0));

                //bottom
                addTriangle(myCube,Point3f(0,0,0),Point3f(1,0,0),Point3f(1,0,1));
                addTriangle(myCube,Point3f(0,0,0),Point3f(1,0,1),Point3f(0,0,1));

                //sides
                addTriangle(myCube,Point3f(1,1,0),Point3f(0,0,0),Point3f(0,1,0));
                addTriangle(myCube,Vector3f(1,1,0),Point3f(1,0,0),Point3f(0,0,0));

                addTriangle(myCube,Point3f(1,1,0),Point3f(1,1,1),Point3f(1,0,1));
                addTriangle(myCube,Point3f(1,1,0),Point3f(1,0,1),Point3f(1,0,0));

                addTriangle(myCube,Point3f(0,1,1),Point3f(0,0,1),Point3f(1,0,1));
                addTriangle(myCube,Point3f(0,1,1),Point3f(1,0,1),Point3f(1,1,1));

                addTriangle(myCube,Point3f(0,1,1),Point3f(0,1,0),Point3f(0,0,0));
                addTriangle(myCube,Point3f(0,1,1),Point3f(0,0,0),Point3f(0,0,1));


                double myVolume = calculatePolyhedraVolume(myCube);
                //double myRealVolume = 1;

                // cerr << myVolume << " - " << myRealVolume << endl;
                ENSURE_MSG(almostEqual(myVolume, 1), "calculatePolyhedraVolume");
            }
            // test Area
            {
                double myArea = computeFaceArea(
                    Vector3d(0,0,0),
                    Vector3d(1,0,0),
                    Vector3d(0,1,0));
                ENSURE_MSG(almostEqual(myArea, 0.5), "computeFaceArea");
            }
            {
                float myArea = computeFaceArea(
                    Vector3f(0,0,4),
                    Vector3f(1,0,4),
                    Vector3f(0,1,4));
                ENSURE_MSG(almostEqual(myArea, 0.5f), "computeFaceArea");
            }
        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new GeometryUtilsUnitTest);

    }
};

int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

	std::cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

