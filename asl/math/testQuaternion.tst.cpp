//==============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================
//
//    $RCSnumeric: test_linearAlgebra.tst.cpp,v $
//
//   $Revision: 1.11 $
//
// Description: unit test for linear algebra
//
//
//==============================================================================

#include "numeric_functions.h"
#include "linearAlgebra.h"
#include "Quaternion.h"

#include <asl/base/UnitTest.h>

#include <iostream>
#include <cmath>

using namespace std;
using namespace asl;

template <class T>
class QuaternionTest : public TemplateUnitTest {
    public:
        QuaternionTest(const char * theTemplateArgument) :
               TemplateUnitTest("Quaternion", theTemplateArgument) 
        {}
        void run() {
            
			Ball<T> myBall;
			Quaternion<T> myQuat1(1.0, 2.0, 3.0, 4.0);                        
            Quaternion<T> myQuat2(5.0, 6.0, 7.0, 8.0);

            // Test getters
            ENSURE(almostEqual(myQuat2.getImaginaryPart(), Vector3<T>(5.0, 6.0, 7.0)));
            ENSURE(myQuat2.getRealPart() == 8.0);

            // Test multiplication
            Quaternion<T> myMultResult = myQuat1 * myQuat2;
            Quaternion<T> myExpectedMultResult;
                
            myExpectedMultResult.setImaginaryPart(cross(myQuat1.getImaginaryPart(), myQuat2.getImaginaryPart()) 
                + myQuat2.getRealPart() * myQuat1.getImaginaryPart() + myQuat1.getRealPart() * myQuat2.getImaginaryPart());

            myExpectedMultResult.setRealPart(myQuat1.getRealPart() * myQuat2.getRealPart() - 
                dot(myQuat1.getImaginaryPart(), myQuat2.getImaginaryPart()));

            ENSURE_MSG(almostEqual(myMultResult, myExpectedMultResult), "Testing quaternion multiplication");

            // Test normalize
            myQuat1.normalize();
            Quaternion<T> myExpectedNorm(T(1.0 / sqrt(30.0)), T(2.0 / sqrt(30.0)), 
                                         T(3.0 / sqrt(30.0)), T(4.0 / sqrt(30.0)));
            ENSURE_MSG(almostEqual(myQuat1, myExpectedNorm), "Testing quaternion normalization");

           // Test axis/angle constructor
            Quaternion<T> myQuat3(Vector3<T>(0.0, 0.0, 1.0), T(PI));
            Quaternion<T> myExpectedQuat3(0.0, 0.0, -1.0, 0.0);
            ENSURE_MSG(almostEqual(myQuat3, myExpectedQuat3), "Testing axis, angle constructur");
            Quaternion<T> myQuat4(Vector3<T>(10.0, 20.0, 30.0), 10.0);
            Quaternion<T> myExpectedQuat4(T((-10.0 / 37.4165739) * sin(10.0 / 2)), 
                                          T((-20.0 / 37.4165739) * sin(10.0 / 2)), 
                                          T((-30.0 / 37.4165739) * sin(10.0 / 2)), 
                                          T(cos(10.0 / 2)));            
            ENSURE_MSG(almostEqual(myQuat4, myExpectedQuat4), "Testing axis, angle constructur");
                        
            {
                // Test construction from Euler XYZ
                Quaternion<T> myQuatNeutral(Quaternion<T>::createFromEuler(Vector3<T>(0.0, 0.0, 0.0)));
                ENSURE_MSG(almostEqual(myQuatNeutral, Quaternion<T>(T(0),T(0),T(0),T(1))), 
                        "Euler Constructor, neutral");

                Quaternion<T> myQuat1 = Quaternion<T>::createFromEuler(Vector3<T>(T(PI_2),0,0));
                T mySquareRootTwoHalf = static_cast<T>(sqrt(2.0)) * 0.5;
                ENSURE_MSG(almostEqual(myQuat1, Quaternion<T>(-mySquareRootTwoHalf,T(0),T(0),
                                mySquareRootTwoHalf)), "Euler Constructor, X+90");

                // these should cancel each other out (think about it...)
                Quaternion<T> myQuatX = Quaternion<T>::createFromEuler(Vector3<T>(T(PI_2), 0, 0));
                Quaternion<T> myQuatZ1 = Quaternion<T>::createFromEuler(Vector3<T>(0, 0, T(PI_2)));
                Quaternion<T> myQuatY = Quaternion<T>::createFromEuler(Vector3<T>(0, -T(PI_2), 0));
                Quaternion<T> myQuatZ2 = Quaternion<T>::createFromEuler(Vector3<T>(0, 0, -T(PI_2)));

                Quaternion<T> myNullQuat = myQuatX * myQuatZ1 * myQuatY * myQuatZ2;
                DPRINT(myNullQuat);
                ENSURE_MSG(almostEqual(myNullQuat, myQuatNeutral), "combination cancel test");

                Vector3<T> myRotation(T(PI_2)/2,T(PI_2)/3,T(PI_2)/4);
                Quaternion<T> myQuat = Quaternion<T>::createFromEuler(myRotation);
                Matrix4<T> myQuatMatrix(myQuat);
                Matrix4<T> myRotateMatrix;
                myRotateMatrix.makeIdentity();
                myRotateMatrix.rotateXYZ(myRotation);
                ENSURE(almostEqual(myRotateMatrix, myQuatMatrix));
            }
            {
                // test construction by two vectors
                Vector3<T> myFirstVector(1,0,0);
                Vector3<T> mySecondVector(0,1,0);
                Quaternion<T> myQuaternion(myFirstVector, mySecondVector);
                Matrix4<T> myProjection(myQuaternion);
                Matrix4<T> myInverse(myQuaternion);
                Point3<T> myPoint = product(Point3<T>(mySecondVector), myProjection);
                Vector3<T> myResult(myPoint);
                ENSURE(almostEqual(myResult, myFirstVector));
            }
            
            {   // test quaternion calculus
                
                // Vector * Quaternion 
                asl::Vector4<T> myVector(-1,0,0,1);
                Quaternion<T> myQuat1 = Quaternion<T>::createFromEuler(Vector3<T>(0,0,T(PI_2)));
                asl::Vector4<T> myResultVector = product(myVector, myQuat1);
                ENSURE(almostEqual(myResultVector, asl::Vector4<T>(0,-1,0,1)));

                // Quaternion * Vector
                ENSURE_EXCEPTION(product(myQuat1, myVector), asl::Exception);
                
                // Quaternion * Scalar and Scalar * Quaternion 
                Quaternion<T> myQuat2 = Quaternion<T>::createFromEuler(Vector3<T>(0,0,T(PI_2)));
                Quaternion<T> myQuat3 = Quaternion<T>::createFromEuler(Vector3<T>(0,0,T(PI_2)));
                Quaternion<T> myResultQuaternion = product(myQuat3, T(0.5));
                Quaternion<T> myResultQuaternion2 = T(2.0) * myResultQuaternion;
                ENSURE(almostEqual(myQuat2, myResultQuaternion2));                


                // Quaternion + Quaternion 
                Quaternion<T> myQuat4 = Quaternion<T>::createFromEuler(Vector3<T>(0,0,T(PI_2)));
                Quaternion<T> myQuat5 = Quaternion<T>::createFromEuler(Vector3<T>(0,T(PI_2),0));
                Quaternion<T> myExpectedResult2(0, T(sqrt(2.0) * -0.5), T(sqrt(2.0) * -0.5), T(sqrt(2.0) ) ) ;
                Quaternion<T> myResultQuaternion3 = sum(myQuat4,myQuat5);
                ENSURE(almostEqual(myResultQuaternion3, myExpectedResult2));                
                
                // Quaternion - Quaternion 
                Quaternion<T> myQuat6 = Quaternion<T>::createFromEuler(Vector3<T>(0,0,T(PI_2)));
                Quaternion<T> myQuat7 = Quaternion<T>::createFromEuler(Vector3<T>(0,-T(PI_2),0));
                Quaternion<T> myExpectedResult3(0, T(sqrt(2.0) * -0.5), T(sqrt(2.0) * -0.5), 0 ) ;
                Quaternion<T> myResultQuaternion4 = difference(myQuat6, myQuat7);
                ENSURE(almostEqual(myResultQuaternion4, myExpectedResult3));                


                // Quaternion / Scalar
                Quaternion<T> myQuat8 = Quaternion<T>::createFromEuler(Vector3<T>(0,0,T(PI_2)));
                Quaternion<T> myQuat9 = Quaternion<T>::createFromEuler(Vector3<T>(0,0,T(PI_2)));
                Quaternion<T> myResultQuaternion5 = quotient(myQuat3, T(2.0));
                Quaternion<T> myResultQuaternion6 = quotient(myResultQuaternion5, T(0.5));
                ENSURE(almostEqual(myQuat9, myResultQuaternion6));                

            }
            
            {
                // Test lerp (linear interpolation)
                Quaternion<T> myQuat1 = Quaternion<T>::createFromEuler(Vector3<T>(0,0,0));
                Quaternion<T> myQuat2 = Quaternion<T>::createFromEuler(Vector3<T>(T(PI_2),0,0));
                asl::Vector4<T> myV1(0,0,1,1);
                Quaternion<T> myResultQuat = lerp<T>(myQuat1, myQuat2, 1.0);
                asl::Vector4<T> myResultVector = product(myV1, myResultQuat);
                ENSURE_MSG(almostEqual(myResultVector, asl::Vector4<T>(0,-1,0,1)), "Testing quaternion lerp");
            }
            {
                // Test slerp (spherical linear interpolation)
                Quaternion<T> myQuat1 = Quaternion<T>::createFromEuler(Vector3<T>(0,0,0));
                Quaternion<T> myQuat2 = Quaternion<T>::createFromEuler(Vector3<T>(T(PI_2),0,0));
                asl::Vector4<T> myV1(0,0,1,1);
                Quaternion<T> myResultQuat = slerp<T>(myQuat1, myQuat2, 1.0);
                asl::Vector4<T> myResultVector = product(myV1, myResultQuat);
                ENSURE_MSG(almostEqual(myResultVector, asl::Vector4<T>(0,-1,0,1)), "Testing quaternion slerp");

            }

        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
		addTest(new QuaternionTest<double>("<double>"));
        addTest(new QuaternionTest<float>("<float>"));
    }
};

int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

	std::cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
