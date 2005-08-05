/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testException.tst.cpp,v $
//
//   $Revision: 1.1.1.1 $
//
// Description: unit test for Exception classes 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Exception.h"
#include "string_functions.h"

#include "UnitTest.h"

#include <string>
#include <iostream>
#include <vector>

using namespace std;  

class DerivedException : public asl::Exception {
public:
    DerivedException(const string & what, const string & where)
        : Exception(what,where,"DerivedException") {}
};

DEFINE_EXCEPTION(MacroDefinedException,asl::Exception)
DEFINE_EXCEPTION(MacroDefinedDerivedException,MacroDefinedException)

using namespace asl;

template <class E>
class ExceptionUnitTest : public TemplateUnitTest {
public:
    ExceptionUnitTest(const char * theTemplateArgument) 
        : TemplateUnitTest("ExceptionUnitTest",theTemplateArgument) {  }
    void run() {
        try {
            throw(E(JUST_FILE_LINE));
        }
        catch (E & myException) {
            SUCCESS("Caught correct Exception");
            SUCCESS(asl::as_string(myException).c_str());
        }
        catch (Exception &) {
            FAILURE("Failed to catch correctly, but a least caught by base");
        }
        catch (...) {
            FAILURE("Failed to catch correctly");
        }
        try {
            throw(E(JUST_FILE_LINE));
        }
        catch (Exception & myException) {
            SUCCESS("Caught Exception correctly by Base");
            SUCCESS(as_string(myException).c_str());
        }
        catch (...) {
            FAILURE("Failed to catch correctly");
        }
      }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new ExceptionUnitTest<DerivedException>("<DerivedException>"));
        addTest(new ExceptionUnitTest<MacroDefinedException>("<MacroDefinedException>"));
        addTest(new ExceptionUnitTest<MacroDefinedDerivedException>("<MacroDefinedDerivedException>"));
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
