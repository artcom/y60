/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __

*/
#include "DataTypes.h"


#include <asl/math/FixedVector.h>
#include <asl/math/linearAlgebra.h>
#include <asl/base/UnitTest.h>

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
