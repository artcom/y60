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
//    $RCSfile: testDataTypes.tst.cpp,v $
//
//   $Revision: 1.1 $
//
// Description: unit test template file - change FixedVector to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#include "DataTypes.h"


#include <asl/FixedVector.h>
#include <asl/linearAlgebra.h>
#include <asl/UnitTest.h>

#include <string>
#include <iostream>

using namespace std;  // automatically added!
using namespace asl;


class FixedVectorStreamUnitTest : public UnitTest {
    public:
        FixedVectorStreamUnitTest() : UnitTest("FixedVectorStream") {  }
        void run()
        {
            {
                std::string myInt2Vector("[[1,0],[2,3]]");
                std::vector<Vector2i> myVector;
                myVector = as<std::vector<Vector2i> >(myInt2Vector);
                ENSURE( myVector[0][0] == 1);
                ENSURE( myVector[0][1] == 0);
                ENSURE( myVector[1][0] == 2);
                ENSURE( myVector[1][1] == 3);
            }
        
            {
                std::vector<Vector2i> myVector;
                myVector.push_back(Vector2i(0,1));
                myVector.push_back(Vector2i(2,3));
        
                std::string myVectorString = as_string(myVector);
                myVector = as< std::vector<Vector2i> >(myVectorString);
        
                ENSURE(myVector[0][0] == 0);
                ENSURE(myVector[0][1] == 1);
                ENSURE(myVector[1][0] == 2);
                ENSURE(myVector[1][1] == 3);
                
            }

            {
                std::string myString = "[[``,`foo`],[],[``]]";

                std::vector<y60::VectorOfString> myVector = as< std::vector<y60::VectorOfString> >(myString);
        
                ENSURE(myVector[0].size() == 2);
                ENSURE(myVector[0][0] == "");
                ENSURE(myVector[0][1] == "foo");
                ENSURE(myVector[1].size() == 0);
                ENSURE(myVector[2].size() == 1);
                ENSURE(myVector[2][0] == "");
                ENSURE(myVector.size() == 3);
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
