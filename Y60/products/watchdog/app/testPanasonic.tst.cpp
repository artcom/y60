//============================================================================
// Copyright (C) 2004 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//=============================================================================

#include <asl/base/UnitTest.h>
#include <asl/base/Ptr.h>

#include "Projector.h"


const int myPort = 0; // if this doesn´t exist under Windows the test will fail


class TestPowerUp: public UnitTest {
public:
    TestPowerUp() : UnitTest ("TestPowerUp") {}

    virtual void run() {

    	asl::Ptr<Projector> myProjector(Projector::getProjector("panasonic", myPort));
        ENSURE(myProjector != 0);
    	myProjector->powerUp();
    }
};

class TestInput: public UnitTest {
public:
    TestInput() : UnitTest ("TestInput") {}

    virtual void run() {

        asl::Ptr<Projector> myProjector(Projector::getProjector("panasonic", myPort));
        ENSURE(myProjector != 0);
        //myProjector->selectInput("RGB_1");
        //myProjector->update();
        //myProjector->command("projector_shutter_open");
        myProjector->shutter(false); //command("projector_shutter_close");
   }
};

class TestPowerDown: public UnitTest {
public:
    TestPowerDown() : UnitTest ("TestPowerDown") {}

    virtual void run() {

        asl::Ptr<Projector> myProjector(Projector::getProjector("panasonic", myPort));
        ENSURE(myProjector != 0);
        myProjector->powerDown();
    }
};


int main( int argc, char *argv[] )
{
    UnitTestSuite mySuite("Panasonic Projector tests", argc, argv);

    try {
        //mySuite.addTest(new TestPowerUp);
        mySuite.addTest(new TestInput);
        //mySuite.addTest(new TestPowerDown);
#if 1
        mySuite.run();
#else
	    std::cerr<<"##### WARNING: tests disabled"<<std::endl;
	    return 0;
#endif	
    }
    catch (const asl::Exception& e) {
        std::cerr << "!!! " << e << std::endl;
    }

    return mySuite.returnStatus();
}
