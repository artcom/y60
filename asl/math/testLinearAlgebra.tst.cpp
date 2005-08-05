/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: testLinearAlgebra.tst.cpp,v $
//
//   $Revision: 1.21 $
//
//   Description: unit test for linear algebra
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "numeric_functions.h"

#include "linearAlgebra.h"
#include "intersection.h"
#include "Point234.h"
#include "Line.h"
#include "Sphere.h"
#include "Plane.h"

#include <asl/UnitTest.h>
#include <cmath>
#include <iostream>

using namespace std;
using namespace asl;

class LinearAlgebraTestBase : public UnitTest {
	public:
		LinearAlgebraTestBase(const char * myName) : UnitTest(myName) {}

		template<class T, class Number>
        void testFixedVectorBase(T & theProband, Number theTypeHint,
                                 int theSize, const string & theProbandName)
        {
			// size test
			ENSURE_MSG(theProband.size() == theSize, string(theProbandName + "'s size.").c_str() );

			// operator [] and == tests
			for (int i = 0; i < theSize; i++ ) {
				Number myValue = Number(5);
				theProband[i] = myValue;
				ENSURE_MSG(theProband[i] == myValue, string(theProbandName + "'s [] access and equal operator.").c_str() );
			}

			// iterator interface test
			typename TripleOf<Number>::iterator theStart = theProband.begin();
			typename TripleOf<Number>::iterator theEnd   = theProband.end();
			int theCounter = 0;
			for (; theStart != theEnd; theStart++) {
				ENSURE_MSG(*theStart == theProband[theCounter++], string(theProbandName + "'s iterators.").c_str() );
			}


		}

		template<class T> void runTripleTest(const T & myT, const string & theProbandName) {
			T myFirstTriple(1.0, 2.0, 3.0);
			T mySecondTriple(-1.f, -2.f, -3.f);

            bool equal = (myFirstTriple == T(1.0, 2.0, 3.0));
			ENSURE_MSG(myFirstTriple == T(1.0, 2.0, 3.0), string(theProbandName + "'s equal operator.").c_str() );
			ENSURE_MSG((myFirstTriple[0] == 1.0) && (myFirstTriple[1] == 2.0) && (myFirstTriple[2] == 3.0) &&
					(myFirstTriple == T(1.0f, 2.0f, 3.0f)), string(theProbandName + "'s equal operator.").c_str() );

			T myResultTriple(myFirstTriple); // copy ctor
			ENSURE_MSG(myResultTriple == myFirstTriple, string(theProbandName + "'s copy ctor.").c_str() );

			myResultTriple.add( mySecondTriple ); // add TripleOf
			ENSURE_MSG( myResultTriple == T(0.f, 0.f, 0.f),
						string(theProbandName + "'s add a " + theProbandName + ".").c_str()  );

			myResultTriple.add( 1.5f ); // add number
			ENSURE_MSG(myResultTriple == T(1.5, 1.5, 1.5), string(theProbandName + "'s add a number.").c_str()  );

			myResultTriple.sub( 1.3f ); // sub number
			ENSURE_MSG( almostEqual(myResultTriple, T(0.2f, 0.2f, 0.2f)),
						string(theProbandName + "'s almostEqual.").c_str()  );
			ENSURE_MSG( !almostEqual(myResultTriple, T(0.2f, 0.2f, 0.2f), 1E-15),
						string(theProbandName + "'s almostEqual.").c_str()  );

			myResultTriple.sub( T(0.3f, 0.4f, 0.5f )); // sub TripleOf
			ENSURE_MSG( almostEqual(myResultTriple, T(-0.1f, -0.2f, -0.3f)),
						string(theProbandName + "'s sutract a " + theProbandName + ".").c_str() );

			myResultTriple.mult( -10.5f ); // mult number
			ENSURE_MSG( almostEqual(myResultTriple, T(1.05f, 2.1f, 3.15f)),
						string(theProbandName + "'s divide by a " + theProbandName + ".").c_str()  );

			myResultTriple.div( 1.05f ); // div number
			ENSURE_MSG(almostEqual(myResultTriple, TripleOf<double>(1, 2, 3)), string(theProbandName + "'s divide by a number.").c_str()  );

			myResultTriple.negate();
			ENSURE_MSG(almostEqual(myResultTriple, mySecondTriple), string(theProbandName + "'s negate.").c_str() );
		}

		bool floatIsEqual(const float f1, const float f2, double thePrecision = 1E-6) {
			return (fabs(f1 - f2)) <= (f1 * thePrecision);
		}

		bool floatIsEqual(const double f1, const double f2, double thePrecision = 1E-6) {
			return (fabs(f1 - f2)) <= (f1 * thePrecision);
		}
};

class VectorUnitTest : public LinearAlgebraTestBase {
    public:
        VectorUnitTest() : LinearAlgebraTestBase("VectorUnitTest") {  }
        void run() {
			{ // TripleOf
    			TripleOf<double> myTriple;
                double myTypeHint = 0;
    			testFixedVectorBase( myTriple, myTypeHint, 3, "TripleOf" );
				runTripleTest(myTriple, "TripleOf" );
			}

			{ // Point3
			    Point3d myPoint3(1.0,2.0,3.0);
                double myTypeHint = 0;
			    testFixedVectorBase( myPoint3, myTypeHint, 3, "Point3");
			    runTripleTest(myPoint3, "Point3");
			    myPoint3 = Point3d(1.0,2.0,3.0);
			    Vector3d myPoint3asVector3d = asVector(myPoint3);
			    ENSURE_MSG(myPoint3asVector3d == Vector3d(1,2,3), "Convert a Point3 to a Vector3d");
			    myPoint3asVector3d.negate();
			    myPoint3 = asPoint(myPoint3asVector3d);
			    ENSURE_MSG(myPoint3 == Point3d(-1,-2,-3), "Convert the Vector3d back to a Point3");
			}

            {   // Vector2d
                Vector2d myVector1(1, 2);
                ENSURE(myVector1[0] == 1);
                ENSURE(myVector1[1] == 2);

                Vector2d myVector2(myVector1);
                ENSURE(myVector2[0] == 1);
                ENSURE(myVector2[1] == 2);

                double myArray[2] = {3, 4};
                Vector2d myVector3(myArray);
                ENSURE(myVector3[0] == 3);
                ENSURE(myVector3[1] == 4);

                myVector3 = myVector1;
                ENSURE(myVector1[0] == 1);
                ENSURE(myVector1[1] == 2);

                ENSURE(dot(myVector1, Vector2d(3.0, 4.0)) == 11);
                ENSURE(length(Vector2d(3.0, 4.0)) - 5.0 < 0.000001);

                // operator
                Vector2d myVec(1.0, 2.0);
                Vector2d myVec2(0.5, 1.0);
                myVec *= 2.0;
                ENSURE(myVec[0] == 2.0 && myVec[1] == 4.0);
                myVec /= 4.0;
                ENSURE(myVec[0] == 0.5 && myVec[1] == 1.0);
                myVec -= myVec2;
                ENSURE(myVec[0] == 0.0 && myVec[1] == 0.0);
                myVec += myVec2;
                ENSURE(myVec[0] == 0.5 && myVec[1] == 1.0);
                Vector2d expectedResult = myVec + myVec2;
                ENSURE(expectedResult[0] == 1.0 && expectedResult[1] == 2.0);
                expectedResult = myVec - myVec2;
                ENSURE(expectedResult[0] == 0.0 && expectedResult[1] == 0.0);
                expectedResult = myVec * 10.0;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0);
                expectedResult = 10.0 * myVec;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0);
                expectedResult = myVec * 10;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0);
                expectedResult = 10 * myVec;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0);
                expectedResult = myVec * 10.0f;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0);
                expectedResult = 10.0f * myVec;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0);
                expectedResult = -expectedResult;
                ENSURE(expectedResult[0] == -5.0 && expectedResult[1] == -10.0);
                expectedResult = myVec / 2.0;
                ENSURE(expectedResult[0] == 0.25 && expectedResult[1] == 0.5);

            }

			{   // Vector3d
			    Vector3d myVector3d;
                double myTypeHint = 0;
			    testFixedVectorBase( myVector3d, myTypeHint, 3, "Vector3d");
			    runTripleTest(myVector3d, "Vector3d");

/*			    Vector3d myPointSub = Point3d(1,2,3) - Point3d(-2, 0, 2);
			    ENSURE_MSG(myPointSub == Vector3d(3,2,1), "Subtract two Point3 = Vector3d");
*/
			    Vector3d myVectorSub = Vector3d(1,2,3) - Vector3d(-2, 0, 2);
			    ENSURE_MSG(myVectorSub == Vector3d(3,2,1), "Subtract two Vector3d = Vector3d");

			    Point3d myPointSub2 = Point3d(1,2,3) - Vector3d(-2, 0, 2);
			    ENSURE_MSG(myPointSub2 == Point3d(3,2,1), "Subtract a Vector3d from a Point3  = Point3");

			    Point3d myPointAdd = Point3d(1,2,3) + Vector3d(2, 0, -2);
			    ENSURE_MSG(myPointAdd == Point3d(3,2,1), "Add a Vector3d to a Point3  = Point3");

			    Vector3d myVectorAdd = Vector3d(1,2,3) + Vector3d(2, 0, -2);
			    ENSURE_MSG(myVectorAdd == Vector3d(3,2,1), "Add two Vector3d = Vector3d");

			    Vector3d myVectorMult = Vector3d(1,2,3) * 3.0;
			    ENSURE_MSG(myVectorMult == Vector3d(3,6,9), "Mult a Vector3d by a factor");

			    Vector3d myVectorMult2 = 3.0 * Vector3d(1,2,3);
			    ENSURE_MSG(myVectorMult2 == Vector3d(3,6,9), "Mult a factor with a Vector3d");

			    Vector3d myVectorDiv = myVectorMult2 / 6.;
			    ENSURE_MSG(myVectorDiv == Vector3d(0.5,1,1.5), "Divide a Vector3d by a divisor");

			    { // dot product tests
			        Vector3d myXaxis(1,0,0);
			        Vector3d myYaxis(0,1,0);
			        ENSURE_MSG(floatIsEqual(dot(myXaxis, myYaxis), 0.0), "The dot product of two perpendicular vectors is 0.0")

			        Vector3d myVec(1.4534f,0.3432f,0.327862f);
	    		    ENSURE_MSG( floatIsEqual(dot(myVec, myVec), (length(myVec) * length(myVec))),
		    	                "The dot product of a vector with itself produces the square ot its length.")

			        Vector3d myXYaxis(1,1,0);
	    		    ENSURE_MSG( floatIsEqual(cosAngle(myXaxis, myXYaxis), static_cast<double>(cos(PI_4))), "The angle between the x-axis and (1,1,0) 45 degree.")

					Vector3d myVector(static_cast<float>(cos(PI_4)), static_cast<double>(cos(PI_4)), 0.0);
					ENSURE_MSG(floatIsEqual(length(myVector), 1), "Vector3d length test");
					Vector3d myVector2(0.0, 0.0, 0.0);
					ENSURE_MSG(floatIsEqual(length(myVector2), 0), "Vector3d length test");
					myVector2 = normalized(myVector2);
					ENSURE_MSG(myVector2 == Vector3d(0,0,0), "Normalize a null vector");

					myVector3d = normalized(Vector3d(5.0, 50.0, -500.0));
					ENSURE_MSG(floatIsEqual(length(myVector3d), 1), "Normalize a vector");

			    }
				{ // cross and normals test
					Vector3d myResult = cross(Vector3d(0,0,1), Vector3d(1,0,0));
				    ENSURE_MSG(myResult == Vector3d(0,1,0), "Crossproduct of x- and z- axis.");

					Vector3d myResult1 = cross(Vector3d(0,0,-1), Vector3d(-1,0,0));
				    ENSURE_MSG(myResult1 == Vector3d(0,1,0), "Crossproduct of  -x- and -z- axis.");

					Vector3d myResult2 = cross(Vector3d(0,0,1), Vector3d(1,0,1));
				    ENSURE_MSG(myResult2 == Vector3d(0,1,0), "Crossproduct of (1,0,1) and z- axis.");

					Vector3d myResult3 = cross(Vector3d(1,0,-1), Vector3d(0,1,1));
				    ENSURE_MSG(myResult3 == Vector3d(1,-1,1), "Crossproduct of (1,0,-1) and (0,1,1).");

					Vector3d myResult4 = normal(Vector3d(0,0,10), Vector3d(0.3f,0,0));
					ENSURE_MSG(myResult4 == Vector3d(0,1,0), "normal of scaled x- and z- axis.");

					Vector3d myResult5 = normal(Vector3d(0,0,-3), Vector3d(-1100,0,0));
				    ENSURE_MSG(myResult5 == Vector3d(0,1,0), "normal of  scaled -x- and -z- axis.");

					Vector3d myResult6 = normal(Vector3d(0,0,10), Vector3d(100,0,1));
				    ENSURE_MSG(myResult6 == Vector3d(0,1,0), "normal of (100,0,1) and scaled z- axis.");

					Vector3d myResult7 = normal(Vector3d(1,0,-1), Vector3d(0,1,1));
				    ENSURE_MSG(almostEqual(myResult7, Vector3d(0.57735f,-0.57735f,0.57735f)), "normal of (1,0,-1) and (0,1,1).");

				}
                {
                    // operator
                    Vector3d myVec(1.0, 2.0, 3.0);
                    Vector3d myVec2(0.5, 1.0, 1.5);
                    myVec *= 2.0;
                    ENSURE(myVec[0] == 2.0 && myVec[1] == 4.0 && myVec[2] == 6.0);
                    myVec /= 4.0;
                    ENSURE(myVec[0] == 0.5 && myVec[1] == 1.0 && myVec[2] == 1.5);
                    myVec -= myVec2;
                    ENSURE(myVec[0] == 0.0 && myVec[1] == 0.0 && myVec[2] == 0.0);
                    myVec += myVec2;
                    ENSURE(myVec[0] == 0.5 && myVec[1] == 1.0 && myVec[2] == 1.5);
                    Vector3d expectedResult = myVec + myVec2;
                    ENSURE(expectedResult[0] == 1.0 && expectedResult[1] == 2.0 && expectedResult[2] == 3.0);
                    expectedResult = myVec - myVec2;
                    ENSURE(expectedResult[0] == 0.0 && expectedResult[1] == 0.0 && expectedResult[2] == 0.0);
                    expectedResult = myVec * 10.0;
                    ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 && expectedResult[2] == 15.0);
                    expectedResult = 10.0 * myVec;
                    ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 && expectedResult[2] == 15.0);
                    expectedResult = myVec * 10;
                    ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 && expectedResult[2] == 15.0);
                    expectedResult = 10 * myVec;
                    ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 && expectedResult[2] == 15.0);
                    expectedResult = myVec * 10.0f;
                    ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 && expectedResult[2] == 15.0);
                    expectedResult = 10.0f * myVec;
                    ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 && expectedResult[2] == 15.0);
                    expectedResult = -expectedResult;
                    ENSURE(expectedResult[0] == -5.0 && expectedResult[1] == -10.0 && expectedResult[2] == -15.0);
                    expectedResult = myVec / 2.0;
                    ENSURE(expectedResult[0] == 0.25 && expectedResult[1] == 0.5 && expectedResult[2] == 0.75);
                }
			}
   			{   // Vector4d
                // operator
                Vector4d myVec(1.0, 2.0, 3.0, 6.0);
                Vector4d myVec2(0.5, 1.0, 1.5, 3.0);
                myVec *= 2.0;
                ENSURE(myVec[0] == 2.0 && myVec[1] == 4.0 && myVec[2] == 6.0 && myVec[3] == 12.0);
                myVec /= 4.0;
                ENSURE(myVec[0] == 0.5 && myVec[1] == 1.0 && myVec[2] == 1.5 && myVec[3] == 3.0);
                myVec -= myVec2;
                ENSURE(myVec[0] == 0.0 && myVec[1] == 0.0 && myVec[2] == 0.0 && myVec[3] == 0.0);
                myVec += myVec2;
                ENSURE(myVec[0] == 0.5 && myVec[1] == 1.0 && myVec[2] == 1.5 && myVec[3] == 3.0);
                Vector4d expectedResult = myVec + myVec2;
                ENSURE(expectedResult[0] == 1.0 && expectedResult[1] == 2.0 &&
                    expectedResult[2] == 3.0 && expectedResult[3] == 6.0);
                expectedResult = myVec - myVec2;
                ENSURE(expectedResult[0] == 0.0 && expectedResult[1] == 0.0 &&
                    expectedResult[2] == 0.0 && expectedResult[3] == 0.0);
                expectedResult = myVec * 10.0;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 &&
                    expectedResult[2] == 15.0 && expectedResult[3] == 30.0);
                expectedResult = 10.0 * myVec;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 &&
                    expectedResult[2] == 15.0 && expectedResult[3] == 30.0);
                expectedResult = myVec * 10;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 &&
                    expectedResult[2] == 15.0 && expectedResult[3] == 30.0);
                expectedResult = 10 * myVec;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 &&
                    expectedResult[2] == 15.0 && expectedResult[3] == 30.0);
                expectedResult = myVec * 10.0f;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 &&
                    expectedResult[2] == 15.0 && expectedResult[3] == 30.0);
                expectedResult = 10.0f * myVec;
                ENSURE(expectedResult[0] == 5.0 && expectedResult[1] == 10.0 &&
                    expectedResult[2] == 15.0 && expectedResult[3] == 30.0);
                expectedResult = -expectedResult;
                ENSURE(expectedResult[0] == -5.0 && expectedResult[1] == -10.0 &&
                    expectedResult[2] == -15.0 && expectedResult[3] == -30.0);
                expectedResult = myVec / 2.0;
                ENSURE(expectedResult[0] == 0.25 && expectedResult[1] == 0.5 &&
                    expectedResult[2] == 0.75 && expectedResult[3] == 1.5);
            }

        }

// Parking slot
//std::cout <<"result : "<<myResult[0]<<" , "<< myResult[1] << " , "<<myResult[2]<<endl;

};

class LinearAlgebraUnitTest : public LinearAlgebraTestBase {
    public:
        LinearAlgebraUnitTest() : LinearAlgebraTestBase("LinearAlgebraUnitTest") {  }
        virtual ~LinearAlgebraUnitTest() {}
        void run() {
			// brainless types, not yet born
			PairOf<double> myPair;
            double myTypeHint = 0;
			testFixedVectorBase(myPair, myTypeHint, 2, "PairOf");

			QuadrupleOf<double> myQuadruple;
			testFixedVectorBase(myQuadruple, myTypeHint, 4, "QuadrupleOf");

			// Test Line
			{
				Line<double> myLine;
				Line<double> myOriginPointer(Point3d(-1,0,0), Vector3d(1,0,0));
				Line<double> myOriginPointer2(Point3d(-1,0,0), Point3d(0,0,0));

				Line<double>myLine2(myOriginPointer2);
				bool myLine2PositionComparision = almostEqual(myLine2.origin, myOriginPointer2.origin);
				bool myLine2TargetComparision   = almostEqual(myLine2.direction, myOriginPointer2.direction);
				ENSURE_MSG( myLine2PositionComparision && myLine2TargetComparision,
					        "Lines copy ctor");

				// both lines must be equal
				bool myLinePositionComparision = almostEqual(myOriginPointer.origin, myOriginPointer2.origin);
				bool myLineTargetComparision   = almostEqual(myOriginPointer.direction, myOriginPointer2.direction);
				ENSURE_MSG( myLinePositionComparision && myLineTargetComparision, "Line Construction");

			}

            testPlane();

			{ // Sphere
				Sphere<double> mySphere1;
				Sphere<double> mySphere2(Point3d(1,1,1), 10);
				ENSURE_MSG( (( almostEqual(mySphere2.center, Point3d(1,1,1))) &&
							   mySphere2.radius == 10.0), "Sphere ctor.")
				Sphere<double> mySphere3(mySphere2);
				ENSURE_MSG( (( almostEqual(mySphere3.center, mySphere2.center)) &&
							   mySphere3.radius == mySphere2.radius), "Sphere copy ctor.")

				Point3d myIntersectionP1, myIntersectionP2;
				Line<double>myPhaser(Point3d(1,0,1), Vector3d(0,1,0) );
				bool myIntersection = intersection(mySphere3, myPhaser, myIntersectionP1, myIntersectionP2);
				ENSURE_MSG( myIntersection, "Intersection Line <-> Sphere")
				ENSURE_MSG( almostEqual(myIntersectionP1, Point3d(1, 11, 1)), "1st intersection line sphere")
				ENSURE_MSG( almostEqual(myIntersectionP2, Point3d(1, -9, 1)), "2nd intersection line sphere")

				Line<double>myPhaser2(Point3d(1,20,1), Vector3d(1,0,0) );
				bool myIntersection2 = intersection(mySphere3, myPhaser2, myIntersectionP1, myIntersectionP2);
				ENSURE_MSG( !myIntersection2, "NonIntersection Line <-> Sphere")
		    }

			{ // nearest stuff
                Line<double> myLine(Point3d(0,1,0), Vector3d(1,0,0) );
                double myDistance = distance(Point3d(0,3,0), myLine);
                Point3d myNearest = nearest(Point3d(0,3,0), myLine);
                ENSURE(almostEqual(myNearest, Point3d(0,1,0)));
                DPRINT(myDistance);
                ENSURE(distance(Point3d(0,3,0), myLine) == distance(myLine, Point3d(0,3,0)));
                ENSURE(almostEqual(myDistance, 2.0));
			}
            //TODO:
			// check almost equal: different size, different types (valid and not valid)
			// check distances and nearest points


        }

    void
    testPlane() {
        Plane<double> myPlane;
        const Plane<double> myPlane2(Vector3d(0,1,0), 1.0);
        const Plane<double> myPlane3( myPlane2 );

        Plane<double> myPlane4( Vector3d(0,1,0), Point3<double>(10,-1,10));
        DPRINT(myPlane4.offset);
        DPRINT(myPlane2.offset);
        ENSURE_MSG((almostEqual(myPlane4.normal, myPlane2.normal) &&
                    almostEqual(myPlane4.offset, myPlane2.offset)),
                    "Testing normal and point on plane ctor");

        Plane<double> myPlane5(normalized(Vector3d(1,1,0)), -5);
        Plane<double> myPlane6(normalized(Vector3d(1,1,0)), Point3<double>(sqrt(50.0), 0, 0)); // 5 * 5 + 5 * 5
        ENSURE_MSG((almostEqual(myPlane5.normal, myPlane6.normal) &&
                    almostEqual(myPlane5.offset, myPlane6.offset)),
                    "Testing normal and point on plane ctor");


        ENSURE_MSG( ((almostEqual(myPlane3.normal, myPlane2.normal)) && myPlane3.offset == myPlane2.offset),
                "Planes plane copy ctor.")

            // test plane, vector projection
            Vector3d myProjectedVec = projection(Vector3d(1,1,1), myPlane2);
        ENSURE_MSG( almostEqual(myProjectedVec, Vector3d(1, 0, 1)), "Vector3 projection to a Plane.")

            Vector3d myProjectedVec2 = projection(Vector3d(0,1,0), myPlane2);
        ENSURE_MSG( almostEqual(myProjectedVec2, Vector3d(0, 0, 0)), "Vector3 projection to a Plane.")

            // test line <-> plane intersection
            Line<double>myLine(Point3d(0,0,0), Vector3d(1,0,0));

        Point3d myIntersection;
        bool myIntersected = intersection(myLine, myPlane2, myIntersection);
        ENSURE_MSG(!myIntersected, "Line parallel to plane should not intersect.");

        Line<double>myLine2(Point3d(0,0,0), Vector3d(0,1,0) );
        const Plane<double> myPlane7(Vector3d(0,1,0), -1.0);
        bool myIntersected2 = intersection(myLine2, myPlane7, myIntersection);
        ENSURE_MSG(myIntersected2, "Line <-> Plane should intersect.");
        DPRINT (myIntersection[0]);
        DPRINT (myIntersection[1]);
        DPRINT (myIntersection[2]);
        ENSURE_MSG( almostEqual(myIntersection, Point3d(0, 1, 0)), "Intersects at (0,1,0)")

        Line<double>myLine3(Point3d(1,0,0), Vector3d(0,1,0) );
        const Plane<double> myPlane8(Vector3d(0,1,0), 0.0);
        myIntersected2 = intersection(myLine3, myPlane8, myIntersection);
        ENSURE_MSG(myIntersected2, "Line <-> Plane should intersect.");
        DPRINT (myIntersection[0]);
        DPRINT (myIntersection[1]);
        DPRINT (myIntersection[2]);
        ENSURE_MSG( almostEqual(myIntersection, Point3d(1, 0, 0)), "Intersects at (0,0,0)")

        // test Box3/Plane intersection
        {
            Plane<double> myXZPlane(Vector3d(0, 1, 0), 0);
            Box3<double> myOverlapedBox(-2, -2, -2, 2, 2, 2);
            int myHalfSpace;
            ENSURE_MSG(intersection(myOverlapedBox, myXZPlane, myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == 0, "Testing if box lies in both halfspaces");

            Box3<double> myHotBox(2, 2, 2, 4, 4, 4);
            ENSURE_MSG( ! intersection(myHotBox, myXZPlane, myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == 1, "Testing if box lies in positive halfspace");

            Box3<double> myColdBox(-4, -4, -4, -2, -2, -2);
            ENSURE_MSG( ! intersection(myColdBox, myXZPlane, myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == -1, "Testing if box lies in negative halfspace");
        }
        {
            Vector3d myNormal(sqrt(2.0), sqrt(2.0), 0);
            Plane<double> myInclinedPlane(myNormal, 0);
            Box3<double> myOverlapedBox(-2, -2, -2, 2, 2, 2);
            int myHalfSpace;
            ENSURE_MSG(intersection(myOverlapedBox, myInclinedPlane, myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == 0, "Testing if box lies in both halfspaces");

            Box3<double> myTouchingBox(0, 0, 0, 2, 2, 2);
            ENSURE_MSG(intersection(myTouchingBox, myInclinedPlane, myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == 0, "Testing if box lies in both halfspaces");

            Box3<double> myHotBox(2, 2, 2, 4, 4, 4);
            ENSURE_MSG( ! intersection(myHotBox, myInclinedPlane,myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == 1, "Testing if box lies in positive halfspace");

            Box3<double> myColdBox(-4, -4, -4, -2, -2, -2);
            ENSURE_MSG( ! intersection(myColdBox, myInclinedPlane, myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == -1, "Testing if box lies in negative halfspace");
        }

        {
            Vector3d myNormal(sqrt(2.0) * 0.5, sqrt(2.0) * 0.5, 0);
            Plane<double> myInclinedPlane(myNormal, -5 * sqrt(2.0));
            int myHalfSpace;
            Box3<double> myOverlapedBox(4, 4, 4, 6, 6, 6);
            ENSURE_MSG(intersection(myOverlapedBox, myInclinedPlane, myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == 0, "Testing if box lies in both halfspace");

            Box3<double> myTouchingBox(2.5, 2.5, 2.5, 5, 5, 5);
            ENSURE_MSG(intersection(myTouchingBox, myInclinedPlane, myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == 0, "Testing if box lies in both halfspaces");

            Box3<double> myHotBox(6, 6, 6, 8, 8, 8);
            ENSURE_MSG( ! intersection(myHotBox, myInclinedPlane,myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == 1, "Testing if box lies in positive halfspace");

            Box3<double> myColdBox(-2, -2, -2, 3, 3, 3);
            ENSURE_MSG( ! intersection(myColdBox, myInclinedPlane, myHalfSpace), "Testing Box3/Plane intersection");
            ENSURE_MSG(myHalfSpace == -1, "Testing if box lies in negative halfspace");
        }

        // test Plane transformation
        {
            Plane<double>   myInclinedPlane(Vector3d( 0.5 * sqrt(2.0), 0.5 * sqrt(2.0), 0), 0);
            Matrix4<double> myRotation;
            myRotation.makeZRotating(asl::PI / 2.0);
            Plane<double> myResultPlane = myInclinedPlane *  myRotation;
            ENSURE_MSG(almostEqual(myResultPlane.normal, Vector3d(-sqrt(2.0) * 0.5 ,sqrt(2.0) * 0.5, 0)),
                       "Testing transformed plane normal.");
            ENSURE_MSG(almostEqual(myResultPlane.offset, 0), "Testing transformed plane offset.");

            Matrix4<double> myTranslation;
            myTranslation.makeTranslating(Vector3d(10,10,10));
            Plane<double> myTranslatedPlane = myResultPlane * myTranslation;

            ENSURE_MSG(almostEqual(myTranslatedPlane.normal, Vector3d(-sqrt(2.0) * 0.5 ,sqrt(2.0) * 0.5, 0)),
                        "Testing transformed plane normal.");
            ENSURE_MSG(almostEqual(myTranslatedPlane.offset, 0 ), "Testing transformed plane offset.");
            myTranslation.makeTranslating(Vector3d(0.0, 10.0, 0.0));
            myTranslatedPlane =  myTranslatedPlane * myTranslation;
            ENSURE_MSG(almostEqual(myTranslatedPlane.normal, Vector3d(-sqrt(2.0) * 0.5 ,sqrt(2.0) * 0.5, 0)),
                        "Testing transformed plane normal.");
            ENSURE_MSG(almostEqual(myTranslatedPlane.offset, - 5 * sqrt(2.0) ), "Testing transformed plane offset.");
        }
        {
            Plane<double> myXYPlane(Vector3d(0, 0, 1), 0);
            Plane<double> myXZPlane(Vector3d(0, 1, 0), 0);
            Plane<double> myYZPlane(Vector3d(1, 0, 0), 0);
            Point3d myIntersection;
            ENSURE_MSG(intersection(myXYPlane, myXZPlane, myYZPlane, myIntersection),
                       "Testing Plane/Plane/Plane intersection");
            ENSURE_MSG(almostEqual(myIntersection, Vector3d(0.0, 0.0, 0.0)), "Testing intersection point");

            Plane<double> myPlane1(Vector3d( 0.5 * sqrt(2.0), -0.5 * sqrt(2.0), 0), Point3d(0, 1, 0));
            Plane<double> myPlane2(Vector3d(0, -1 , 0), Point3d(0, 2, 0));
            Plane<double> myPlane3(Vector3d(0, 0, 1), Point3d(0, 0, -1));
            ENSURE_MSG(intersection(myPlane1, myPlane2, myPlane3, myIntersection),
                       "Testing Plane/Plane/Plane intersection");
            ENSURE_MSG(almostEqual(myIntersection, Vector3d(1.0, 2.0, -1.0)), "Testing intersection point");

            myPlane1.normal = Vector3d(0, -1, 0);
            myPlane1.offset = 10;
            ENSURE_MSG( ! intersection(myPlane1, myPlane2, myPlane3, myIntersection),
                       "Testing Plane/Plane/Plane intersection");

        }
        { // distace stuff
            Plane<double> myPlane(Vector3d(0,1,0), -1);
            Point3d myNearest = nearest(Point3d(0,3,0), myPlane);
            double myDistance = distance(Point3d(0,3,0), myPlane);
            DPRINT(myDistance);
            ENSURE(distance(Point3d(0,3,0), myPlane) == distance(myPlane, Point3d(0,3,0)));
            ENSURE(almostEqual(myDistance, 2.0));
        }
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
		addTest(new VectorUnitTest); // tests for objects with 3 elements
		                             // Point3, TripleOf, Vector3
        addTest(new LinearAlgebraUnitTest);
    }
};

int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

	std::cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

