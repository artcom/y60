/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testAssure.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test for Exception classes 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Assure.h"

#include "UnitTest.h"

#include <sys/types.h>
#include <signal.h>
#ifndef WIN32
#include <sys/wait.h>
#include <unistd.h>
#endif

using namespace std;

bool isBigger(int a, int b) {
    return a > b;
}

class AssureUnitTest : public UnitTest {
public:
    AssureUnitTest() 
        : UnitTest("AssureUnitTest") {  }
    void run() {
        #undef ASSURE_POLICY
        #define ASSURE_POLICY AssurePolicy::Ignore
        ASSURE(2 * 2 == 4);
        SUCCESS("AssurePolicy::Ignore true expression");
        ASSURE(2 * 2 == 5);
        int myFooBar = 5;
        SUCCESS("AssurePolicy::Ignore false expression");
        ASSURE(isBigger(0,23));
        SUCCESS("Testing function call within ASSURE()");
        #undef ASSURE_POLICY
      
        //Assure<AssurePolicy::Ignore> myTest(isBigger(0,23), "some text", "lalala", 2);

        ASSURE_WITH(AssurePolicy::Ignore, 1 + 1 == 2);
        SUCCESS("Explicit AssurePolicy::Ignore true expression");
        ASSURE_WITH(AssurePolicy::Ignore, 1 + 1 == 3);
        SUCCESS("Explicit AssurePolicy::Ignore false expression");
        
        ASSURE_WITH(AssurePolicy::Warn, 1 + 1 == 2);
        SUCCESS("Explicit AssurePolicy::Warn true expression");
        ASSURE_WITH(AssurePolicy::Warn, 1 + 1 == 3);
        SUCCESS("Explicit AssurePolicy::Warn false expression");

        ASSURE_WITH(AssurePolicy::Throw, true);
        SUCCESS("Explicit AssurePolicy::Throw true expression");
        try {
            ASSURE_WITH(AssurePolicy::Throw,false);
            FAILURE("Exception not thrown");
        }
        catch (AssurePolicy::Violation & ex) {
            SUCCESS("Exception thrown and caught:");
            cerr << ex << endl;
        }

#ifndef WIN32        
        ASSURE_WITH(AssurePolicy::Exit, true);
        SUCCESS("Explicit AssurePolicy::Exit true expression");
        pid_t childs_pid = fork();
        if (childs_pid == 0) {
            ASSURE_WITH(AssurePolicy::Exit, false);
            exit(0);
        }
        int childStatus;
        waitpid(childs_pid, &childStatus, 0);
        DPRINT(WEXITSTATUS(childStatus));
        ENSURE(WEXITSTATUS(childStatus) == 255);

        ASSURE_WITH(AssurePolicy::Abort, true);
        SUCCESS("Explicit AssurePolicy::Abort true expression");
        childs_pid = fork();
        if (childs_pid == 0) {
            ASSURE_WITH(AssurePolicy::Abort, false);
            exit(0);
        }
        waitpid(childs_pid, &childStatus, 0);
        DPRINT(WEXITSTATUS(childStatus));
        ENSURE(WTERMSIG(childStatus) == SIGABRT);
#endif        
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new AssureUnitTest());
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
