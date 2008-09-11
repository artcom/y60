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
//    $RCSfile: testMatrix4Base.tst.cpp,v $
//
//    $Revision: 1.6 $
//
//    Description: unit test for matrixbase
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Matrix4Base.h"
#include "Quaternion.h"

#include "numeric_functions.h"

#include <asl/base/UnitTest.h>
#include <iostream>

using namespace std;
using namespace asl;

template <class T>
class Matrix4BaseUnitTest : public UnitTest {
    public:
        Matrix4BaseUnitTest() : UnitTest("Matrix4BaseUnitTest") {  }
        void run() {
            testAlmostEqual();
            testEqualOperator();
            testAssign();
            testCopyConstructor();
            testAssignGetRowColumn();
            testTranspose();
            testNorm();
            testMakeIdentity();
            testMakeXYZRotating();
            testGetXYZRotation();
            testMakeGetRotating();
            testMakeGetScalingTranslating();
            testRotate();
            testScaleTranslate();
            testPrimeNumbers();
            testMultiply();
            testMultiplyLinearAffine();
            testInvertAffine();
            testInvertLinear();
        }
    
    private:

        void printMatrix(const Matrix4Base<T> & myMatrix) {
            cerr << endl 
                << myMatrix[0][0] << " " 
                << myMatrix[0][1] << " "
                << myMatrix[0][2] << " "
                << myMatrix[0][3] << endl
                << myMatrix[1][0] << " "
                << myMatrix[1][1] << " "
                << myMatrix[1][2] << " "
                << myMatrix[1][3] << endl
                << myMatrix[2][0] << " "
                << myMatrix[2][1] << " "
                << myMatrix[2][2] << " "
                << myMatrix[2][3] << endl
                << myMatrix[3][0] << " "
                << myMatrix[3][1] << " "
                << myMatrix[3][2] << " "
                << myMatrix[3][3] << " "
                << endl;
        }
        void testAlmostEqual() {
            Matrix4Base<T> myMatrix1;
            Matrix4Base<T> myMatrix2;
            myMatrix1.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);                            
            myMatrix2.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);            
            ENSURE_MSG(almostEqual(myMatrix1, myMatrix2),
                "Testing almostEqual<Matrix4Base>");

            T * myMatrix2Ptr = myMatrix2.begin()->begin();
            for (unsigned i = 0; i < 16; ++i) {
                myMatrix2.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);            
                *myMatrix2Ptr = -1;
                myMatrix2Ptr++;
                ENSURE_MSG(!almostEqual(myMatrix1, myMatrix2),
                    "Testing almostEqual<Matrix4Base>");            
            }
        }

        void testEqualOperator() {
            Matrix4Base<T> myMatrix1;
            myMatrix1.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);            
            Matrix4Base<T> myMatrix2;
            myMatrix2.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);            
            Matrix4Base<T> myMatrix3;
            myMatrix3.assign(23,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);           
            ENSURE(myMatrix1 == myMatrix2);
            ENSURE(!(myMatrix1 == myMatrix3));
            ENSURE(!(myMatrix1 != myMatrix2));
            ENSURE(myMatrix1 != myMatrix3);
        }

        void testAssign() {
            Matrix4Base<T> myMatrix;
            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);         
            for (unsigned i = 0; i < 4; ++i) {
                Vector4<T> myExpectedResult(T(i * 4), T(i * 4 + 1), 
                    T(i * 4 + 2), T(i * 4 + 3));
                ENSURE_MSG(almostEqual(myMatrix[i], myExpectedResult),
                    "Testing [] operator");
                Vector4<T> myVector((T)i, (T)i, (T)i, (T)i);
                myMatrix[i] = myVector;
                ENSURE(almostEqual(myMatrix[i], myVector));
            }
        }

   		unsigned getNextPrime(unsigned myNumber) {
			while(myNumber < INT_MAX) {
				myNumber++;
				unsigned i = 2;
				for (; i < myNumber; ++i) {
					if (!(myNumber % i)) {
						break;
					}
				}

				if (myNumber == i) {
					return myNumber;
				}
			}

			return 0;
		}

        void testCopyConstructor() {
            Matrix4Base<T> myMatrix1;
            myMatrix1.assign(0,1,2,3,4,50,6,7,8,9,100,11,12,13,14,105);
            Matrix4Base<T> myMatrix2(myMatrix1);
            ENSURE_MSG(almostEqual(myMatrix1, myMatrix2), "Testing copy constructor");
        }

        void testAssignGetRowColumn() {
            Matrix4Base<T> myMatrix;                
            for (unsigned i = 0; i < 4; ++i) { 
                Vector4<T> myVector(T(9 * i), T(6 * i), T(3 * i), T(1 * i));

                myMatrix.assignRow(i, myVector);
                ENSURE_MSG(almostEqual(myMatrix.getRow(i), myVector), 
                    "Testing assignRow / getRow with VectorPOD");
                myMatrix.assignColumn(i, myVector);
                ENSURE_MSG(almostEqual(myMatrix.getColumn(i), myVector), 
                    "Testing assignColumn / getColumn with VectorPOD");

                Vector4<T> myQuadrupel(T(9 * i), T(6 * i), T(3 * i), T(1 * i));
                myMatrix.assignRow(i, myQuadrupel);
                ENSURE_MSG(almostEqual(myMatrix.getRow(i), myQuadrupel), 
                    "Testing assignRow / getRow with Quadrupel");
                myMatrix.assignColumn(i, myQuadrupel);
                ENSURE_MSG(almostEqual(myMatrix.getColumn(i), myQuadrupel), 
                    "Testing assignColumn / getColumn with Quadrupel");
            }
        }

        void testTranspose() {
            Matrix4Base<T> myMatrix;
            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
            Matrix4Base<T> myMatrixT;
            myMatrixT.assign(0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15);

            myMatrix.transpose();
            ENSURE_MSG(almostEqual(myMatrix, myMatrixT), "Testing transpose");
/*
            myMatrix.transposeAffine();
            Matrix4Base<T> myMatrixTa;
            myMatrixTa.assign(0,1,2,12,4,5,6,13,8,9,10,14,3,7,11,15);
            ENSURE_MSG(almostEqual(myMatrix, myMatrixTa), "Testing transpose");
*/
        }

        void testNorm() {
            Matrix4Base<T> myMatrix;
            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);         
            T myNorm = myMatrix.norm();
            ENSURE_MSG(almostEqual(
                sqrt(double(1+4+9+16+25+36+49+64+81+100+121+144+169+196+225)), myNorm), 
                "Testing norm");
        }

        void testMakeIdentity() {
            Matrix4Base<T> myMatrix;
            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);         
            myMatrix.makeIdentity();
            Matrix4Base<T> myIdentity;
            myIdentity.assign(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1); 
            ENSURE_MSG(almostEqual(myMatrix, myIdentity), "Testing makeIdentity");
        }

        void testMakeXYZRotating() {
            Matrix4Base<T> myMatrix;
            myMatrix.makeIdentity();
            myMatrix.makeXRotating(T(0.123));
            T c = T(cos(0.123));
            T s = T(sin(0.123));
            Matrix4Base<T> myExpectedResult;
            myExpectedResult.assign(1,0,0,0,0,c,s,0,0,-s,c,0,0,0,0,1);  
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeXRotating");

            myMatrix.makeIdentity();
            myMatrix.makeYRotating(T(0.123));
            myExpectedResult.assign(c,0,-s,0,0,1,0,0,s,0,c,0,0,0,0,1);  
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeYRotating");

            myMatrix.makeIdentity();
            myMatrix.makeZRotating(T(0.123));
            myExpectedResult.assign(c,s,0,0,-s,c,0,0,0,0,1,0,0,0,0,1);  
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeZRotating");
            
            myMatrix.makeIdentity();
            myMatrix.makeZRotating(3);
            myMatrix.rotateY(2);
            myMatrix.rotateX(1);
            myExpectedResult.makeXYZRotating(Vector3<T>(1,2,3));  
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeXYZRotating");

            myMatrix.makeIdentity();
            myMatrix.makeZRotating(-10);
            myMatrix.rotateY(-20);
            myMatrix.rotateX(0.5);
            myExpectedResult.makeXYZRotating(Vector3<T>(0.5,-20,-10));  
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeXYZRotating");
            
        }

        void testGetXYZRotation() {
            Matrix4Base<T> myMatrix;
            ENSURE_EXCEPTION(myMatrix.getXRotation(), NotYetImplemented);
            ENSURE_EXCEPTION(myMatrix.getYRotation(), NotYetImplemented);
            ENSURE_EXCEPTION(myMatrix.getZRotation(), NotYetImplemented);
        }

        void testMakeGetRotating() {
            Matrix4Base<T> myMatrix;
            myMatrix.makeIdentity();
            Vector3<T> myAxis(1, 2, 3);
            myAxis = normalized(myAxis);
            myMatrix.makeRotating(myAxis, T(0.123));
            
            Vector3<T> myResultAxis;
            T myResultAngle;
            myMatrix.getRotation(myResultAxis, myResultAngle);
            myResultAxis = normalized(myResultAxis);
            ENSURE_MSG(almostEqual(myAxis, myResultAxis), "Testing makeRotating/getRotation - axis");
            ENSURE_MSG(almostEqual(0.123, myResultAngle), "Testing makeRotating/getRotation - angle");              
        }

        void testMakeGetScalingTranslating() {
            Matrix4Base<T> myMatrix;
            Matrix4Base<T> myExpectedResult;
            myMatrix.makeIdentity();
            myMatrix.makeScaling(Vector3<T>(3, -2, -1));                
            myExpectedResult.assign(3,0,0,0,0,-2,0,0,0,0,-1,0,0,0,0,1); 
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeScaling");

            Vector3<T> myResult = myMatrix.getScaling();
            ENSURE_MSG(almostEqual(myResult, Vector3<T>(3, -2, -1)), "Testing getScaling");

            myMatrix.makeIdentity();
            myMatrix.makeTranslating(Vector3<T>(9, -20, -10000));               
            myExpectedResult.assign(1,0,0,0,0,1,0,0,0,0,1,0,9,-20,-10000,1);    
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeTranslating");

            myResult = myMatrix.getTranslation();
            ENSURE_MSG(almostEqual(myResult, Vector3<T>(9, -20, -10000)), "Testing getTranslation");

            ENSURE_MSG(true, "Checking for beer in the fridge");
        }

        void testRotate() {
            Matrix4Base<T> myMatrix;
            Matrix4Base<T> myExpectedResult;
            myMatrix.makeIdentity();
            myMatrix.rotateX(T(PI));                
            myExpectedResult.assign(1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1); 
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing rotateX");

            myMatrix.rotateX(T(PI_4));              
            myMatrix.rotateX(T(PI_4));              
            myMatrix.rotateX(T(PI_4));              
            myMatrix.rotateX(T(PI_4));              
            myExpectedResult.makeIdentity();
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing rotateX");

            myMatrix.makeIdentity();
            myMatrix.rotateY(T(PI));                
            myExpectedResult.assign(-1,0,0,0,0,1,0,0,0,0,-1,0,0,0,0,1); 
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing rotateY");


            myMatrix.rotateY(T(PI_4));              
            myMatrix.rotateY(T(PI));                                
            myMatrix.rotateY(T(-PI_4));             
            myExpectedResult.makeIdentity();
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing rotateY");

            myMatrix.makeIdentity();
            myMatrix.rotateZ(T(PI));                
            myExpectedResult.assign(-1,0,0,0,0,-1,0,0,0,0,1,0,0,0,0,1); 
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing rotateZ");

            myMatrix.rotateZ(T(PI_4));              
            myMatrix.rotateZ(T(PI_2));                          
            myMatrix.rotateZ(T(PI_4));              
            myExpectedResult.makeIdentity();
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing rotateZ");

            myMatrix.makeIdentity();
            myMatrix.rotate(Vector3<T>(1, 0, 0), T(0.123));
            myExpectedResult.makeIdentity();
            myExpectedResult.makeXRotating(T(0.123));            
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing rotate");

            myMatrix.makeIdentity();
            myMatrix.rotate(Vector3<T>(0, 1, 0), T(0.321));
            myExpectedResult.makeIdentity();
            myExpectedResult.makeYRotating(T(0.321));
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing rotate");
            
            myMatrix.makeIdentity();
            myMatrix.rotate(Vector3<T>(0, 0, 1), T(-1.123));
            myExpectedResult.makeIdentity();
            myExpectedResult.makeZRotating(T(-1.123));
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing rotate");

            Matrix4Base<T> myTestPoint1;
            Matrix4Base<T> myTestPoint2;
            myTestPoint1.makeIdentity();
            myTestPoint1.makeTranslating(Vector3<T>(1,0,0));
            myTestPoint2.makeIdentity();

            myMatrix.makeIdentity();
            myMatrix.rotate(normalized(Vector3<T>(1, 1, 0)), T(PI));            
            Matrix4Base<T>::multiplyFull(myTestPoint1, myMatrix, myTestPoint2);
            ENSURE_MSG(almostEqual(myTestPoint2.getTranslation(), Vector3<T>(0,1,0)), "Testing rotate");
        }

        void testScaleTranslate() { 
            Matrix4Base<T> myMatrix;
            Matrix4Base<T> myExpectedResult;
            myMatrix.assign(1,2,3,5,7,11,13,17,19,23,29,31,37,41,43,47);    
            myMatrix.scale(Vector3<T>(-1,2,-2));                
            myExpectedResult.assign(-1,4,-6,5,-7,22,-26,17,-19,46,-58,31,-37,82,-86,47);    
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing scale");

            myMatrix.assign(1,2,3,5,7,11,13,17,19,23,29,31,37,41,43,47);                    
            Matrix4Base<T> myTranslation;
            myTranslation.makeIdentity();
            myTranslation.makeTranslating(Vector3<T>(2,-4,75));
            myExpectedResult.multiplyFull(myMatrix, myTranslation, myExpectedResult);
            myMatrix.translate(Vector3<T>(2,-4,75));                
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing translate");
        }

        void testPrimeNumbers() {
            unsigned myPrime = 0;
            cerr << "Programmers need a break, too. Lets make the processor burn a bit" << endl;
            for (unsigned i = 0; i < 1000; ++i) {
                myPrime = getNextPrime(myPrime);                
                cerr << myPrime << " ";
            }
            cerr << endl;
        }

        void testMultiply() {
            Matrix4Base<T> myMatrix1;
            myMatrix1.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);            
            Matrix4Base<T> myMatrix2;
            myMatrix2.assign(15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0);
            Matrix4Base<T> myExpectedMatrix;
            for (unsigned i = 0; i < 4; ++i) {
                for (unsigned j = 0; j < 4; ++j) {
                    myExpectedMatrix[i][j] = dot(myMatrix1.getRow(i), myMatrix2.getColumn(j));
                }
            }
            myMatrix1.multiply(myMatrix2);
            ENSURE_MSG(almostEqual(myMatrix1, myExpectedMatrix),
                "Testing multiply()");

            for (unsigned i = 0; i < 4; ++i) {
                for (unsigned j = 0; j < 4; ++j) {
                    myExpectedMatrix[i][j] = dot(myMatrix2.getRow(i), myMatrix1.getColumn(j));
                }
            }

            myMatrix1.preMultiply(myMatrix2);
            ENSURE_MSG(almostEqual(myMatrix1, myExpectedMatrix),
                "Testing preMultiply()");
        }

        void testMultiplyLinearAffine()  {
            Matrix4Base<T> myMatrix1;
            Matrix4Base<T> myMatrix2;
            Matrix4Base<T> myExpectedMatrix;
            Matrix4Base<T> myResult;
            myMatrix1.assign(0,1,2,0,4,5,6,0,8,9,10,0,0,0,0,1);         
            myMatrix2.assign(15,14,13,0,11,10,9,0,7,6,5,0,0,0,0,1);
            myResult.assign(0,1,2,0,4,5,6,0,8,9,10,0,0,0,0,1);          
            Matrix4Base<T>::multiplyLinear(myMatrix1, myMatrix2, myResult);
            Matrix4Base<T>::multiplyFull(myMatrix1, myMatrix2, myExpectedMatrix);
            ENSURE_MSG(almostEqual(myResult, myExpectedMatrix), "Testing multiplyLinear()");

            myMatrix1.assign(0,1,2,0,4,5,6,0,8,9,10,0,12,13,14,1);          
            myMatrix2.assign(15,14,13,0,11,10,9,0,7,6,5,0,3,2,1,1);
            myResult.assign(0,1,2,0,4,5,6,0,8,9,10,0,12,13,14,1);
            Matrix4Base<T>::multiplyAffine(myMatrix1, myMatrix2, myResult);
            Matrix4Base<T>::multiplyFull(myMatrix1, myMatrix2, myExpectedMatrix);
            ENSURE_MSG(almostEqual(myResult, myExpectedMatrix), "Testing multiplyAffine()");

            myMatrix1.assign(0,1,2,0,4,5,6,0,8,9,10,0,0,0,0,1);                 
            myMatrix2.assign(1,0,0,0,0,1,0,0,0,0,1,0,-3,-2,-1,1);
            myResult.assign(0,1,2,0,4,5,6,0,8,9,10,0,0,0,0,1);
            Matrix4Base<T>::multiplyLinearTranslating(myMatrix1, myMatrix2, myResult);
            Matrix4Base<T>::multiplyFull(myMatrix1, myMatrix2, myExpectedMatrix);
            ENSURE_MSG(almostEqual(myResult, myExpectedMatrix), "Testing multiplyLinearTranslating()");

            myMatrix1.assign(1,0,0,0,0,1,0,0,0,0,1,0,-3,-2,-1,1);
            myMatrix2.assign(0,1,2,0,4,5,6,0,8,9,10,0,0,0,0,1);                             
            myResult.assign(1,0,0,0,0,1,0,0,0,0,1,0,-3,-2,-1,1);
            Matrix4Base<T>::multiplyTranslatingLinear(myMatrix1, myMatrix2, myResult);
            Matrix4Base<T>::multiplyFull(myMatrix1, myMatrix2, myExpectedMatrix);
            ENSURE_MSG(almostEqual(myResult, myExpectedMatrix), "Testing multiplyTranslatingLinear()");

            ENSURE_MSG(true, "Checking what your girlfriend is doing right now.");

            myMatrix1.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
            myMatrix2.assign(0,1,2,0,4,5,6,0,8,9,10,0,0,0,0,1);                             
            myResult.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
            Matrix4Base<T>::multiplyUnkownLinear(myMatrix1, myMatrix2, myResult);
            Matrix4Base<T>::multiplyFull(myMatrix1, myMatrix2, myExpectedMatrix);
            ENSURE_MSG(almostEqual(myResult, myExpectedMatrix), "Testing multiplyUnkownAffine()");            

            myMatrix1.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);            
            myMatrix2.assign(15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0);
            myExpectedMatrix.assign(15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15);
            Matrix4Base<T>::add(myMatrix1, myMatrix2, myResult);
            ENSURE_MSG(almostEqual(myResult, myExpectedMatrix), "Testing add()");            
        }

        void testInvertAffine() {
            Matrix4Base<T> myMatrix;
            myMatrix.assign(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1);
            ENSURE_MSG(!myMatrix.invertAffine(), "Testing if affine matrix is invertable");
            myMatrix.assign(3,1,2,0,4,5,6,0,8,9,10,0,12,13,14,1);
            Matrix4Base<T> myMatrix2(myMatrix);
            Matrix4Base<T> myResult;
            Matrix4Base<T> myExpectedMatrix;
            myExpectedMatrix.makeIdentity();

            ENSURE_MSG(myMatrix2.invertAffine(), "Testing if affine matrix is invertable");
            Matrix4Base<T>::multiplyFull(myMatrix2, myMatrix, myResult);
            ENSURE_MSG(almostEqual(myResult, myExpectedMatrix), "Testing inverseAffine()");            
        }

        void testInvertLinear() {
            Matrix4Base<T> myMatrix;
            myMatrix.assign(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1);
            ENSURE_MSG(!myMatrix.invertLinear(), "Testing if linear matrix is invertable");
            myMatrix.assign(3,1,2,0,4,5,6,0,8,9,10,0,0,0,0,1);
            Matrix4Base<T> myMatrix2(myMatrix);
            Matrix4Base<T> myResult;
            Matrix4Base<T> myExpectedMatrix;
            myExpectedMatrix.makeIdentity();

            ENSURE_MSG(myMatrix2.invertLinear(), "Testing if linear matrix is invertable");
            Matrix4Base<T>::multiplyFull(myMatrix2, myMatrix, myResult);
            ENSURE_MSG(almostEqual(myResult, myExpectedMatrix), "Testing inverseLinear()");            

        }

};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        addTest(new Matrix4BaseUnitTest<float>);
        addTest(new Matrix4BaseUnitTest<double>);
    }
};

int main(int argc, char *argv[]) {
    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();
	std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << endl;
    return mySuite.returnStatus();
}
