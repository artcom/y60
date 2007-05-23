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

#include "UnitTest.h"
#include "StackTrace.h"

#include <string>
#include <iostream>

using namespace std; 
using namespace asl; 


void throwAndTrace() {
    throw asl::Exception("Alles Ok!", PLUS_FILE_LINE );
}


class StackTraceUnitTest : public UnitTest {
public:
    StackTraceUnitTest() : UnitTest("StackTraceUnitTest") {  }
    void run() {
        StackTrace myStack;
        // if this fails because of optimization just change it to greater zero
        ENSURE( myStack.size() == 7 );
        DPRINT( myStack );
        ENSURE_EXCEPTION( throwAndTrace(), asl::Exception );

        asl::Exception myException("Alles Ok!", PLUS_FILE_LINE);
        ENSURE( myException.stack().size() == 8 );
        DPRINT( Exception::getDumpStackTraceFlag() );
        DPRINT( myException );
        Exception::dumpStackTrace( true );
        DPRINT( Exception::getDumpStackTraceFlag() );
        DPRINT( myException );
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new StackTraceUnitTest, 10000);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
