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
//   $RCSfile: testMatrix4.tst.cpp,v $
//
//   $Revision: 1.20 $
//
//   Description: unit test for matrix
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Matrix4.h"
#include "Plane.h"
#include "linearAlgebra.h"
#include "numeric_functions.h"

// temporarily inserted to compare speed with sony library
//#include "vectormath/c/vectormath_aos.h"

#include <asl/base/UnitTest.h>
#include <asl/base/Time.h>
#include <iostream>

using namespace std;
using namespace asl;

bool ourPerformanceTest = true;
#ifdef DEBUG_VARIANT
    const int ourNumberOfMults = 2500;
#else
//    const int ourNumberOfMults = 1000000;
    const int ourNumberOfMults = 250000;
#endif


template <class T>
class Matrix4UnitTest : public UnitTest {
    public:
        Matrix4UnitTest() : UnitTest("Matrix4UnitTest") {
        }

        void run() {
            testAlmostEqual();
            testStaticIdentity();
            testStandardConstructors();
            testQuaternionConstructor();
            testAssign();
            testClassify();
            testGetData();
            testRowColumnAccess();
            testGetType();
            testMakeIdentityScalingTranslating();
            testRotation();
            testRotateXYZ();
            testScale();
            testTranslate();
            testRotateX();
            testRotateY();
            testRotateZ();
            testRotate();
            testPostMultiply();
            testAsterixOperator();
            testTranspose();
            testInversion();
            testInvertClassify();
            testMatrixDecomposition();
            testGetRotation();
            testTypeInvariance();
            testMultiplyMatrixPV();
            testTransformedNormal();
            testEasyMatrixAccess();
            testMakeLookAt();

            if (ourPerformanceTest) {
                finalPerformanceTest();
                testInvertPerformance();
                testMultiplyPerformance();
            }
        }

    private:
        string _myPerformanceString;

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

        void finalPerformanceTest() {
            cerr << endl << "Performance test results:" << endl;
            cerr << _myPerformanceString << endl;

            const unsigned myBigNumberOfMults = ourNumberOfMults;

            // Time rolled out full multiply with c arrays as base line
            typedef T M16[16];
            M16 * a = new M16[myBigNumberOfMults];
            M16 * b = new M16[myBigNumberOfMults];
            M16 * ab = new M16[myBigNumberOfMults];

            for (unsigned i = 0; i < myBigNumberOfMults; ++i) {
                for (int j = 0; j < 16; ++j) {
                    a[i][j] = T(i * j);
                    b[i][j] = T(i * -j);
                }
            }

            cerr << "Baselinetest number of multiplications: " << myBigNumberOfMults << endl;
            asl::NanoTime myBaseTimer;
            for (unsigned i = 0; i < myBigNumberOfMults; ++i) {
                M16 & c = ab[i];
                c[0] = a[i][0] * b[i][0] + a[i][1] * b[i][4] + a[i][2] * b[i][8]  + a[i][3] * b[i][12];
                c[1] = a[i][0] * b[i][1] + a[i][1] * b[i][5] + a[i][2] * b[i][9]  + a[i][3] * b[i][13];
                c[2] = a[i][0] * b[i][2] + a[i][1] * b[i][6] + a[i][2] * b[i][10] + a[i][3] * b[i][14];
                c[3] = a[i][0] * b[i][3] + a[i][1] * b[i][7] + a[i][2] * b[i][11] + a[i][3] * b[i][15];

                c[4] = a[i][4] * b[i][0] + a[i][5] * b[i][4] + a[i][6] * b[i][8]  + a[i][7] * b[i][12];
                c[5] = a[i][4] * b[i][1] + a[i][5] * b[i][5] + a[i][6] * b[i][9]  + a[i][7] * b[i][13];
                c[6] = a[i][4] * b[i][2] + a[i][5] * b[i][6] + a[i][6] * b[i][10] + a[i][7] * b[i][14];
                c[7] = a[i][4] * b[i][3] + a[i][5] * b[i][7] + a[i][6] * b[i][11] + a[i][7] * b[i][15];

                c[8]  = a[i][8] * b[i][0] + a[i][9] * b[i][4] + a[i][10] * b[i][8]  + a[i][11] * b[i][12];
                c[9]  = a[i][8] * b[i][1] + a[i][9] * b[i][5] + a[i][10] * b[i][9]  + a[i][11] * b[i][13];
                c[10] = a[i][8] * b[i][2] + a[i][9] * b[i][6] + a[i][10] * b[i][10] + a[i][11] * b[i][14];
                c[11] = a[i][8] * b[i][3] + a[i][9] * b[i][7] + a[i][10] * b[i][11] + a[i][11] * b[i][15];

                c[12] = a[i][12] * b[i][0] + a[i][13] * b[i][4] + a[i][14] * b[i][8]  + a[i][15] * b[i][12];
                c[13] = a[i][12] * b[i][1] + a[i][13] * b[i][5] + a[i][14] * b[i][9]  + a[i][15] * b[i][13];
                c[14] = a[i][12] * b[i][2] + a[i][13] * b[i][6] + a[i][14] * b[i][10] + a[i][15] * b[i][14];
                c[15] = a[i][12] * b[i][3] + a[i][13] * b[i][7] + a[i][14] * b[i][11] + a[i][15] * b[i][15];
            }
            asl::NanoTime myTime = asl::NanoTime() - myBaseTimer;
            double myMultiplicationsPerSecond = myBigNumberOfMults / double(myTime.millis());
            cerr << endl << "Matrix Multiplication Baseline: " << myMultiplicationsPerSecond << " mults per second" << endl << endl;

            T sum2 = 0;
            for (unsigned i = 0; i < myBigNumberOfMults; ++i) {
                for (int j = 0; j < 16; ++j) {
                    sum2 += ab[i][j];
                }
            }
            cerr << "sum = " << sum2 << endl; // enforce result to avoid excessive optimization

            delete [] a;
            delete [] b;
            delete [] ab;

            Matrix4Base<T> * Ma = new Matrix4Base<T>[myBigNumberOfMults];
            Matrix4Base<T> * Mb = new Matrix4Base<T>[myBigNumberOfMults];
            Matrix4Base<T> * Mc = new Matrix4Base<T>[myBigNumberOfMults];

            for (unsigned i = 0; i < myBigNumberOfMults; ++i) {
                for (int j = 0; j < 4; ++j) {
                    for (int k = 0; k < 4; ++k) {
                        Ma[i][j][k] = T(i * j * k);
                        Mb[i][j][k] = T(i * -j * k);
                    }
                }
            }

            asl::NanoTime myFullTimer;
            for (unsigned i = 0; i < myBigNumberOfMults; ++i) {
                Matrix4Base<T>::multiplyFull(Ma[i], Mb[i], Mc[i]);
            }
            myTime = asl::NanoTime() - myFullTimer;
            myMultiplicationsPerSecond = myBigNumberOfMults / double(myTime.millis());
            cerr << endl << "Matrix4Base Full Multiplication: " << myMultiplicationsPerSecond << " mults per second" << endl << endl;

            T sum = 0;
            for (unsigned i = 0; i < myBigNumberOfMults; ++i) {
                for (int j = 0; j < 4; ++j) {
                    for (int k = 0; k < 4; ++k) {
                        sum += Mc[i][j][k];
                    }
                }
            }
            cerr << "sum = " << sum << endl; // enforce result to avoid excessive optimization

            delete [] Ma;
            delete [] Mb;
        }

        void testMultiplyPerformance(Matrix4<T> a, Matrix4<T> b) {
            Matrix4<T> myResult;

            std::string myOutLine = a.getTypeString() + "\t * " + b.getTypeString() + "\t";
            cerr << myOutLine;
            _myPerformanceString += myOutLine;

           Matrix4<T> * Ma = new Matrix4<T>[ourNumberOfMults];
           Matrix4<T> * Mb = new Matrix4<T>[ourNumberOfMults];
           Matrix4<T> * Mc = new Matrix4<T>[ourNumberOfMults];

           for (unsigned i = 0; i < ourNumberOfMults; ++i) {
               Ma[i] = a;
               Mb[i] = b;
           }

           //static Matrix4<T> myMatrixArray[ourNumberOfMults];
           //static Matrix4<T> myMatrixResults[ourNumberOfMults];

            // dumb full test
            asl::NanoTime myFullTimer;
            for (unsigned i = 0; i < ourNumberOfMults; ++i) {
                Matrix4Base<T>::multiplyFull(Ma[i], Mb[i], Mc[i]);
            }
            asl::NanoTime myFullTime = asl::NanoTime() - myFullTimer;
            double myFullCount = double(ourNumberOfMults) / double(myFullTime.millis());
            myOutLine = string(" - full: ") + as_string(myFullCount)+ "[T="+as_string(myFullTime.millis()) + string("]\t");
            cerr << myOutLine;
            _myPerformanceString += myOutLine;

            // smart test

            asl::NanoTime mySmartTimer;
            for (unsigned i = 0; i < ourNumberOfMults; ++i) {
                //Mc[i] = Ma[i];
                Ma[i].postMultiply(Mb[i]);
            }
            asl::NanoTime mySmartTime = asl::NanoTime() - mySmartTimer;
            double mySmartCount = double(ourNumberOfMults) / double(mySmartTime.millis());
            myOutLine = string(" | smart: ") + as_string(mySmartCount) +  "[T="+as_string(mySmartTime.millis()) + string("]\t");
            cerr << myOutLine;
            _myPerformanceString += myOutLine;

            if (mySmartCount < myFullCount) {
                myOutLine = string("    ### ") +
                    as_string((myFullCount/mySmartCount - 1) * 100) +
                    "% slower.\n";
            } else {
                myOutLine = string("    >>> ") +
                    as_string((mySmartCount/myFullCount - 1) * 100) +
                    "% faster.\n";
            }
            cerr << myOutLine;
            _myPerformanceString += myOutLine;
            delete [] Ma;
            delete [] Mb;
            delete [] Mc;

        }

        void testMatrixMultiplication(Matrix4<T> a, Matrix4<T> b, MatrixType theType) {
            if (ourPerformanceTest) {
                testMultiplyPerformance(a, b);
            }

            Matrix4<T> myOriginalA(a);
            std::string myTypeOfB = b.getTypeString();
            Matrix4Base<T> myMatrix1;
            Matrix4Base<T> myMatrix2;
            Matrix4Base<T> myResult;

            myMatrix1.assign(a.getData());
            myMatrix2.assign(b.getData());

            Matrix4Base<T>::multiplyFull(myMatrix1, myMatrix2, myResult);
            a.postMultiply(b);

            ENSURE_MSG(almostEqual(myResult, Matrix4Base<T>(a.getData())),
                (std::string("Testing postMultiply for: ") + myOriginalA.getTypeString() +
                 " * " + myTypeOfB).c_str());
            ENSURE_MSG(a.getType() == theType,
                (string("Testing postMultiply result type returns: ") + a.getTypeString()).c_str());

            a.assign(myOriginalA);

            b.postMultiply(a);
            Matrix4Base<T>::multiplyFull(myMatrix2, myMatrix1, myResult);

            ENSURE_MSG(almostEqual(myResult,  Matrix4Base<T>(b.getData())),
                (std::string("Testing postMultiply for: ") + myTypeOfB +
                 " * " + myOriginalA.getTypeString()).c_str());
            ENSURE_MSG(b.getType() == theType,
                (string("Testing postMultiply result type returns: ") + b.getTypeString()).c_str());
        }

        void testAlmostEqual() {
            Matrix4<T> myMatrix1;
            Matrix4<T> myMatrix2;
            myMatrix1.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
            myMatrix2.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
            ENSURE_MSG(almostEqual(myMatrix1, myMatrix2),
                "Testing almostEqual<Matrix4>");

            myMatrix2.assign(0,1,2,3,4,5,6,7,8,9,999,11,12,13,14,15);
            ENSURE_MSG(!almostEqual(myMatrix1, myMatrix2),
                "Testing almostEqual<Matrix4>");
        }

        void testStandardConstructors() {
            Matrix4<T> myMatrix1;
            myMatrix1.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
            Matrix4<T> myMatrix2(myMatrix1);

            ENSURE_MSG(almostEqual(myMatrix1, myMatrix2), "Testing copy constructor");
            ENSURE_MSG(myMatrix1.getType() == UNKNOWN, "Test getType()");
        }

        void testQuaternionConstructor() {
            Matrix4<T> myExpectedMatrix;
            {
                Vector3<T> myAxis(T(0.0), T(0.0), T(1.0));
                Quaternion<T> myQuat(myAxis, float(PI));
                Matrix4<T> myMatrix(myQuat);
                myExpectedMatrix.makeZRotating(T(PI));
                ENSURE_MSG(almostEqual(myMatrix, myExpectedMatrix), "Testing build matrix from quaternion");
                ENSURE_MSG(myMatrix.getType() == ROTATING, "Test getType()");
            }
            {
                Quaternion<T> myQuat(Vector3<T>(5.0, 0.0, 0.0), float(0.123));
                Matrix4<T> myMatrix(myQuat);
                myExpectedMatrix.makeXRotating(T(0.123));
                ENSURE_MSG(almostEqual(myMatrix, myExpectedMatrix), "Testing build matrix from quaternion");
            }
            {
                Quaternion<T> myQuat(Vector3<T>(0.0, 4.0, 0.0), float(0.432));
                Matrix4<T> myMatrix(myQuat);
                myExpectedMatrix.makeYRotating(T(0.432));
                ENSURE_MSG(almostEqual(myMatrix, myExpectedMatrix), "Testing build matrix from quaternion");
            }
            {
                Quaternion<T> myQuat(Vector3<T>(0.0, 0.0, 3.0), float(0.555));
                Matrix4<T> myMatrix(myQuat);
                myExpectedMatrix.makeZRotating(T(0.555));
                ENSURE_MSG(almostEqual(myMatrix, myExpectedMatrix), "Testing build matrix from quaternion");
            }
            {
                Quaternion<T> myQuat(Vector3<T>(1.0, 2.0, 3.0), float(0.432));
                myExpectedMatrix.makeRotating(normalized(Vector3<T>(1.0, 2.0, 3.0)), T(0.432));
                Matrix4<T> myMatrix(myQuat);

                Vector4<T> myPoint(1.0, 2.0, 3.0, 1.0);
                Vector4<T> myResult1 = myPoint * myMatrix;
                Vector4<T> myResult2 = myPoint * myExpectedMatrix;
                ENSURE_MSG(almostEqual(myMatrix, myExpectedMatrix), "Testing build matrix from quaternion");
                ENSURE(almostEqual(myResult1, myResult2));

                myQuat = myQuat * myQuat;
                Matrix4<T> myMatrix2(myQuat);
                myExpectedMatrix.makeRotating(normalized(Vector3<T>(1.0, 2.0, 3.0)), float(0.864));
                ENSURE_MSG(almostEqual(myMatrix2, myExpectedMatrix), "Testing concatinated rotations");
            }
        }

        void testStaticIdentity() {
            const Matrix4<T> & myIdentity = Matrix4<T>::Identity();
            ENSURE(myIdentity.getType() == IDENTITY);
            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();
            ENSURE(almostEqual(myMatrix, myIdentity));
            Matrix4<T> myOtherIdentity = Matrix4<T>::Identity();
            ENSURE(almostEqual(myMatrix, myOtherIdentity));
        }

        void testAssign() {
            Matrix4<T> myMatrix;
            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, LINEAR);
            for (unsigned i = 0; i < 4; ++i) {
                Vector4<T> myExpectedResult(T(i * 4), T(i * 4 + 1),
                    T(i * 4 + 2), T(i * 4 + 3));
                ENSURE_MSG(almostEqual(myMatrix[i], myExpectedResult),
                    "Testing assign");
            }

            ENSURE_MSG(myMatrix.getType() == LINEAR, "Testing assign");
        }

        void setMatrix(Matrix4<T> & a, const Matrix4<T> & b) {
            a.assign(b[0][0], b[0][1], b[0][2], b[0][3], b[1][0], b[1][1], b[1][2], b[1][3],
                     b[2][0], b[2][1], b[2][2], b[2][3], b[3][0], b[3][1], b[3][2], b[3][3]);
        }

        void testClassify() {
            Matrix4<T> myMatrix;
            Matrix4<T> myGeneratedMatrix;
            // Identity
            myMatrix.assign(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing auto classification: identity");

            // X_ROTATING
            myGeneratedMatrix.makeXRotating(T(0.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == X_ROTATING, "Testing auto classification: X_ROTATING");
            myGeneratedMatrix.makeXRotating(T(5.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == X_ROTATING, "Testing auto classification: X_ROTATING");
            myGeneratedMatrix.makeXRotating(T(-0.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == X_ROTATING, "Testing auto classification: X_ROTATING");
            myGeneratedMatrix.makeXRotating(T(-5.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == X_ROTATING, "Testing auto classification: X_ROTATING");

            // Y_ROTATING
            myGeneratedMatrix.makeYRotating(T(0.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == Y_ROTATING, "Testing auto classification: Y_ROTATING");
            myGeneratedMatrix.makeYRotating(T(4.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == Y_ROTATING, "Testing auto classification: Y_ROTATING");
            myGeneratedMatrix.makeYRotating(T(-1.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == Y_ROTATING, "Testing auto classification: Y_ROTATING");
            myGeneratedMatrix.makeYRotating(T(-4.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == Y_ROTATING, "Testing auto classification: Y_ROTATING");

            // Z_ROTATING
            myGeneratedMatrix.makeZRotating(T(2.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == Z_ROTATING, "Testing auto classification: Z_ROTATING");
            myGeneratedMatrix.makeZRotating(T(6.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == Z_ROTATING, "Testing auto classification: Z_ROTATING");
            myGeneratedMatrix.makeZRotating(T(-2.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == Z_ROTATING, "Testing auto classification: Z_ROTATING");
            myGeneratedMatrix.makeZRotating(T(-6.123));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == Z_ROTATING, "Testing auto classification: Z_ROTATING");

            // ROTATING
            myGeneratedMatrix.makeRotating(normalized(Vector3<T>(T(0.123), T(1.23), T(2.34))), T(4.321));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == ROTATING, "Testing auto classification: ROTATING");
            myGeneratedMatrix.makeRotating(normalized(Vector3<T>(T(2.123), T(-1.23), T(0))), T(1.321));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == ROTATING, "Testing auto classification: ROTATING");
            myGeneratedMatrix.makeRotating(normalized(Vector3<T>(T(0), T(-1.23), T(-1))), T(-4.321));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == ROTATING, "Testing auto classification: ROTATING");
            myGeneratedMatrix.makeRotating(normalized(Vector3<T>(T(0), T(-1.23), T(-1))), T(-4.321));
            myGeneratedMatrix.scale(Vector3<T>(T(1.5), T(0.5), T(1.414)));
            setMatrix(myMatrix, myGeneratedMatrix);
            ENSURE_MSG(myMatrix.getType() == LINEAR, "Testing auto classification: LINEAR");

            // SCALING
            myMatrix.assign(-1,0,0,0,0,2,0,0,0,0,0.5,0,0,0,0,1);
            ENSURE_MSG(myMatrix.getType() == SCALING, "Testing auto classification: SCALING");

            // LINEAR
            myMatrix.assign(0,1,2,0,4,5,6,0,8,9,10,0,0,0,0,1);
            ENSURE_MSG(myMatrix.getType() == LINEAR, "Testing auto classification: LINEAR");

            // TRANSLATING
            myMatrix.assign(1,0,0,0,0,1,0,0,0,0,1,0,-2,T(-5.1),T(0.8),1);
            ENSURE_MSG(myMatrix.getType() == TRANSLATING, "Testing auto classification: TRANSLATING");

            // AFFINE
            myMatrix.assign(0,1,2,0,4,5,6,0,8,9,10,0,12,13,14,1);
            ENSURE_MSG(myMatrix.getType() == AFFINE, "Testing auto classification: AFFINE");

            // UNKNOWN
            myMatrix.assign(0,1,2,110,4,5,6,20,8,9,10,23,12,13,14,16);
            ENSURE_MSG(myMatrix.getType() == UNKNOWN, "Testing auto classification: UNKNOWN");
        }

        void testGetData() {
            Matrix4<T> myMatrix;
            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, UNKNOWN);
            const T * myData = myMatrix.getData();

            for (unsigned i = 0; i < 16; ++i) {
                // Introducing this stream output circumvents a
                // GCC optimizer bug (or so i believe) in:
                //
                //   gcc (GCC) 4.2.4 (Ubuntu 4.2.4-1ubuntu3)
                //
                // The bug showed itself by making the equality
                // test work only for the first four items and
                // only occured in the OPT build. If you are
                // reading this and it is no longer 2008, please
                // try running the test without this hack and
                // remove it if things are fine afterwards. -IA
#ifdef __GNUC__
                cout << "Comparing " << i << " and " << myData[i] << " for almost-equality..." << endl;
#endif
                ENSURE_MSG(almostEqual(myData[i], i), "Testing getData");
            }
        }

        void testRowColumnAccess() {
            Matrix4<T> myMatrix;
            for (unsigned i = 0; i < 4; ++i) {
                myMatrix.assignRow(i, Vector4<T>(1,2,3,4));
                ENSURE_MSG(myMatrix.getType() == UNKNOWN, "Testing assignRow()");
                ENSURE_MSG(almostEqual(myMatrix.getRow(i), Vector4<T>(1,2,3,4)), "Testing get/assignRow()");
                myMatrix.makeIdentity();

                myMatrix.assignColumn(i, Vector4<T>(5,6,7,8));
                if (i != 3) {
                    ENSURE_MSG(myMatrix.getType() == AFFINE, "Testing assignColumn()");
                } else {
                    ENSURE_MSG(myMatrix.getType() == UNKNOWN, "Testing assignColumn()");
                }
                ENSURE_MSG(almostEqual(myMatrix.getColumn(i), Vector4<T>(5,6,7,8)), "Testing get/assignColumn()");
                myMatrix.makeIdentity();
            }
        }

        void testGetType() {
            Matrix4<T> myMatrix;
            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, IDENTITY);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing getType()");
            ENSURE_MSG(myMatrix.getTypeString() == "identity", "Testing getTypeString()");

            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, X_ROTATING);
            ENSURE_MSG(myMatrix.getType() == X_ROTATING, "Testing getType()");
            ENSURE_MSG(myMatrix.getTypeString() == "x_rotating", "Testing getTypeString()");

            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, Y_ROTATING);
            ENSURE_MSG(myMatrix.getType() == Y_ROTATING, "Testing getType()");
            ENSURE_MSG(myMatrix.getTypeString() == "y_rotating", "Testing getTypeString()");

            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, Z_ROTATING);
            ENSURE_MSG(myMatrix.getType() == Z_ROTATING, "Testing getType()");
            ENSURE_MSG(myMatrix.getTypeString() == "z_rotating", "Testing getTypeString()");

            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, ROTATING);
            ENSURE_MSG(myMatrix.getType() == ROTATING, "Testing getType()");
            ENSURE_MSG(myMatrix.getTypeString() == "rotating", "Testing getTypeString()");

            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, SCALING);
            ENSURE_MSG(myMatrix.getType() == SCALING, "Testing getType()");
            ENSURE_MSG(myMatrix.getTypeString() == "scaling", "Testing getTypeString()");

            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, LINEAR);
            ENSURE_MSG(myMatrix.getType() == LINEAR, "Testing getType()");
            ENSURE_MSG(myMatrix.getTypeString() == "linear", "Testing getTypeString()");

            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, TRANSLATING);
            ENSURE_MSG(myMatrix.getType() == TRANSLATING, "Testing getType()");
            ENSURE_MSG(myMatrix.getTypeString() == "translating", "Testing getTypeString()");

            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, AFFINE);
            ENSURE_MSG(myMatrix.getType() == AFFINE, "Testing getType()");
            ENSURE_MSG(myMatrix.getTypeString() == "affine", "Testing getTypeString()");

            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, UNKNOWN);
            ENSURE_MSG(myMatrix.getType() == UNKNOWN, "Testing getType()");
            ENSURE_MSG(myMatrix.getTypeString() == "unknown", "Testing getTypeString()");
        }

        void testMakeIdentityScalingTranslating() {
            {
                Matrix4<T> myMatrix;
                myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, UNKNOWN);
                myMatrix.makeIdentity();
                ENSURE_MSG(myMatrix.getType() == IDENTITY, "Test makeIdentity()");
                Matrix4<T> myIdentity;
                myIdentity.assign(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1, IDENTITY);
                ENSURE_MSG(almostEqual(myMatrix, myIdentity), "Test makeIdentity()");
            }
            {
                Matrix4<T> myMatrix;
                myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, UNKNOWN);
                myMatrix.makeScaling(Vector3<T>(-1, 2, 3));
                ENSURE_MSG(myMatrix.getType() == SCALING, "Test makeScaling()");
                Matrix4<T> myScaling;
                myScaling.assign(-1,0,0,0,0,2,0,0,0,0,3,0,0,0,0,1, TRANSLATING);
                ENSURE_MSG(almostEqual(myMatrix, myScaling), "Test makeScaling()");
            }
            {
                Matrix4<T> myMatrix;
                myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, UNKNOWN);
                myMatrix.makeTranslating(Vector3<T>(3, 2, -1));
                ENSURE_MSG(myMatrix.getType() == TRANSLATING, "Test makeTranslating()");
                Matrix4<T> myTranslating;
                myTranslating.assign(1,0,0,0,0,1,0,0,0,0,1,0,3,2,-1,1, TRANSLATING);
                ENSURE_MSG(almostEqual(myMatrix, myTranslating), "Test makeTranslating()");
            }
        }

        void testRotation() {
            Matrix4<T> myIdentity;
            myIdentity.makeIdentity();

            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();
            myMatrix.rotateX(0);
            ENSURE(almostEqual(myMatrix, myIdentity));
            myMatrix.rotateY(0);
            ENSURE(almostEqual(myMatrix, myIdentity));
            myMatrix.rotateZ(0);
            ENSURE(almostEqual(myMatrix, myIdentity));

            myMatrix.makeIdentity();
            myMatrix.rotateX(T(2 * PI));
            ENSURE(almostEqual(myMatrix, myIdentity));

            myMatrix.makeIdentity();
            myMatrix.rotateY(T(2 * PI));
            ENSURE(almostEqual(myMatrix, myIdentity));

            myMatrix.makeIdentity();
            myMatrix.rotateZ(T(2 * PI));
            ENSURE(almostEqual(myMatrix, myIdentity));

            myMatrix.makeIdentity();
            myMatrix.rotateX(T(PI));
            myMatrix.rotateX(T(PI));
            ENSURE(almostEqual(myMatrix, myIdentity));

            myMatrix.makeIdentity();
            myMatrix.rotateY(T(PI));
            myMatrix.rotateY(T(PI));
            ENSURE(almostEqual(myMatrix, myIdentity));

            myMatrix.makeIdentity();
            myMatrix.rotateZ(T(PI));
            myMatrix.rotateZ(T(PI));
            ENSURE(almostEqual(myMatrix, myIdentity));

            myMatrix.makeIdentity();
            myMatrix.rotateZ(T(PI));
            myMatrix.rotateX(T(PI));
            myMatrix.rotateY(T(PI));
            ENSURE(almostEqual(myMatrix, myIdentity));

            Matrix4<T> myExpectedResult;
            myExpectedResult.makeIdentity();

            myMatrix.makeXRotating(0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Test getType()");
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeXRotating");
            myMatrix.makeXRotating(T(2.0 * PI));
            ENSURE_MSG(myMatrix.getType() == X_ROTATING, "Test getType()");
            ENSURE(almostEqual(myMatrix, myExpectedResult));
            myMatrix.makeXRotating(T(PI_4));
            myExpectedResult.rotateX(T(PI_4));
            ENSURE(almostEqual(myMatrix, myExpectedResult));
            myExpectedResult.makeIdentity();

            myMatrix.makeYRotating(0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Test getType()");
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeYRotating");
            myMatrix.makeYRotating(T(2.0 * PI));
            ENSURE_MSG(myMatrix.getType() == Y_ROTATING, "Test getType()");
            ENSURE(almostEqual(myMatrix, myExpectedResult));
            myMatrix.makeYRotating(T(PI_4));
            myExpectedResult.rotateY(T(PI_4));
            ENSURE(almostEqual(myMatrix, myExpectedResult));
            myExpectedResult.makeIdentity();

            myMatrix.makeZRotating(0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Test getType()");
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeZRotating");
            myMatrix.makeZRotating(T(2.0 * PI));
            ENSURE_MSG(myMatrix.getType() == Z_ROTATING, "Test getType()");
            ENSURE(almostEqual(myMatrix, myExpectedResult));
            myMatrix.makeZRotating(T(PI_4));
            myExpectedResult.rotateZ(T(PI_4));
            ENSURE(almostEqual(myMatrix, myExpectedResult));
            myExpectedResult.makeIdentity();

            myMatrix.makeRotating(Vector3<T>(1, 0, 0), T(0.123));
            ENSURE_MSG(myMatrix.getType() == ROTATING, "Test getType()");
            myExpectedResult.makeXRotating(T(0.123));
            ENSURE_MSG(myMatrix.getType() == ROTATING, "Test getType()");
            ENSURE_MSG(almostEqual(myMatrix, myExpectedResult), "Testing makeRotating");
            myMatrix.makeRotating(Vector3<T>(1.0, 2.0, 3.0), T(2 * PI));
            myExpectedResult.makeRotating(Vector3<T>(1.0, 2.0, 3.0), 0);
            ENSURE(almostEqual(myMatrix, myExpectedResult));

            myMatrix.makeXYZRotating(Vector3<T>(1,2,3));
            ENSURE_MSG(myMatrix.getType() == ROTATING, "Test getType()");
        }

        void testRotateXYZ() {
            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();
            myMatrix.rotateXYZ(Vector3<T>(2,3,4));
            Matrix4<T> myExpectedResult;
            myExpectedResult.makeIdentity();
            myExpectedResult.rotateX(2);
            myExpectedResult.rotateY(3);
            myExpectedResult.rotateZ(4);
            ENSURE(almostEqual(myMatrix, myExpectedResult));
            ENSURE_MSG(myMatrix.getType() == ROTATING, "Test getType()");
        }

        void testScaleHelper(Matrix4<T> & theMatrix1, MatrixType theType) {
            Matrix4<T> myMatrix2;
            Matrix4<T> myMatrix3;
            myMatrix2.assign(theMatrix1);
            theMatrix1.scale(Vector3<T>(T(123.456), T(-2.457), T(0.00123)));
            myMatrix3.makeScaling(Vector3<T>(T(123.456), T(-2.457), T(0.00123)));
            myMatrix2.postMultiply(myMatrix3);
            ENSURE_MSG(theMatrix1.getType() == theType,
                (string("Test scale resulttype with ") + theMatrix1.getTypeString()).c_str());
            ENSURE_MSG(almostEqual(theMatrix1, myMatrix2),
                (string("Test scale result with ") + theMatrix1.getTypeString()).c_str());
        }

        void testScale() {
            Matrix4<T> myMatrix1;

            myMatrix1.makeIdentity();
            testScaleHelper(myMatrix1, SCALING);
            myMatrix1.makeScaling(Vector3<T>(3, 2, 1));
            testScaleHelper(myMatrix1, SCALING);
            myMatrix1.makeXRotating(T(1.234));
            testScaleHelper(myMatrix1, LINEAR);
            myMatrix1.makeYRotating(T(1.234));
            testScaleHelper(myMatrix1, LINEAR);
            myMatrix1.makeZRotating(T(1.234));
            testScaleHelper(myMatrix1, LINEAR);
            myMatrix1.makeRotating(Vector3<T>(2,3,4), T(1.234));
            testScaleHelper(myMatrix1, LINEAR);
            myMatrix1.makeTranslating(Vector3<T>(-2,-3,4));
            testScaleHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,0,0,0,1,LINEAR);
            testScaleHelper(myMatrix1, LINEAR);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,1,2,3,1,AFFINE);
            testScaleHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,6,4,5,6,7,7,8,9,8,1,2,3,19,UNKNOWN);
            testScaleHelper(myMatrix1, UNKNOWN);
        }

        void testTranslateHelper(Matrix4<T> & theMatrix1, MatrixType theType) {
            Matrix4<T> myMatrix2;
            Matrix4<T> myMatrix3;
            myMatrix2.assign(theMatrix1);
            theMatrix1.translate(Vector3<T>(T(123.456), T(-2.457), T(0.00123)));
            myMatrix3.makeTranslating(Vector3<T>(T(123.456), T(-2.457), T(0.00123)));
            myMatrix2.postMultiply(myMatrix3);
            ENSURE_MSG(theMatrix1.getType() == theType,
                (string("Test translate resulttype with ") + theMatrix1.getTypeString()).c_str());
            ENSURE_MSG(almostEqual(theMatrix1, myMatrix2),
                (string("Test translate result with ") + theMatrix1.getTypeString()).c_str());
        }

        void testTranslate() {
            Matrix4<T> myMatrix1;

            myMatrix1.makeIdentity();
            testTranslateHelper(myMatrix1, TRANSLATING);
            myMatrix1.makeScaling(Vector3<T>(3, 2, 1));
            testTranslateHelper(myMatrix1, AFFINE);
            myMatrix1.makeXRotating(T(1.234));
            testTranslateHelper(myMatrix1, AFFINE);
            myMatrix1.makeYRotating(T(1.234));
            testTranslateHelper(myMatrix1, AFFINE);
            myMatrix1.makeZRotating(T(1.234));
            testTranslateHelper(myMatrix1, AFFINE);
            myMatrix1.makeRotating(Vector3<T>(2,3,4), T(1.234));
            testTranslateHelper(myMatrix1, AFFINE);
            myMatrix1.makeTranslating(Vector3<T>(-2,-3,4));
            testTranslateHelper(myMatrix1, TRANSLATING);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,0,0,0,1,LINEAR);
            testTranslateHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,1,2,3,1,AFFINE);
            testScaleHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,6,4,5,6,7,7,8,9,8,1,2,3,19,UNKNOWN);
            testScaleHelper(myMatrix1, UNKNOWN);
        }

        void testEasyMatrixAccess() {
            Matrix4<T> myMatrix;

            // setTranslation() / getTranslation()
            myMatrix.makeIdentity();
            myMatrix.setTranslation(Vector3<T>(1,2,3));
            ENSURE_MSG(almostEqual(myMatrix.getRow(3), Vector4<T>(1,2,3,1)), "Test setTranslation()");
            ENSURE_MSG(almostEqual(myMatrix.getTranslation(), Vector3<T>(1,2,3)), "Test getTranslation()");
            ENSURE_MSG(myMatrix.getType() == TRANSLATING, "Test setTranslation() classification");
            myMatrix.setTranslation(Vector3<T>(3,2,1));
            ENSURE_MSG(myMatrix.getType() == TRANSLATING, "Test setTranslation() classification");
            myMatrix.makeScaling(Vector3<T>(2,2,2));
            myMatrix.setTranslation(Vector3<T>(3,2,1));
            ENSURE_MSG(myMatrix.getType() == AFFINE, "Test setTranslation() classification");
            myMatrix.assign(1,2,3,6,4,5,6,7,7,8,9,8,1,2,3,19,UNKNOWN);
            myMatrix.setTranslation(Vector3<T>(3,2,1));
            ENSURE_MSG(myMatrix.getType() == UNKNOWN, "Test setTranslation() classification");

            // setScale() / getScale()
            myMatrix.makeIdentity();
            myMatrix.setScale(Vector3<T>(1,2,3));
            Matrix4<T> myReference;
            myReference.assign(1,0,0,0,0,2,0,0,0,0,3,0,0,0,0,1);
            ENSURE_MSG(almostEqual(myMatrix, myReference), "Test setScale()");
            ENSURE_MSG(almostEqual(myMatrix.getScale(), Vector3<T>(1,2,3)), "Test getScale()");
            ENSURE_MSG(myMatrix.getType() == SCALING, "Test setScale() classification");
            myMatrix.setScale(Vector3<T>(3,2,1));
            ENSURE_MSG(myMatrix.getType() == SCALING, "Test setScale() classification");
            myMatrix.makeTranslating(Vector3<T>(2,2,2));
            myMatrix.setScale(Vector3<T>(3,2,1));
            ENSURE_MSG(myMatrix.getType() == AFFINE, "Test setScale() classification");
            myMatrix.makeTranslating(Vector3<T>(2,2,2));
            myMatrix.rotateX(1);
            myMatrix.setScale(Vector3<T>(3,2,1));
            ENSURE_MSG(myMatrix.getType() == AFFINE, "Test setScale() classification");
            myMatrix.assign(1,2,3,6,4,5,6,7,7,8,9,8,1,2,3,19,UNKNOWN);
            myMatrix.setScale(Vector3<T>(3,2,1));
            ENSURE_MSG(myMatrix.getType() == UNKNOWN, "Test setScale() classification");
            myMatrix.makeYRotating(1);
            myMatrix.setScale(Vector3<T>(3,2,1));
            ENSURE_MSG(myMatrix.getType() == LINEAR, "Test setScale() classification");
        }

        void testRotateXHelper(Matrix4<T> & theMatrix1, MatrixType theType) {
            Matrix4<T> myMatrix2;
            Matrix4<T> myMatrix3;
            myMatrix2.assign(theMatrix1);
            theMatrix1.rotateX(T(3.123));
            myMatrix3.makeXRotating(T(3.123));
            myMatrix2.postMultiply(myMatrix3);
            ENSURE_MSG(theMatrix1.getType() == theType,
                (string("Test rotateX resulttype with ") + theMatrix1.getTypeString()).c_str());
            ENSURE_MSG(almostEqual(theMatrix1, myMatrix2),
                (string("Test rotateX result with ") + theMatrix1.getTypeString()).c_str());
        }

        void testRotateX() {
            Matrix4<T> myMatrix1;

            myMatrix1.makeIdentity();
            testRotateXHelper(myMatrix1, X_ROTATING);
            myMatrix1.makeScaling(Vector3<T>(3, 2, 1));
            testRotateXHelper(myMatrix1, LINEAR);
            myMatrix1.makeXRotating(T(1.234));
            testRotateXHelper(myMatrix1, X_ROTATING);
            myMatrix1.makeYRotating(T(1.234));
            testRotateXHelper(myMatrix1, ROTATING);
            myMatrix1.makeZRotating(T(1.234));
            testRotateXHelper(myMatrix1, ROTATING);
            myMatrix1.makeRotating(Vector3<T>(2,3,4), T(1.234));
            testRotateXHelper(myMatrix1, ROTATING);
            myMatrix1.makeTranslating(Vector3<T>(-2,-3,4));
            testRotateXHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,0,0,0,1,LINEAR);
            testRotateXHelper(myMatrix1, LINEAR);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,1,2,3,1,AFFINE);
            testRotateXHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,6,4,5,6,7,7,8,9,8,1,2,3,19,UNKNOWN);
            testRotateXHelper(myMatrix1, UNKNOWN);
        }

        void testRotateYHelper(Matrix4<T> & theMatrix1, MatrixType theType) {
            Matrix4<T> myMatrix2;
            Matrix4<T> myMatrix3;
            myMatrix2.assign(theMatrix1);
            theMatrix1.rotateY(T(3.123));
            myMatrix3.makeYRotating(T(3.123));
            myMatrix2.postMultiply(myMatrix3);
            ENSURE_MSG(theMatrix1.getType() == theType,
                (string("Test rotateY resulttype with ") + theMatrix1.getTypeString()).c_str());
            ENSURE_MSG(almostEqual(theMatrix1, myMatrix2),
                (string("Test rotateY result with ") + theMatrix1.getTypeString()).c_str());
        }

        void testRotateY() {
            Matrix4<T> myMatrix1;

            myMatrix1.makeIdentity();
            testRotateYHelper(myMatrix1, Y_ROTATING);
            myMatrix1.makeScaling(Vector3<T>(3, 2, 1));
            testRotateYHelper(myMatrix1, LINEAR);
            myMatrix1.makeXRotating(T(1.234));
            testRotateYHelper(myMatrix1, ROTATING);
            myMatrix1.makeYRotating(T(1.234));
            testRotateYHelper(myMatrix1, Y_ROTATING);
            myMatrix1.makeZRotating(T(1.234));
            testRotateYHelper(myMatrix1, ROTATING);
            myMatrix1.makeRotating(Vector3<T>(2,3,4), T(1.234));
            testRotateYHelper(myMatrix1, ROTATING);
            myMatrix1.makeTranslating(Vector3<T>(-2,-3,4));
            testRotateYHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,0,0,0,1,LINEAR);
            testRotateYHelper(myMatrix1, LINEAR);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,1,2,3,1,AFFINE);
            testRotateYHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,6,4,5,6,7,7,8,9,8,1,2,3,19,UNKNOWN);
            testRotateYHelper(myMatrix1, UNKNOWN);
        }

        void testRotateZHelper(Matrix4<T> & theMatrix1, MatrixType theType) {
            Matrix4<T> myMatrix2;
            Matrix4<T> myMatrix3;
            myMatrix2.assign(theMatrix1);
            theMatrix1.rotateZ(T(3.123));
            myMatrix3.makeZRotating(T(3.123));
            myMatrix2.postMultiply(myMatrix3);
            ENSURE_MSG(theMatrix1.getType() == theType,
                (string("Test rotateZ resulttype with ") + theMatrix1.getTypeString()).c_str());
            ENSURE_MSG(almostEqual(theMatrix1, myMatrix2),
                (string("Test rotateZ result with ") + theMatrix1.getTypeString()).c_str());
        }

        void testRotateZ() {
            Matrix4<T> myMatrix1;

            myMatrix1.makeIdentity();
            testRotateZHelper(myMatrix1, Z_ROTATING);
            myMatrix1.makeScaling(Vector3<T>(3, 2, 1));
            testRotateZHelper(myMatrix1, LINEAR);
            myMatrix1.makeXRotating(T(1.234));
            testRotateZHelper(myMatrix1, ROTATING);
            myMatrix1.makeYRotating(T(1.234));
            testRotateZHelper(myMatrix1, ROTATING);
            myMatrix1.makeZRotating(T(1.234));
            testRotateZHelper(myMatrix1, Z_ROTATING);
            myMatrix1.makeRotating(Vector3<T>(2,3,4), T(1.234));
            testRotateZHelper(myMatrix1, ROTATING);
            myMatrix1.makeTranslating(Vector3<T>(-2,-3,4));
            testRotateZHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,0,0,0,1,LINEAR);
            testRotateZHelper(myMatrix1, LINEAR);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,1,2,3,1,AFFINE);
            testRotateZHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,6,4,5,6,7,7,8,9,8,1,2,3,19,UNKNOWN);
            testRotateZHelper(myMatrix1, UNKNOWN);
        }

        void testRotateHelper(Matrix4<T> & theMatrix1, MatrixType theType) {
            Matrix4<T> myMatrix2;
            Matrix4<T> myMatrix3;
            myMatrix2.assign(theMatrix1);
            theMatrix1.rotate(Vector3<T>(-1, 2, 33), T(3.123));
            myMatrix3.makeRotating(Vector3<T>(-1, 2, 33), T(3.123));
            myMatrix2.postMultiply(myMatrix3);
            ENSURE_MSG(theMatrix1.getType() == theType,
                (string("Test rotate resulttype with ") + theMatrix1.getTypeString()).c_str());
            ENSURE_MSG(almostEqual(theMatrix1, myMatrix2),
                (string("Test rotate result with ") + theMatrix1.getTypeString()).c_str());
        }

        void testRotate() {
            Matrix4<T> myMatrix1;
            myMatrix1.makeIdentity();
            testRotateHelper(myMatrix1, ROTATING);
            myMatrix1.makeScaling(Vector3<T>(3, 2, 1));
            testRotateHelper(myMatrix1, LINEAR);
            myMatrix1.makeXRotating(T(1.234));
            testRotateHelper(myMatrix1, ROTATING);
            myMatrix1.makeYRotating(T(1.234));
            testRotateHelper(myMatrix1, ROTATING);
            myMatrix1.makeZRotating(T(1.234));
            testRotateHelper(myMatrix1, ROTATING);
            myMatrix1.makeRotating(Vector3<T>(2,3,4), T(1.234));
            testRotateHelper(myMatrix1, ROTATING);
            myMatrix1.makeTranslating(Vector3<T>(-2,-3,4));
            testRotateHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,0,0,0,1,LINEAR);
            testRotateHelper(myMatrix1, LINEAR);
            myMatrix1.assign(1,2,3,0,4,5,6,0,7,8,9,0,1,2,3,1,AFFINE);
            testRotateHelper(myMatrix1, AFFINE);
            myMatrix1.assign(1,2,3,6,4,5,6,7,7,8,9,8,1,2,3,19,UNKNOWN);
            testRotateHelper(myMatrix1, UNKNOWN);
        }

        void testPostMultiply() {
            Matrix4<T> myIdentityMatrix;
            myIdentityMatrix.makeIdentity();

            Matrix4<T> myXRotatingMatrix;
            myXRotatingMatrix.makeXRotating(T(1.234));

            Matrix4<T> myYRotatingMatrix;
            myYRotatingMatrix.makeYRotating(T(2.234));

            Matrix4<T> myZRotatingMatrix;
            myZRotatingMatrix.makeZRotating(T(5.234));

            Matrix4<T> myRotatingMatrix;
            myRotatingMatrix.makeRotating(normalized(Vector3<T>(T(0.12),T(2.1),T(1.2))), T(5.234));

            Matrix4<T> myScalingMatrix;
            myScalingMatrix.makeScaling(Vector3<T>(T(0.12),T(2.1),T(1.2)));

            Matrix4<T> myLinearMatrix;
            myLinearMatrix.assign(0,1,2,0,4,5,6,0,8,9,10,0,0,0,0,1, LINEAR);

            Matrix4<T> myTranslatingMatrix;
            myTranslatingMatrix.makeTranslating(Vector3<T>(T(3.12),T(4.1),T(5.2)));

            Matrix4<T> myAffineMatrix;
            myAffineMatrix.assign(0,1,2,0,4,5,6,0,8,9,10,0,12,13,14,1, AFFINE);

            Matrix4<T> myUnknownMatrix;
            myUnknownMatrix.assign(0,1,2,110,4,5,6,20,8,9,10,23,12,13,14,1, UNKNOWN);

            testMatrixMultiplication(myIdentityMatrix,  myIdentityMatrix, IDENTITY);

            testMatrixMultiplication(myIdentityMatrix,  myXRotatingMatrix, X_ROTATING);
            testMatrixMultiplication(myXRotatingMatrix, myXRotatingMatrix, X_ROTATING);

            testMatrixMultiplication(myIdentityMatrix,  myYRotatingMatrix, Y_ROTATING);
            testMatrixMultiplication(myXRotatingMatrix, myYRotatingMatrix, ROTATING);
            testMatrixMultiplication(myYRotatingMatrix, myYRotatingMatrix, Y_ROTATING);

            testMatrixMultiplication(myIdentityMatrix,  myZRotatingMatrix, Z_ROTATING);
            testMatrixMultiplication(myXRotatingMatrix, myZRotatingMatrix, ROTATING);
            testMatrixMultiplication(myYRotatingMatrix, myZRotatingMatrix, ROTATING);
            testMatrixMultiplication(myZRotatingMatrix, myZRotatingMatrix, Z_ROTATING);

            testMatrixMultiplication(myIdentityMatrix,  myRotatingMatrix, ROTATING);
            testMatrixMultiplication(myXRotatingMatrix, myRotatingMatrix, ROTATING);
            testMatrixMultiplication(myYRotatingMatrix, myRotatingMatrix, ROTATING);
            testMatrixMultiplication(myZRotatingMatrix, myRotatingMatrix, ROTATING);
            testMatrixMultiplication(myRotatingMatrix,  myRotatingMatrix, ROTATING);

            testMatrixMultiplication(myIdentityMatrix,  myScalingMatrix, SCALING);
            testMatrixMultiplication(myXRotatingMatrix, myScalingMatrix, LINEAR);
            testMatrixMultiplication(myYRotatingMatrix, myScalingMatrix, LINEAR);
            testMatrixMultiplication(myZRotatingMatrix, myScalingMatrix, LINEAR);
            testMatrixMultiplication(myRotatingMatrix,  myScalingMatrix, LINEAR);
            testMatrixMultiplication(myScalingMatrix,   myScalingMatrix, SCALING);

            testMatrixMultiplication(myIdentityMatrix,  myLinearMatrix, LINEAR);
            testMatrixMultiplication(myXRotatingMatrix, myLinearMatrix, LINEAR);
            testMatrixMultiplication(myYRotatingMatrix, myLinearMatrix, LINEAR);
            testMatrixMultiplication(myZRotatingMatrix, myLinearMatrix, LINEAR);
            testMatrixMultiplication(myRotatingMatrix,  myLinearMatrix, LINEAR);
            testMatrixMultiplication(myScalingMatrix,   myLinearMatrix, LINEAR);
            testMatrixMultiplication(myLinearMatrix,    myLinearMatrix, LINEAR);

            testMatrixMultiplication(myIdentityMatrix,    myTranslatingMatrix, TRANSLATING);
            testMatrixMultiplication(myXRotatingMatrix,   myTranslatingMatrix, AFFINE);
            testMatrixMultiplication(myYRotatingMatrix,   myTranslatingMatrix, AFFINE);
            testMatrixMultiplication(myZRotatingMatrix,   myTranslatingMatrix, AFFINE);
            testMatrixMultiplication(myRotatingMatrix,    myTranslatingMatrix, AFFINE);
            testMatrixMultiplication(myScalingMatrix,     myTranslatingMatrix, AFFINE);
            testMatrixMultiplication(myLinearMatrix,      myTranslatingMatrix, AFFINE);
            testMatrixMultiplication(myTranslatingMatrix, myTranslatingMatrix, TRANSLATING);

            testMatrixMultiplication(myIdentityMatrix,    myAffineMatrix, AFFINE);
            testMatrixMultiplication(myXRotatingMatrix,   myAffineMatrix, AFFINE);
            testMatrixMultiplication(myYRotatingMatrix,   myAffineMatrix, AFFINE);
            testMatrixMultiplication(myZRotatingMatrix,   myAffineMatrix, AFFINE);
            testMatrixMultiplication(myRotatingMatrix,    myAffineMatrix, AFFINE);
            testMatrixMultiplication(myScalingMatrix,     myAffineMatrix, AFFINE);
            testMatrixMultiplication(myLinearMatrix,      myAffineMatrix, AFFINE);
            testMatrixMultiplication(myTranslatingMatrix, myAffineMatrix, AFFINE);
            testMatrixMultiplication(myAffineMatrix,      myAffineMatrix, AFFINE);

            testMatrixMultiplication(myIdentityMatrix,    myUnknownMatrix, UNKNOWN);
            testMatrixMultiplication(myXRotatingMatrix,   myUnknownMatrix, UNKNOWN);
            testMatrixMultiplication(myYRotatingMatrix,   myUnknownMatrix, UNKNOWN);
            testMatrixMultiplication(myZRotatingMatrix,   myUnknownMatrix, UNKNOWN);
            testMatrixMultiplication(myRotatingMatrix,    myUnknownMatrix, UNKNOWN);
            testMatrixMultiplication(myScalingMatrix,     myUnknownMatrix, UNKNOWN);
            testMatrixMultiplication(myLinearMatrix,      myUnknownMatrix, UNKNOWN);
            testMatrixMultiplication(myTranslatingMatrix, myUnknownMatrix, UNKNOWN);
            testMatrixMultiplication(myAffineMatrix,      myUnknownMatrix, UNKNOWN);
            testMatrixMultiplication(myUnknownMatrix,     myUnknownMatrix, UNKNOWN);
        }

        void testAsterixOperator() {
            // Test Vector * Matrix
            Matrix4<T> myMatrix;
            myMatrix.assign(0,1,2,3,
                            4,5,6,7,
                            8,9,10,11,
                            12,13,14,15);
            Vector4<T> myVector(0,1,2,3);
            ENSURE_MSG(almostEqual(myVector * myMatrix, Vector4<T>(56, 62, 68, 74)),
                "Testing vector * matrix");

            myMatrix.assign(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
            myVector.assign(Vector4<T>(-1,1,-1,1));
            ENSURE_MSG(almostEqual(myVector * myMatrix, Vector4<T>(0,0,0,0)),
                "Testing vector * matrix");

            // Point * Matrix
            Point3<T> myPoint(0,1,2);
            Vector4<T> myResult;
            myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
            myResult = product(Vector4<T>(0, 1, 2, 1), myMatrix);
            myResult = myResult * (1 / (dot(Vector4<T>(0,1,2,1), myMatrix.getColumn(3))));
            ENSURE_MSG(almostEqual(myPoint * myMatrix,
                Point3<T>(myResult.begin())), "Testing point * matrix");
            myPoint = myPoint * myMatrix;

            myMatrix.assign(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
            myPoint.assign(Point3<T>(-1,1,-1));
            myResult = product(Vector4<T>(-1, 1, -1, 1), myMatrix);
            ENSURE_MSG(almostEqual(myPoint * myMatrix,
                Point3<T>(myResult.begin())), "Testing point * matrix");
        }

        void testTransposeHelper(Matrix4<T> & theMatrix, MatrixType theType) {
            Matrix4<T> myTransposed;
            myTransposed.assign(theMatrix);
            myTransposed.transpose();

            cerr << "Testing transpose for matrix type: " << theMatrix.getTypeString() << endl;
            ENSURE_MSG(almostEqual(theMatrix.getRow(0), myTransposed.getColumn(0)), "Testing row 0");
            ENSURE_MSG(almostEqual(theMatrix.getRow(1), myTransposed.getColumn(1)), "Testing row 1");
            ENSURE_MSG(almostEqual(theMatrix.getRow(2), myTransposed.getColumn(2)), "Testing row 2");
            ENSURE_MSG(almostEqual(theMatrix.getRow(3), myTransposed.getColumn(3)), "Testing row 3");

            ENSURE_MSG(myTransposed.getType() == theType, "Testing type after transpose");
        }

        void testTranspose() {
            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();
            testTransposeHelper(myMatrix, IDENTITY);

            myMatrix.makeScaling(Vector3<T>(2,3,4));
            testTransposeHelper(myMatrix, SCALING);

            myMatrix.makeTranslating(Vector3<T>(2,3,4));
            testTransposeHelper(myMatrix, UNKNOWN);

            myMatrix.makeXRotating(T(1.234));
            testTransposeHelper(myMatrix, X_ROTATING);

            myMatrix.makeYRotating(T(3.234));
            testTransposeHelper(myMatrix, Y_ROTATING);

            myMatrix.makeZRotating(T(-1.234));
            testTransposeHelper(myMatrix, Z_ROTATING);

            myMatrix.makeRotating(normalized(Vector3<T>(1,2,3)), T(-2.234));
            testTransposeHelper(myMatrix, ROTATING);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,0,11,0,0,0,0,1, LINEAR);
            testTransposeHelper(myMatrix, LINEAR);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,10,11,0,-10,20,30,1, AFFINE);
            testTransposeHelper(myMatrix, UNKNOWN);

            myMatrix.assign(1,2,3,0,15,6,7,8,9,0,11,12,13,14,15, 16, UNKNOWN);
            testTransposeHelper(myMatrix, UNKNOWN);
        }


        void testInversion() {
            Matrix4<T> myMatrix;
            Matrix4<T> myExpectedIdentity;
            myExpectedIdentity.makeIdentity();

            // test non-invertable
            myMatrix.assign(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
            Matrix4<T> myInverse(myMatrix);
            ENSURE_MSG(!myInverse.invert(), "Testing not invertable Matrix");

            // test invertable identity
            myMatrix.makeIdentity();
            ENSURE_MSG(myMatrix.invert(), "Testing if matrix is invertable");
            ENSURE_MSG(almostEqual(myMatrix, myExpectedIdentity),
                "Testing identity inversion");

            // test invertable affine
            myMatrix.makeIdentity();
            myMatrix.scale(Vector3<T>(T(123.456), T(-2.457), T(0.00123)));
            myMatrix.rotateXYZ(Vector3<T>(T(-3.1416), T(1.23), T(-1.654)));
            myMatrix.translate(Vector3<T>(T(-5), T(-10), T(6)));

            myInverse.assign(myMatrix);
            ENSURE_MSG(myInverse.invert(), "Testing if matrix is invertable");
            myInverse.postMultiply(myMatrix);
            ENSURE_MSG(almostEqual(myInverse, myExpectedIdentity),
                "Testing affine inversion");

            // test invertable full
            myMatrix.makeIdentity();
            myMatrix.assign(1,2,3,0,15,6,7,8,9,0,11,12,13,14,15, 16);
            myInverse.assign(myMatrix);
            ENSURE_MSG(myInverse.getDeterminant() == -4320,"getDeterminant()");
            ENSURE_MSG(myInverse.invert(), "Testing if matrix is invertable");
            //printMatrix(myInverse);
            myInverse.postMultiply(myMatrix);
            ENSURE_MSG(almostEqual(myInverse, myExpectedIdentity),
                "Testing full inversion");
        }

        void runInvertPerformanceTest(Matrix4<T> theMatrix) {
            asl::NanoTime myStartTime;

            for (unsigned i = 0; i < ourNumberOfMults * 4; ++i) {
                theMatrix.invert();
            }
            asl::NanoTime myInvertTime = asl::NanoTime() - myStartTime;
            cerr << "Inversion of " << theMatrix.getTypeString() << "-matrix takes: " << myInvertTime.millis() <<" ms"<< endl;
        }

        void testInvertClassifyHelper(Matrix4<T> & theMatrix) {
            if (ourPerformanceTest) {
                runInvertPerformanceTest(theMatrix);
            }

            Matrix4<T> myInverse;
            Matrix4<T> myIdentity;
            myIdentity.makeIdentity();
            myInverse.assign(theMatrix);
            ENSURE_MSG(myInverse.invert(), "Testing if matrix is invertable");

            //printMatrix(myInverse);
            ENSURE_MSG(theMatrix.getType() == myInverse.classifyMatrix(),
                (string("Testing matrix type after invert, result: ") + theMatrix.getTypeString()).c_str());
            myInverse.postMultiply(theMatrix);
            ENSURE_MSG(almostEqual(myInverse, myIdentity), "Testing inversion");
            theMatrix.makeIdentity();
        }

        // Tests classifier and inverter:
        // The matrix type should not change if the matrix gets inverted
        void testInvertClassify() {
            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();
            testInvertClassifyHelper(myMatrix);

            myMatrix.makeScaling(Vector3<T>(2,3,4));
            testInvertClassifyHelper(myMatrix);

            myMatrix.makeTranslating(Vector3<T>(2,3,4));
            testInvertClassifyHelper(myMatrix);

            myMatrix.makeXRotating(T(1.234));
            testInvertClassifyHelper(myMatrix);

            myMatrix.makeYRotating(T(3.234));
            testInvertClassifyHelper(myMatrix);

            myMatrix.makeZRotating(T(-1.234));
            testInvertClassifyHelper(myMatrix);

            myMatrix.makeRotating(normalized(Vector3<T>(1,2,3)), T(-2.234));
            testInvertClassifyHelper(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,0,11,0,0,0,0,1, LINEAR);
            testInvertClassifyHelper(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,10,11,0,-10,20,30,1, AFFINE);
            testInvertClassifyHelper(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,8,9,0,11,12,13,14,15, 16, UNKNOWN);
            testInvertClassifyHelper(myMatrix);
        }

        void testInvertPerformance(Matrix4<T> a) {

            Matrix4<T> myResult;

            std::string myOutLine = a.getTypeString() + ":\t";
            cerr << myOutLine;
            _myPerformanceString += myOutLine;

           Matrix4<T> * Ma = new Matrix4<T>[ourNumberOfMults];
           Matrix4<T> * Mb = new Matrix4<T>[ourNumberOfMults];

           for (unsigned i = 0; i < ourNumberOfMults; ++i) {
               Ma[i] = a;
               Mb[i] = a;
           }

            // dumb full test
            asl::NanoTime myFullTimer;
            for (unsigned i = 0; i < ourNumberOfMults; ++i) {
                Ma[i].invertCramer();
            }
            asl::NanoTime myFullTime = asl::NanoTime() - myFullTimer;
            double myFullCount = double(ourNumberOfMults) / double(myFullTime.millis());
            myOutLine = string(" - cramer: ") + as_string(myFullCount)+ "[T="+as_string(myFullTime.millis()) + string("]\t");
            cerr << myOutLine;
            _myPerformanceString += myOutLine;

            // smart test

            asl::NanoTime mySmartTimer;
            for (unsigned i = 0; i < ourNumberOfMults; ++i) {
                Mb[i].invert();
            }
            asl::NanoTime mySmartTime = asl::NanoTime() - mySmartTimer;
            double mySmartCount = double(ourNumberOfMults) / double(mySmartTime.millis());
            myOutLine = string(" | smart: ") + as_string(mySmartCount) +  "[T="+as_string(mySmartTime.millis()) + string("]\t");
            cerr << myOutLine;
            _myPerformanceString += myOutLine;

            if (mySmartCount < myFullCount) {
                myOutLine = string("    ### ") +
                    as_string((myFullCount/mySmartCount - 1) * 100) +
                    "% slower.\n";
            } else {
                myOutLine = string("    >>> ") +
                    as_string((mySmartCount/myFullCount - 1) * 100) +
                    "% faster.\n";
            }
            cerr << myOutLine;
            _myPerformanceString += myOutLine;
            delete [] Ma;
            delete [] Mb;
        }

        void testInvertPerformance() {
            cerr << "#### Testing invert Performance:" << endl;
            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();
            testInvertPerformance(myMatrix);

            myMatrix.makeScaling(Vector3<T>(2,3,4));
            testInvertPerformance(myMatrix);

            myMatrix.makeTranslating(Vector3<T>(2,3,4));
            testInvertPerformance(myMatrix);

            myMatrix.makeXRotating(T(1.234));
            testInvertPerformance(myMatrix);

            myMatrix.makeYRotating(T(3.234));
            testInvertPerformance(myMatrix);

            myMatrix.makeZRotating(T(-1.234));
            testInvertPerformance(myMatrix);

            myMatrix.makeRotating(normalized(Vector3<T>(1,2,3)), T(-2.234));
            testInvertPerformance(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,0,11,0,0,0,0,1, LINEAR);
            testInvertPerformance(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,10,11,0,-10,20,30,1, AFFINE);
            testInvertPerformance(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,8,9,0,11,12,13,14,15, 16, UNKNOWN);
            testInvertPerformance(myMatrix);
        }

        void testMultiplyMatrixPV(Matrix4<T> a) {
            ENSURE_MSG(almostEqual(product(Vector4<T>(2,3,5,7),a), fullproduct(Vector4<T>(2,3,5,7),a)),
                string(string("testMultiply Vector4 * ")+a.getTypeString()+" matrix").c_str());
            ENSURE_MSG(almostEqual(product(Point3<T>(2,3,5),a), fullproduct(Point3<T>(2,3,5),a)),
                string(string("testMultiply Point3 * ")+a.getTypeString()+" matrix").c_str());
#ifdef MORE_VERBOSITY
            DPRINT(product(Vector4<T>(2,3,5,7),a));
            DPRINT(fullproduct(Vector4<T>(2,3,5,7),a);
            DPRINT(product(Point3<T>(2,3,5),a));
            DPRINT(fullproduct(Point3<T>(2,3,5),a));
#endif
        }
        void testMultiplyMatrixPV() {
            cerr << "#### Testing Multiply Point/Vector * Matrix:" << endl;
            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();
            testMultiplyMatrixPV(myMatrix);

            myMatrix.makeScaling(Vector3<T>(2,3,4));
            testMultiplyMatrixPV(myMatrix);

            myMatrix.makeTranslating(Vector3<T>(2,3,4));
            testMultiplyMatrixPV(myMatrix);

            myMatrix.makeXRotating(T(1.234));
            testMultiplyMatrixPV(myMatrix);

            myMatrix.makeYRotating(T(3.234));
            testMultiplyMatrixPV(myMatrix);

            myMatrix.makeZRotating(T(-1.234));
            testMultiplyMatrixPV(myMatrix);

            myMatrix.makeRotating(normalized(Vector3<T>(1,2,3)), T(-2.234));
            testMultiplyMatrixPV(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,0,11,0,0,0,0,1, LINEAR);
            testMultiplyMatrixPV(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,10,11,0,-10,20,30,1, AFFINE);
            testMultiplyMatrixPV(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,8,9,0,11,12,13,14,15, 16, UNKNOWN);
            testMultiplyMatrixPV(myMatrix);
        }

        void testMultiplyPerformance(Matrix4<T> a) {

            std::string myOutLine = a.getTypeString() + ":\t";
            cerr << myOutLine;
            _myPerformanceString += myOutLine;

           Matrix4<T> * Ma = new Matrix4<T>[ourNumberOfMults];
           Vector4<T> * Va = new Vector4<T>[ourNumberOfMults];
           Vector4<T> * Vb = new Vector4<T>[ourNumberOfMults];

           for (unsigned i = 0; i < ourNumberOfMults; ++i) {
               Ma[i] = a;
               Va[i] = Vector4<T>(1,2,3,4);
           }

            // dumb full test
            asl::NanoTime myFullTimer;
            for (unsigned i = 0; i < ourNumberOfMults; ++i) {
                Vb[i] = fullproduct(Va[i], Ma[i]);
            }
            asl::NanoTime myFullTime = asl::NanoTime() - myFullTimer;
            double myFullCount = double(ourNumberOfMults) / double(myFullTime.millis());
            myOutLine = string(" - full: ") + as_string(myFullCount)+ "[T="+as_string(myFullTime.millis()) + string("]\t");
            cerr << myOutLine;
            _myPerformanceString += myOutLine;

            // smart test

            asl::NanoTime mySmartTimer;
            for (unsigned i = 0; i < ourNumberOfMults; ++i) {
                Vb[i] = product(Va[i], Ma[i]);
            }
            asl::NanoTime mySmartTime = asl::NanoTime() - mySmartTimer;
            double mySmartCount = double(ourNumberOfMults) / double(mySmartTime.millis());
            myOutLine = string(" | smart: ") + as_string(mySmartCount) +  "[T="+as_string(mySmartTime.millis()) + string("]\t");
            cerr << myOutLine;
            _myPerformanceString += myOutLine;

            if (mySmartCount < myFullCount) {
                myOutLine = string("    ### ") +
                    as_string((myFullCount/mySmartCount - 1) * 100) +
                    "% slower.\n";
            } else {
                myOutLine = string("    >>> ") +
                    as_string((mySmartCount/myFullCount - 1) * 100) +
                    "% faster.\n";
            }
            cerr << myOutLine;
            _myPerformanceString += myOutLine;
            delete [] Ma;
            delete [] Va;
            delete [] Vb;
        }

        void testMultiplyPerformance() {
            cerr << "#### Testing Multiply Performance:" << endl;
            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();
            testMultiplyPerformance(myMatrix);

            myMatrix.makeScaling(Vector3<T>(2,3,4));
            testMultiplyPerformance(myMatrix);

            myMatrix.makeTranslating(Vector3<T>(2,3,4));
            testMultiplyPerformance(myMatrix);

            myMatrix.makeXRotating(T(1.234));
            testMultiplyPerformance(myMatrix);

            myMatrix.makeYRotating(T(3.234));
            testMultiplyPerformance(myMatrix);

            myMatrix.makeZRotating(T(-1.234));
            testMultiplyPerformance(myMatrix);

            myMatrix.makeRotating(normalized(Vector3<T>(1,2,3)), T(-2.234));
            testMultiplyPerformance(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,0,11,0,0,0,0,1, LINEAR);
            testMultiplyPerformance(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,10,11,0,-10,20,30,1, AFFINE);
            testMultiplyPerformance(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,8,9,0,11,12,13,14,15, 16, UNKNOWN);
            testMultiplyPerformance(myMatrix);
        }

        struct DecomposedMatrix {
            Vector3<T> scale;
            Vector3<T> orientation;
            Vector3<T> position;
        };

        void testDecompositionHelper(const Matrix4<T> & theMatrix,
                                     DecomposedMatrix & theResult)
        {
            Vector3<T> myScale;
            Vector3<T> myShear;
            Vector3<T> myOrientation;
            Vector3<T> myPosition;
            theMatrix.decompose(myScale, myShear, myOrientation, myPosition);
            ENSURE_MSG(almostEqual(myShear, Vector3<T>(0,0,0)),
                (string("Matrix decomposition (shear), type: " + theMatrix.getTypeString())).c_str());
            ENSURE_MSG(almostEqual(myScale, theResult.scale),
                (string("Matrix decomposition (scale), type: " + theMatrix.getTypeString())).c_str());
            ENSURE_MSG(almostEqual(myPosition, theResult.position),
                (string("Matrix decomposition (position), type: " + theMatrix.getTypeString())).c_str());

            ENSURE_MSG(rotationIsEquivalent(myOrientation, theResult.orientation),
                (string("Matrix decomposition (orientation), type: " + theMatrix.getTypeString())).c_str());

            // Clear result
            theResult.scale       = Vector3<T>(1,1,1);
            theResult.orientation = Vector3<T>(0,0,0);
            theResult.position    = Vector3<T>(0,0,0);
        }

        bool
        rotationIsEquivalent(Vector3<T> a, Vector3<T> b) {
            Matrix4<T> myMatrix1;
            myMatrix1.makeXRotating(a[0]);
            myMatrix1.rotateY(a[1]);
            myMatrix1.rotateZ(a[2]);
            Matrix4<T> myMatrix2;
            myMatrix2.makeXRotating(b[0]);
            myMatrix2.rotateY(b[1]);
            myMatrix2.rotateZ(b[2]);
            return almostEqual(Point3<T>(1,1,1) * myMatrix1, Point3<T>(1,1,1) * myMatrix2);
        }

        void testMatrixDecomposition() {
            DecomposedMatrix myResult;
            myResult.scale       = Vector3<T>(1,1,1);
            myResult.orientation = Vector3<T>(0,0,0);
            myResult.position    = Vector3<T>(0,0,0);

            // Test identity
            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();
            testDecompositionHelper(myMatrix, myResult);

            // Test translating only
            myResult.position = Vector3<T>(1,2,3);
            myMatrix.makeTranslating(myResult.position);
            testDecompositionHelper(myMatrix, myResult);

            // Test scaling only
            myResult.scale = Vector3<T>(3,2,1);
            myMatrix.makeScaling(myResult.scale);
            testDecompositionHelper(myMatrix, myResult);

            // Test rotation only
            myMatrix.makeXRotating(T(PI_4));
            myResult.orientation = Vector3<T>(T(PI_4),0,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeYRotating(T(PI_4));
            myResult.orientation = Vector3<T>(0,T(PI_4),0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeZRotating(T(PI_4));
            myResult.orientation = Vector3<T>(0,0,T(PI_4));
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeXRotating(1);
            myResult.orientation = Vector3<T>(1,0,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeYRotating(-1);
            myResult.orientation = Vector3<T>(0,-1,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeZRotating(4);
            myResult.orientation = Vector3<T>(0,0,4);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeXRotating(T(5.123));
            myResult.orientation = Vector3<T>(T(5.123),0,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeYRotating(-5);
            myResult.orientation = Vector3<T>(0,-5,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeZRotating(0);
            myResult.orientation = Vector3<T>(0,0,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeXRotating(312);
            myResult.orientation = Vector3<T>(312,0,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeYRotating(T(-0.111));
            myResult.orientation = Vector3<T>(0,T(-0.111),0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeZRotating(T(PI));
            myResult.orientation = Vector3<T>(0,0,T(PI));
            testDecompositionHelper(myMatrix, myResult);

            // Test composition of rotations
            myResult.orientation = Vector3<T>(1,2,3);
            myMatrix.makeXYZRotating(myResult.orientation);
            testDecompositionHelper(myMatrix, myResult);

            myResult.orientation = Vector3<T>(-1,0,-3);
            myMatrix.makeXYZRotating(myResult.orientation);
            testDecompositionHelper(myMatrix, myResult);

            // Test composition of scale and translation
            myResult.scale = Vector3<T>(3,2,1);
            myResult.position = Vector3<T>(-4,-5,6);
            myMatrix.makeScaling(myResult.scale);
            myMatrix.translate(myResult.position);
            testDecompositionHelper(myMatrix, myResult);

            // Test composition of rotation and translation
            myResult.orientation = Vector3<T>(10,0,0);
            myResult.position = Vector3<T>(-1,-2,-3);
            myMatrix.makeXRotating(10);
            myMatrix.translate(myResult.position);
            testDecompositionHelper(myMatrix, myResult);

            // Test composition of rotation and scale
            myResult.scale = Vector3<T>(3,2,1);
            myResult.orientation = Vector3<T>(-5,0,0);
            myMatrix.makeScaling(myResult.scale);
            myMatrix.rotateX(-5);
            testDecompositionHelper(myMatrix, myResult);

            // [CH] This does not work, yet - sorry

            myResult.scale = Vector3<T>(-3,-2,-1);
            myResult.orientation = Vector3<T>(1,0,0);
            myMatrix.makeScaling(myResult.scale);
            myMatrix.rotateX(1);
            testDecompositionHelper(myMatrix, myResult);

/*
            myResult.scale = Vector3<T>(3,2,1);
            myResult.orientation = Vector3<T>(3,4,5);
            myMatrix.makeScaling(myResult.scale);
            myMatrix.rotateHPR(Vector3<T>(4,3,5));
            testDecompositionHelper(myMatrix, myResult);

            // Test composition of scale, rotation and translation
            myResult.scale = Vector3<T>(2,4,75);
            myResult.orientation = Vector3<T>(1,9,3);
            myResult.position = Vector3<T>(3,8,0);
            myMatrix.makeScaling(myResult.scale);
            myMatrix.rotateHPR(Vector3<T>(9,1,3));
            myMatrix.translate(myResult.position);
            testDecompositionHelper(myMatrix, myResult);
*/
        }

        void testGetRotationHelper(const Vector3<T> & theRotation) {
            Matrix4<T> myMatrix;
            Matrix4<T> myResultMatrix;
            Vector3<T> myResult;

            // Test XYZ rotation order
            myMatrix.makeIdentity();
            myMatrix.rotateX(theRotation[0]);
            myMatrix.rotateY(theRotation[1]);
            myMatrix.rotateZ(theRotation[2]);

            myMatrix.getRotation(myResult, Matrix4<T>::ROTATION_ORDER_XYZ);

            myResultMatrix.makeXRotating(myResult[0]);
            myResultMatrix.rotateY(myResult[1]);
            myResultMatrix.rotateZ(myResult[2]);

            ENSURE_MSG(almostEqual(myMatrix, myResultMatrix),
                (string("getRotation(), rotation: " + as_string(theRotation) +
                 " result: ") + as_string(myResult) + " order xyz").c_str());

            // Test XZY rotation order
            myMatrix.makeIdentity();
            myMatrix.rotateX(theRotation[0]);
            myMatrix.rotateZ(theRotation[2]);
            myMatrix.rotateY(theRotation[1]);

            myMatrix.getRotation(myResult, Matrix4<T>::ROTATION_ORDER_XZY);

            myResultMatrix.makeXRotating(myResult[0]);
            myResultMatrix.rotateZ(myResult[2]);
            myResultMatrix.rotateY(myResult[1]);

            ENSURE_MSG(almostEqual(myMatrix, myResultMatrix),
                (string("getRotation(), rotation: " + as_string(theRotation) +
                 " result: ") + as_string(myResult) + " order xzy").c_str());

            // Test YXZ rotation order
            myMatrix.makeIdentity();
            myMatrix.rotateY(theRotation[1]);
            myMatrix.rotateX(theRotation[0]);
            myMatrix.rotateZ(theRotation[2]);

            myMatrix.getRotation(myResult, Matrix4<T>::ROTATION_ORDER_YXZ);

            myResultMatrix.makeYRotating(myResult[1]);
            myResultMatrix.rotateX(myResult[0]);
            myResultMatrix.rotateZ(myResult[2]);

            ENSURE_MSG(almostEqual(myMatrix, myResultMatrix),
                (string("getRotation(), rotation: " + as_string(theRotation) +
                 " result: ") + as_string(myResult) + " order yxz").c_str());

            // Test YZX rotation order
            myMatrix.makeIdentity();
            myMatrix.rotateY(theRotation[1]);
            myMatrix.rotateZ(theRotation[2]);
            myMatrix.rotateX(theRotation[0]);

            myMatrix.getRotation(myResult, Matrix4<T>::ROTATION_ORDER_YZX);

            myResultMatrix.makeYRotating(myResult[1]);
            myResultMatrix.rotateZ(myResult[2]);
            myResultMatrix.rotateX(myResult[0]);

            ENSURE_MSG(almostEqual(myMatrix, myResultMatrix),
                (string("getRotation(), rotation: " + as_string(theRotation) +
                 " result: ") + as_string(myResult) + " order yzx").c_str());

            // Test ZXY rotation order
            myMatrix.makeIdentity();
            myMatrix.rotateZ(theRotation[2]);
            myMatrix.rotateX(theRotation[0]);
            myMatrix.rotateY(theRotation[1]);

            myMatrix.getRotation(myResult, Matrix4<T>::ROTATION_ORDER_ZXY);

            myResultMatrix.makeZRotating(myResult[2]);
            myResultMatrix.rotateX(myResult[0]);
            myResultMatrix.rotateY(myResult[1]);

            ENSURE_MSG(almostEqual(myMatrix, myResultMatrix),
                (string("getRotation(), rotation: " + as_string(theRotation) +
                 " result: ") + as_string(myResult) + " order zxy").c_str());

            // Test ZYX rotation order
            myMatrix.makeIdentity();
            myMatrix.rotateZ(theRotation[2]);
            myMatrix.rotateY(theRotation[1]);
            myMatrix.rotateX(theRotation[0]);

            myMatrix.getRotation(myResult, Matrix4<T>::ROTATION_ORDER_ZYX);

            myResultMatrix.makeZRotating(myResult[2]);
            myResultMatrix.rotateY(myResult[1]);
            myResultMatrix.rotateX(myResult[0]);

            ENSURE_MSG(almostEqual(myMatrix, myResultMatrix),
                (string("getRotation(), rotation: " + as_string(theRotation) +
                 " result: ") + as_string(myResult) + " order zyx").c_str());
        }

        void testGetRotation() {
            testGetRotationHelper(Vector3<T>(T(PI_4),0,0));
            testGetRotationHelper(Vector3<T>(0,T(PI_4),0));
            testGetRotationHelper(Vector3<T>(0,0,T(PI_4)));

            testGetRotationHelper(Vector3<T>(1,0,0));
            testGetRotationHelper(Vector3<T>(0,2,0));
            testGetRotationHelper(Vector3<T>(0,0,3));

            testGetRotationHelper(Vector3<T>(T(PI_4),T(PI_4),0));
            testGetRotationHelper(Vector3<T>(0,T(PI_4),T(PI_4)));
            testGetRotationHelper(Vector3<T>(T(PI_4),0,T(PI_4)));

            testGetRotationHelper(Vector3<T>(1,2,0));
            testGetRotationHelper(Vector3<T>(0,3,4));
            testGetRotationHelper(Vector3<T>(-1,0,-2));

            testGetRotationHelper(Vector3<T>(T(PI_4),T(PI_4),T(PI_4)));
            testGetRotationHelper(Vector3<T>(1,2,3));
            testGetRotationHelper(Vector3<T>(40,50,60));
            testGetRotationHelper(Vector3<T>(-40,50,-0.5));
            testGetRotationHelper(Vector3<T>(0,0,0));
/*
            myMatrix.makeYRotating(T(PI_4));
            myResult.orientation = Vector3<T>(0,T(PI_4),0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeZRotating(T(PI_4));
            myResult.orientation = Vector3<T>(0,0,T(PI_4));
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeXRotating(1);
            myResult.orientation = Vector3<T>(1,0,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeYRotating(-1);
            myResult.orientation = Vector3<T>(0,-1,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeZRotating(4);
            myResult.orientation = Vector3<T>(0,0,4);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeXRotating(T(5.123));
            myResult.orientation = Vector3<T>(T(5.123),0,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeYRotating(-5);
            myResult.orientation = Vector3<T>(0,-5,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeZRotating(0);
            myResult.orientation = Vector3<T>(0,0,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeXRotating(312);
            myResult.orientation = Vector3<T>(312,0,0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeYRotating(T(-0.111));
            myResult.orientation = Vector3<T>(0,T(-0.111),0);
            testDecompositionHelper(myMatrix, myResult);

            myMatrix.makeZRotating(T(PI));
            myResult.orientation = Vector3<T>(0,0,T(PI));
            testDecompositionHelper(myMatrix, myResult);

            // Test composition of rotations
            myResult.orientation = Vector3<T>(1,2,3);
            myMatrix.makeXYZRotating(myResult.orientation);
            testDecompositionHelper(myMatrix, myResult);

            myResult.orientation = Vector3<T>(-1,0,-3);
            myMatrix.makeXYZRotating(myResult.orientation);
            testDecompositionHelper(myMatrix, myResult);
  */
        }

        void testTypeInvariance() {
            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();

            myMatrix.makeScaling(Vector3<T>(1, 1, 1));
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with makeScaling");

            myMatrix.makeTranslating(Vector3<T>(0, 0, 0));
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with translating");

            myMatrix.makeXRotating(0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with makeXRotating");

            myMatrix.makeYRotating(0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with makeYRotating");

            myMatrix.makeZRotating(0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with makeZRotating");

            myMatrix.makeRotating(Vector3<T>(1,1,1), 0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with makeRotating");

            myMatrix.scale(Vector3<T>(1, 1, 1));
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with scale");

            myMatrix.translate(Vector3<T>(0, 0, 0));
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with translate");

            myMatrix.rotateX(0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with rotateX");

            myMatrix.rotateY(0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with rotateY");

            myMatrix.rotateZ(0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with rotateZ");

            myMatrix.rotateXYZ(Vector3<T>(0,0,0));
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with rotateXYZ");

            myMatrix.rotate(Vector3<T>(1,1,1), 0);
            ENSURE_MSG(myMatrix.getType() == IDENTITY, "Testing invariance with rotate");
        }

        void testTransformedNormalHelper(const Matrix4<T> & theMatrix) {
            Vector3<T> myNormal(1,2,3);
            myNormal = normalized(myNormal);
            Vector3<T> myTransformedNormal = transformedNormal(myNormal, theMatrix);

            ENSURE_MSG(almostEqual(magnitude(myTransformedNormal), 1),
                (string("Test transformed normal length = 1 for matrix type ") + theMatrix.getTypeString()).c_str());

            // Find three points on a plane perpendicular to myNormal
            Point3<T> p1(myNormal[1], -myNormal[0], 0);
            Point3<T> p2 = asPoint(cross(myNormal, asVector(p1)));
            Point3<T> p3 = p1 + asVector(p2);

            Triangle<T> myTriangle(p1,p3, p2);
            Vector3<T> myTriangleNormalBeforTransf = normalized(myTriangle.normal());
            myTriangle = myTriangle * theMatrix;
            Vector3<T> myTriangleNormal = normalized(myTriangle.normal());

            ENSURE_MSG(almostEqual(myTransformedNormal, myTriangleNormal),
                (string("Test transformed normal for matrix type ") + theMatrix.getTypeString()).c_str());
        }

        void testTransformedNormal() {
            Matrix4<T> myMatrix;
            myMatrix.makeIdentity();

            testTransformedNormalHelper(myMatrix);

            myMatrix.makeScaling(Vector3<T>(3, 3, 3));
            testTransformedNormalHelper(myMatrix);

            myMatrix.makeScaling(Vector3<T>(1, 2, 3));
            testTransformedNormalHelper(myMatrix);

            myMatrix.makeTranslating(Vector3<T>(1, 2, 3));
            testTransformedNormalHelper(myMatrix);

            myMatrix.makeXRotating(1);
            testTransformedNormalHelper(myMatrix);

            myMatrix.makeYRotating(2);
            testTransformedNormalHelper(myMatrix);

            myMatrix.makeZRotating(3);
            testTransformedNormalHelper(myMatrix);

            myMatrix.makeRotating(normalized(Vector3<T>(1,2,3)), T(-2.234));
            testTransformedNormalHelper(myMatrix);

            myMatrix.scale(Vector3<T>(1,2,3));
            testTransformedNormalHelper(myMatrix);

            myMatrix.assign(1,2,3,0,15,6,7,0,9,10,11,0,-10,20,30,1, AFFINE);
            testTransformedNormalHelper(myMatrix);

            //The transformedNormal method does not work with arbitrary unknown matrices
            //myMatrix.assign(1,2,3,0,15,6,7,8,9,10,11,12,13,14,15, 1, Matrix4<T>::UNKNOWN);
            //testTransformedNormalHelper(myMatrix);
        }

        void testMakeLookAt() {
            Matrix4<T> myMatrix;
            Vector3<T> myEye(1,1,1);
            Vector3<T> myCenter(0,0,0);
            Vector3<T> myUp(0,1,0);

            myMatrix.makeLookAt(myEye, myCenter, myUp);

            ENSURE(myMatrix.getTranslation() == Vector3<T>(-1,-1,-1)); 
            ENSURE(almostEqual(myMatrix.getRow(0), Vector4<T>(0.57735f,0,-0.57735f,0))); 
            ENSURE(almostEqual(myMatrix.getRow(1), Vector4<T>(-0.333333f,0.666667f,-0.333333f,0))); 
            ENSURE(almostEqual(myMatrix.getRow(2), Vector4<T>(0.57735f,0.57735f,0.57735f,0))); 

            //std::cout << myMatrix << std::endl;
        }

            
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new Matrix4UnitTest<float>);
        addTest(new Matrix4UnitTest<double>);
    }
};

int main(int argc, char *argv[]) {
    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();
	std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << endl;
    return mySuite.returnStatus();
}
