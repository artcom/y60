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
//    $RCSfile: testVectorStream.tst.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: unit test template file - change FixedVector to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "FixedVector.h"
#include "linearAlgebra.h"
#include <asl/base/UnitTest.h>

#include <string>
#include <iostream>
#include <sstream>

using namespace std;  // automatically added!
using namespace asl;


class FixedVectorStreamUnitTest : public UnitTest {
    public:
        FixedVectorStreamUnitTest() : UnitTest("FixedVectorStream") {  }
        void setMatrix(Matrix4<float> & a, const Matrix4<float> & b) {
            a.assign(b[0][0], b[0][1], b[0][2], b[0][3], b[1][0], b[1][1], b[1][2], b[1][3], 
                     b[2][0], b[2][1], b[2][2], b[2][3], b[3][0], b[3][1], b[3][2], b[3][3]);
        }
        void run()
        {


            {
                Vector3d myVector(1.0, 2.0, 3.0);
                ostringstream myStream;
                myStream << formatFixedVector('<', '>', '#',  false) << myVector;
                ENSURE_MSG(myStream.str() == "<1#2#3>", "Testing format manipulator");
                istringstream myInStream(myStream.str());
                DPRINT(myInStream.str());
                Vector3d myReadVector;
                myInStream >> formatFixedVector('<', '>', '#', false) >> myReadVector;
                ENSURE_MSG(myInStream, "Testing stream state");
                DPRINT(myReadVector);
                ENSURE_MSG(myReadVector == Vector3d(1.0, 2.0, 3.0), "Testing format manipulator for istreams");

                Vector3d myVector2(3.0, 4.0, 5.0);
                myStream.str("");
                myStream << formatFixedVector(')', '(', 't', false) << myVector << " "
                         << formatFixedVector('|', '|', '|', false) << myVector2;
                ENSURE_MSG(myStream.str() == ")1t2t3( |3|4|5|", "Testing multiple manipulators in one stream");
                DPRINT(myVector2);
            }

            
			{
		        Matrix4<double> myMatrix;
        			    	
		        myMatrix.assign(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);			
        		
		        // Test data access
		        const double * myData = myMatrix.getData();						
		        for (unsigned i = 0; i < 16; i++) {			
			        ENSURE(myData[i] == double(i));
		        }						
        		
			}		
            // Test matrix streaming
            {
                Matrix4<float> myMatrix;
                Matrix4<float> myGeneratedMatrix;
                // X_ROTATING
                myGeneratedMatrix.makeXRotating(0.123f);
                std::string myMatrixString = asl::as_string(myGeneratedMatrix);
                asl::fromString(myMatrixString, myMatrix);
                ENSURE_MSG(almostEqual(myMatrix, myGeneratedMatrix), "Testing parsing a x_rotating matrix from string");
                
                // test printing a matrix to string and parsing a matrix from that string
                myMatrixString = "unknown[[0,1,2,3],[4,5,6,7],[8,9,10,11],[12,13,14,15]]";
                asl::fromString(myMatrixString, myMatrix);
                std::string myGeneratedMatrixString = asl::as_string(myMatrix);
                ENSURE_MSG((myMatrixString == myGeneratedMatrixString), "Testing from and to string");

                // create some incorrect strings
                std::string myCorruptMatrixString = "unknown][0,1,2,3],[4,5,6,7],[8,9,10,11],[12,13,14,15]]";
                ENSURE_EXCEPTION(as<Matrix4<float> >(myCorruptMatrixString), ParseException);

                myCorruptMatrixString = "unknown[0,1,2,3],[4,5,6,7],[8,9,10,11],[12,13,14,15]]";
                ENSURE_EXCEPTION(as<Matrix4<float> >(myCorruptMatrixString), ParseException);

                myCorruptMatrixString = "unknown[[0,1,2,3],[4,5,6,7],[8,9,10,11],[12,13,14,15]";
                ENSURE_EXCEPTION(as<Matrix4<float> >(myCorruptMatrixString), ParseException);

                myCorruptMatrixString = "unknown";
                ENSURE_EXCEPTION(as<Matrix4<float> >(myCorruptMatrixString), ParseException);

                ostringstream myOStream;
                myOStream << formatFixedVector('<', '>', '|', false) << myMatrix;
                std::string myStrangeMatrixString = "unknown<<0|1|2|3>|<4|5|6|7>|<8|9|10|11>|<12|13|14|15>>";
                DPRINT(myOStream.str());
                ENSURE_MSG(myStrangeMatrixString == myOStream.str(), "Testing matrix with manipulated format");
                istringstream myIStream(myOStream.str());
                Matrix4<float> myParsedMatrix;
                myIStream >> formatFixedVector('<', '>', '|', false) >> myParsedMatrix;
                ENSURE_MSG(almostEqual(myParsedMatrix, myMatrix), "Testing manipulated matrix parsing");
                
            }
            {
                std::string myVector4String("[1.0,2.0,3.0,4.0]");
                Vector4d myVector4;
                myVector4 = as<Vector4d>(myVector4String);
                ENSURE(almostEqual(myVector4[0], 1.0));
                ENSURE(almostEqual(myVector4[1], 2.0));
                ENSURE(almostEqual(myVector4[2], 3.0));
                ENSURE(almostEqual(myVector4[3], 4.0));
            }
            
            {
                std::string myTruncatedString("[1.0,");
                ENSURE_EXCEPTION(as<Vector4d>(myTruncatedString), ParseException);
            }
           
            {
                std::string myFloat5String("[1.0,2.0,3.0,4.0,5.0]");
                ENSURE_EXCEPTION(as<Vector4d>(myFloat5String), ParseException);
            }
            
            {
                std::string myWrongEndString("[1.0,2.0,3.0,4.0x");
                ENSURE_EXCEPTION(as<Vector4d>(myWrongEndString), ParseException);
            }
            
            {
                Vector4d myVector4(9.8, 7.6, 5.4, 3.2);
                std::string myVector4String = as_string(myVector4);
                myVector4 = as<Vector4d>(myVector4String);
                ENSURE(almostEqual(myVector4[0], 9.8));
                ENSURE(almostEqual(myVector4[1], 7.6));
                ENSURE(almostEqual(myVector4[2], 5.4));
                ENSURE(almostEqual(myVector4[3], 3.2));
            }
                          
            {   
                Vector3d myVector(0,1,1);
                std::string myVectorString = as_string(myVector);
                myVector = as<Vector3d>(myVectorString);
                ENSURE(myVector[0] == 0);
                ENSURE(myVector[1] == 1);
                ENSURE(myVector[2] == 1);        
            }
            {
                Box2f b1(1,2,3,4);
                std::string myBoxString = as_string(b1);
                DPRINT(myBoxString);
                Box2f b2 = as<Box2f>(myBoxString);
                ENSURE(almostEqual(b1, b2)); 
                
                b1.makeEmpty();
                myBoxString = as_string(b1);
                ENSURE(myBoxString == "[]");
                DPRINT(myBoxString);
                b2 = as<Box2f>(myBoxString);
                ENSURE(almostEqual(b1, b2)); 
            }
            {
                Box2f myTestBox(1, 2, 3, 4);
                ostringstream myOStream;
                myOStream << formatFixedVector('<', '>', '|', false) << myTestBox;
                DPRINT(myOStream.str());
                ENSURE_MSG(myOStream.str() == "<<1|2>|<3|4>>", "Testing manipulated box streaming.");
                istringstream myIStream(myOStream.str());
                Box2f myParsedBox;
                myIStream >> formatFixedVector('<', '>', '|', false) >> myParsedBox;
                ENSURE_MSG(almostEqual(myParsedBox,myTestBox), "Testing manipulated box parsing.");
             }
             {
                Box2f myEmptyBox;
                myEmptyBox.makeEmpty();
                ENSURE(!myEmptyBox.hasPosition());
                ostringstream myOStream;
                myOStream << formatFixedVector('<', '>', '|', false) << myEmptyBox;
                DPRINT(myOStream.str());
                ENSURE_MSG(myOStream.str() == "<>", "Testing manipulated empty box streaming.");
                istringstream myIStream(myOStream.str());
                Box2f myParsedBox;
                myIStream >> formatFixedVector('<', '>', '|', false) >> myParsedBox;
                ENSURE_MSG(almostEqual(myParsedBox,myEmptyBox), "Testing manipulated empty box parsing.");
            }
            {
                Box3f b1(1,2,3,4,5,6);
                std::string myBoxString = as_string(b1);
                cerr << myBoxString << endl;
                Box3f b2 = as<Box3f>(myBoxString);
                ENSURE(almostEqual(b1, b2)); 
                
                b1.makeEmpty();
                myBoxString = as_string(b1);
                cerr << myBoxString << endl;
                b2 = as<Box3f>(myBoxString);
                ENSURE(almostEqual(b1, b2)); 
            }
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new FixedVectorStreamUnitTest());
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
