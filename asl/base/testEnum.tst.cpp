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
IMPLEMENT_ENUM( Animal, AnimalStrings);

enum PropertyEnum {
    EDIBLE,
    PETABLE,
    RIDEABLE,
    CRUSHABLE,
    PropertyEnum_MAX
};

static const char * PropertyStrings[] = {
    "edible",
    "petable",
    "ridable",
    "crushable",
    "" 
};


DEFINE_BITSET( Properties, Property, PropertyEnum );
IMPLEMENT_BITSET( Propertes, Property, PropertyStrings );

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

class BitsetUnitTest : public UnitTest {
public:
    explicit BitsetUnitTest() : UnitTest("BitsetUnitTest") {}
    void run() {
        Properties myElefantProps;
        myElefantProps.set(RIDEABLE);
        myElefantProps.set(PETABLE);
        ENSURE(myElefantProps[RIDEABLE]);
        ENSURE(myElefantProps[PETABLE]);
        ENSURE( ! myElefantProps[EDIBLE]);
        ENSURE( ! myElefantProps[CRUSHABLE]);
        DPRINT(myElefantProps);

        {
            Properties myAntProps;
            istringstream myStream("[edible,crushable]");
            myAntProps.parse(myStream);
            ENSURE( myStream.good() );
            ENSURE( myAntProps[CRUSHABLE] );
            ENSURE( ! myAntProps[RIDEABLE] );
            ENSURE( myAntProps[EDIBLE] );
            ENSURE( ! myAntProps[PETABLE] );
            ostringstream myOStream;
            myAntProps.print(myOStream);
            DPRINT(myOStream.str());
            ENSURE(myOStream.str() == "[edible,crushable]");
            myOStream.str("");
            myOStream << myAntProps;
            DPRINT(myOStream.str());
            ENSURE(myOStream.str() == "[edible,crushable]");


            myStream.str("[edible]");
            myAntProps.parse(myStream);
            ENSURE( myStream.good() );
            ENSURE( ! myAntProps[CRUSHABLE] );
            ENSURE( ! myAntProps[RIDEABLE] );
            ENSURE( myAntProps[EDIBLE] );
            ENSURE( ! myAntProps[PETABLE] );
        }
        {
            Properties myAntProps;
            istringstream myStream("[crushable,edible]");
            ENSURE( myStream.good() );
            myAntProps.parse(myStream);
            ENSURE( myAntProps[CRUSHABLE] );
            ENSURE( ! myAntProps[RIDEABLE] );
            ENSURE( myAntProps[EDIBLE] );
            ENSURE( ! myAntProps[PETABLE] );
        }
        {
            Properties myAntProps;
            istringstream myStream("[crushable,crushable]");
            myAntProps.parse(myStream);
            ENSURE(myStream.fail());
        }
        {
            Properties myAntProps;
            istringstream myStream("[crushable,edible");
            myAntProps.parse(myStream);
            ENSURE(myStream.fail());
        }
        {
            Properties myAntProps;
            istringstream myStream("[crushable,elephant]");
            myAntProps.parse(myStream);
            ENSURE(myStream.fail());
        }
        {
            Properties myAntProps;
            istringstream myStream("crushable]");
            myAntProps.parse(myStream);
            ENSURE(myStream.fail());
        }
        {
            Properties myJellyfishProps;
            istringstream myStream("[]");
            myJellyfishProps.parse(myStream);
            ENSURE(myStream.good());
            ENSURE( ! myJellyfishProps[CRUSHABLE] );
            ENSURE( ! myJellyfishProps[RIDEABLE] );
            ENSURE( ! myJellyfishProps[EDIBLE] );
            ENSURE( ! myJellyfishProps[PETABLE] );
            ostringstream myOStream;
            myJellyfishProps.print(myOStream);
            // myOStream << myJellyfishProps;
            DPRINT(myOStream.str());
            ENSURE(myOStream.str() == "[]");
        }
        {
            Properties myAntProps;
            istringstream myStream("[crushable,,edible]");
            myAntProps.parse(myStream);
            ENSURE(myStream.fail());
        }
        {
            Properties myAntProps;
            istringstream myStream("[crushable,]");
            myAntProps.parse(myStream);
            ENSURE(myStream.fail());
        }
        {
            Properties myAntProps;
            istringstream myStream("");
            myAntProps.parse(myStream);
            ENSURE(myStream.fail());
        }
    }
};

int main(int argc, char *argv[]) {

    UnitTestSuite mySuite(argv[0], argc, argv);

    try {
        mySuite.addTest(new BitsetUnitTest, 10000);
        mySuite.addTest(new EnumUnitTest, 10000);
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
