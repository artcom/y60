//==============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin 
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

#include "Enum.h"
#include "UnitTest.h"

#include <iostream>
#include <sstream>

using namespace std;

enum AnimalEnum {
    CAMEL,
    ELEPHANT,
    LION,
    ANT,
    SNAKE,
    AnimalEnum_MAX
};

static const char * AnimalStrings[] = {
    "camel",
    "elephant",
    "lion",
    "ant",
    "snake",
    ""
};

DEFINE_ENUM( Animal, AnimalEnum);

VERIFY_ENUM( Animal, AnimalStrings);

class EnumUnitTest : public UnitTest {
public:
    explicit EnumUnitTest() : UnitTest("EnumUnitTest") {}
    void run() {
        Animal myAnimal;
        myAnimal = CAMEL;
        ENSURE( myAnimal == CAMEL);
        ostringstream myOS;
        myOS << myAnimal;
        ENSURE( myOS.str() == "camel");

        myAnimal = ELEPHANT;
        ENSURE( myAnimal == ELEPHANT);
        myOS.str("");
        myOS << myAnimal;
        ENSURE( myOS.str() == "elephant");

        myAnimal = LION;
        ENSURE( myAnimal == LION);
        myOS.str("");
        myOS << myAnimal;
        ENSURE( myOS.str() == "lion");

        myAnimal = ANT;
        ENSURE( myAnimal == ANT);
        myOS.str("");
        myOS << myAnimal;
        ENSURE( myOS.str() == "ant");

        myAnimal = SNAKE;
        ENSURE( myAnimal == SNAKE);
        myOS.str("");
        myOS << myAnimal;
        ENSURE( myOS.str() == "snake");

        myAnimal.fromString("camel");
        ENSURE( myAnimal == CAMEL);

        //myAnimal.print();
        //std::cerr << endl;
        myOS.str("");
        myAnimal.print( myOS );
        ENSURE( myOS.str() == "camel");

        myAnimal.fromString( "elephant" );
        ENSURE( myAnimal == ELEPHANT);

        istringstream myIS("lion,");
        myAnimal.parse( myIS );
        ENSURE( myAnimal == LION );

        myIS.str("ant");
        myIS >> myAnimal;

        switch (myAnimal) {
            case ANT:
                //cerr << "I hate " << myAnimal << "s" << endl;
                SUCCESS("Switch ok.");
                break;
            default:
                //cerr << "kaputt '" << myAnimal << "'" << endl;
                FAILURE("Switch failed.");
        }
        
        for (unsigned i = 0; i < Animal::MAX; ++i) {
            DPRINT2(i, Animal::getString(i));
            //cerr << i << " = " << Animal::getString(i) << endl;
        }

        ENSURE_EXCEPTION(myAnimal.fromString("bike"), asl::IllegalEnumValue);

    }
};


int main(int argc, char *argv[]) {

    UnitTestSuite mySuite(argv[0]);

    try {
        mySuite.addTest(new EnumUnitTest);
        mySuite.run();
    }
    catch (...) {
        cerr << "## An unknown exception occured during execution." << endl;
        mySuite.incrementFailedCount();
    }

    int returnStatus = -1;
    if (mySuite.getPassedCount() != 0) {
        returnStatus = 0;
    } else {
        cerr << "## No tests." << endl;
        
    }
    cerr << ">> Finsihed test suite '" << argv[0] << "', return status = " << returnStatus << endl;
    return returnStatus;

}
