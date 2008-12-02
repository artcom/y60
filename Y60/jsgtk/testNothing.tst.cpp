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
// Description: A do-nothing test to check linker errors 
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "GTKApp.h"
#include <asl/base/UnitTest.h>

class NullUnitTest : public UnitTest {
    public:
        NullUnitTest() : UnitTest("NullUnitTest") {
        }

        void run() {
            asl::Ptr<jslib::GTKApp> myObject(new jslib::GTKApp());
            ENSURE(myObject);
        }
};


class NullTestSuite : public UnitTestSuite {
public:
    NullTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new NullUnitTest());
    }
};

int main(int argc, char *argv[]) {

    NullTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();

    return mySuite.returnStatus();
}
