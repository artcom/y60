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
//    $RCSfile: testLogger.tst.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: unit test template file - change Time to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#include "Logger.h"

#include "UnitTest.h"
#include "string_functions.h"
#include "Dashboard.h"
#include <string>
#include <iostream>

using namespace std;
using namespace asl;


class LoggerUnitTest : public UnitTest {
public:
    LoggerUnitTest() : UnitTest("LoggerUnitTest") {}
    void run() {
        AC_FATAL << "This is a Test Fatal Log Entry" << "- and this is some more text" << std::endl;
        AC_ERROR << "This is a Test Error Log Entry" << "- and this is some more text" << std::endl;
        AC_WARNING << "This is a Test Warning Log Entry" << "- and this is some more text" << std::endl;
        AC_INFO << "This is a Test Info Log Entry" << "- and this is some more text" << std::endl;
        AC_DEBUG << "This is a Test Debug Log Entry" << "- and this is some more text" << std::endl;
        AC_TRACE << "This is a Test Trace Log Entry" << "- and this is some more text" << std::endl;
        if (AC_TRACE_ON) {
            cerr << "This is a conditional Test Trace Log Entry" << "- and this is some more text" << std::endl;
        }
        if (AC_DEBUG_ON) {
            cerr << "This is a conditional Test debug Log Entry" << "- and this is some more text" << std::endl;
        }
        if (AC_INFO_ON) {
            cerr << "This is a conditional Test info Log Entry" << "- and this is some more text" << std::endl;
        }
       SUCCESS("first");
    }
};

std::string wasteTime() {
    int s = 0;
    for (int i = 0; i<10000;++i) {
		s+=i;
    }
    return asl::as_string(s);
}

class LoggerPerfTest : public UnitTest {
public:
    LoggerPerfTest() : UnitTest("LoggerPerfTest") {}
    void run() {
        const int myTestCount = 100000000;
        asl::Time myStart;
        for (int i = 0; i < myTestCount; ++i) {
            AC_DEBUG << "This is a Test Warning Log Entry" << wasteTime();       
            //int * myPtr = new int(1);
        }
        double myTime = asl::Time() - myStart;
        cerr << myTestCount << " logchecks took " << myTime << "secs., " << myTestCount/myTime/(1000*1000) << " mio. checks/sec" << endl;
       
       SUCCESS("ready");
    }
};


#if 0

// TODO: make thread safety test
void 
thread1_func(PosixThread & thread) {
    for (int i=0; i < 10000; i++) {

        AC_DB(".");

        AC_DB((int)0<<string("."));
        AC_DB((char)0<<string("."));
        AC_DB((float)0<<string("."));
        AC_DB((double)0<<string("."));
        AC_DB((char*)0<<string("."));
        AC_DB((short)0<<string("."));
        AC_DB("thread1_func: ."<<"."<<"."<<"."<<"."<<"."<<"."<<"."<<"."<<"."<<"."<<"."<<flush<<endl);
    }
}

void 
thread2_func(PosixThread & thread) {
    for (int i=0; i < 10000; i++) {
        AC_DB((int)00<<string("0"));
        AC_DB((char)00<<string("0"));
        AC_DB((float)00<<string("0"));
        AC_DB((double)00<<string("0"));
        AC_DB((char*)00<<string("0"));
        AC_DB((short)0<<string("0"));
        AC_DB("thread2_func: 0"<<"0"<<"0"<<"0"<<"0"<<"0"<<"0"<<"0"<<"0"<<"0"<<"0"<<"0"<<flush<<endl);
    }
}




int main() {
    
/*
    DebugLog db;

    db << endl;
    db << flush;
    db << "TEST" << endl;
    db << 0 << endl;
    db << 0.0 << endl;
    db << 0.0 << endl;
*/
    DebugLog::instance() << endl;
    DebugLog::instance() << flush;
    DebugLog::instance() << "TEST" << endl;
    DebugLog::instance() << 0 << endl;
    DebugLog::instance() << 0.0 << endl;
    DebugLog::instance() << 0.0 << endl;
    
    ThreadSafeDebugLog::instance() << endl;
    ThreadSafeDebugLog::instance() << flush;
    ThreadSafeDebugLog::instance() << "TEST" << endl;
    ThreadSafeDebugLog::instance() << 0 << endl;
    ThreadSafeDebugLog::instance() << 0.0 << endl;
    ThreadSafeDebugLog::instance() << 0.0 << endl;
    
    AC_DB("DebugLevel 1");
    AC_DB2("DebugLevel 2");
    AC_DB3("DebugLevel 3");
    AC_WARNING("Debug Warning" << endl << flush);
    AC_ERROR("Debug Error" << endl);
    
    PosixThread thread1(thread1_func);
    PosixThread thread2(thread2_func);
    
    thread1.fork();
    thread2.fork();

    thread1.join();
    thread2.join();

    exit(0);
}
#endif


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new LoggerPerfTest,10);
        addTest(new LoggerUnitTest,10);
    }
};

int main(int argc, char *argv[]) {
    Dashboard::get().print(cout);

    MyTestSuite mySuite(argv[0], argc, argv);
    //asl::Logger::get().setVerbosity(Severity(argc));
    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
        << ", return status = " << mySuite.returnStatus() << endl;
    Dashboard::get().print(cout);
    return mySuite.returnStatus();
}

