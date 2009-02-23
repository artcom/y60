/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: UnitTest.cpp,v $
//
//   $Revision: 1.5 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "UnitTest.h"

#include "console_functions.h"

#ifdef _WIN32
#   include <windows.h>
#   include <io.h>
#endif

#ifndef _WIN32
#   include "signal_functions.h"
#endif

using namespace std;
using namespace asl;

#if 0
bool isTTY(ostream & os) {
#ifndef _WIN32
    int myFD = -1;
    if (&os == &cout) {
        myFD = fileno(stdout);
    }
    if (&os == &cerr) {
        myFD = fileno(stderr);
    }

    return isatty( myFD );
#else
    int myFD = -1;
    if (&os == &cout) {
        myFD = _fileno(stdout);
    }
    if (&os == &cerr) {
        myFD = _fileno(stderr);
    }

    return _isatty( myFD );
#endif
}

#ifdef _WIN32
HANDLE GetConsole(ostream & os) {
    HANDLE myHandle = 0;
    if (&os == &cout) {
        myHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    if (&os == &cerr) {
        myHandle = GetStdHandle(STD_ERROR_HANDLE);
    }
    return myHandle;
}
#endif

ostream & TTYRED ( ostream & os ) {
    if (isTTY(os)) {
#if _WIN32
        os.flush();
        SetConsoleTextAttribute(GetConsole(os), FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
        os << "\033[0;31m";
#endif
    }
    return os;
}

ostream & TTYGREEN ( ostream & os ) {
    if (isTTY(os)) {
#if _WIN32
        os.flush();
        SetConsoleTextAttribute(GetConsole(os), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
        os << "\033[0;32m";
#endif
    }
    return os;
}

ostream & asl::TTYYELLOW ( ostream & os ) {
    if (isTTY(os)) {
#if _WIN32
        os.flush();
        SetConsoleTextAttribute(GetConsole(os), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
        os << "\033[0;33m";
#endif
    }
    return os;
}
ostream & ENDCOLOR ( ostream & os ) {
    if (isTTY(os)) {
#if _WIN32
        os.flush();
        SetConsoleTextAttribute(GetConsole(os), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#else
        os << "\033[0m";
#endif
    }
    return os;
}
#endif

unsigned int
UnitTest::getPassedCount() const {
    return _passedCount;
}

unsigned int
UnitTest::getFailedCount() const {
    return _failedCount;
}

unsigned int
UnitTest::getExpectedFailedCount() const {
    return _expectedfailedCount;
}

const char *
UnitTest::getMyName() const {
    return _myName;
}


UnitTest::~UnitTest() {
}

void
UnitTest::incrementPassedCount() {
    ++_passedCount;
}

void
UnitTest::incrementFailedCount() {
    ++_failedCount;
}

void
UnitTest::incrementExpectedFailedCount() {
    ++_expectedfailedCount;
}

int
UnitTest::returnStatus() const {
    std::cout << "Passed count: " << getPassedCount() << "\n";
    std::cout << "Failed count: " << getFailedCount() << std::endl;;
    if (getPassedCount() != 0 && getFailedCount() == 0) {
        return 0;
    } else {
        return 1;
    }
}

void
UnitTest::setSilentSuccess(bool makeSilent) {
    _silentSuccess = makeSilent;
}

void
UnitTest::setAbortOnFailure(bool makeAbort) {
    _abortOnFailure = makeAbort;
}

void
UnitTest::setup() {
    std::cerr << ">>>> Launching Test Unit '" << _myName << "'" << std::endl;
}

void
UnitTest::teardown() {
    std::cerr << ">>>> Completed Test Unit '" << getMyName() << "'" << ", ";

    if (getFailedCount()) {
        std::cerr << TTYRED;
    }
    std::cerr << getFailedCount() << " total tests failed unexpected" << ENDCOLOR;

    std::cerr << ", ";
    if (getExpectedFailedCount()) {
        std::cerr << TTYYELLOW;
    }
    std::cerr << getExpectedFailedCount() << " total tests failed expected" << ENDCOLOR;

    std::cerr << ", ";
    if (getPassedCount()) {
        std::cerr << TTYGREEN;
    }
    std::cerr << getPassedCount() << " total tests passed" << ENDCOLOR;

    std::cerr << std::endl;

}

UnitTest::UnitTest(const char * myName)
    : _myName(myName)
    , _passedCount(0), _failedCount(0), _expectedfailedCount(0)
    , _profilingRepeatCount(0)
    , _silentSuccess(false), _abortOnFailure(false) { }

void
UnitTest::ensure(bool myExpressionResult,
        const char * myExpression,
        const char * mySourceFileName,
        unsigned long mySourceLine,
        bool myExpectedResult)
{
    reportResult(myExpressionResult,myExpectedResult);
    if (myExpressionResult == myExpectedResult) {
        if (_silentSuccess) {
            return;
        }
        std::cerr << ">>>>>> "<< (myExpectedResult ? TTYGREEN : TTYYELLOW)
                  << (myExpectedResult ? "OK    " : "KNOWN ") << ENDCOLOR;
    } else {
        std::cerr << "###### "<< (myExpectedResult ? TTYRED : TTYYELLOW)
                  << (myExpectedResult ? "FAIL  " : "UNEXP " ) << ENDCOLOR;
    }
    std::cerr << " ("<< myExpression << "), Line " << mySourceLine << std::endl;
    if (!myExpressionResult && _abortOnFailure) {
        std::cerr << "UnitTest::ensure: Execution aborted" << std::endl;
        abort();
    }
}

const char *
UnitTest::getTracePrefix () {
    return ">>>>>>        ";
}

void
UnitTest::setPassedCount(unsigned int passedTests) {
    _passedCount = passedTests;
}
void
UnitTest::setFailedCount(unsigned int failedTests) {
    _failedCount = failedTests;
}
void
UnitTest::setExpectedFailedCount(unsigned int failedTests) {
    _expectedfailedCount = failedTests;
}

void
UnitTest::setProfileRepeatCount(unsigned int theCount) {
    _profilingRepeatCount = theCount;
}

unsigned int
UnitTest::getProfileRepeatCount() const {
    return _profilingRepeatCount;
}

void
UnitTest::setMyName(const char * theName) {
    _myName = theName;
}

void
UnitTest::reportResult(bool myExpressionResult, bool myExpectedResult)
{
    if( myExpressionResult ) {
        if( myExpectedResult ) {
            incrementPassedCount(); // expected success
        } else {
            incrementPassedCount(); // unexpected success
        }
    } else {
        if( myExpectedResult ) {
            incrementFailedCount(); // unexpected failure
        } else {
            incrementExpectedFailedCount(); // expected failure
        }
    }
}

void
UnitTestSuite::run() {
    try {
        try {
            asl::Exception::initExceptionBehaviour();
#ifndef _WIN32
            asl::initSignalHandling();
#endif
            setup();
        } catch (std::exception & e) {
            std::cerr << TTYRED << "## A std::exception occured during setup of test suite '"
                << getMyName() << ENDCOLOR << "':" << std::endl << e.what() << std::endl;
            incrementFailedCount();
            throw;
        } catch (asl::Exception & e) {
            std::cerr << TTYRED << "## An exception occured during setup of test suite '"
                << getMyName() << ENDCOLOR << "':" << std::endl << e << std::endl;
            incrementFailedCount();
            throw;
        } catch (...) {
            std::cerr << TTYRED << "## An exception occured during setup of test suite '"
                << getMyName() << "'" << ENDCOLOR << std::endl;
            incrementFailedCount();
            throw;
        }
        try {
            if (_argc == 1) {
                for (unsigned i = 0; i < _myTests.size(); ++i) {
                    _myTests[i]->setup();
                    _myTests[i]->run();
                    setPassedCount(getPassedCount() + _myTests[i]->getPassedCount());
                    setFailedCount(getFailedCount() + _myTests[i]->getFailedCount());
                    setExpectedFailedCount(getExpectedFailedCount() + _myTests[i]->getExpectedFailedCount());
                    _myTests[i]->teardown();
                }
            } else {
                if ((_argc > 1) && string("profile") == _argv[1]) {
                    std::cerr << "Running in profile mode" << std::endl;
                    for (unsigned i = 0; i < _myTests.size(); ++i) {
                        _myTests[i]->setup();
                        _myTests[i]->setSilentSuccess();
                        _myTests[i]->setAbortOnFailure();
                        std::cerr << "Repeating Test " << _myTests[i]->getMyName() << " in profile mode "<< _myTests[i]->getProfileRepeatCount()<<" times."<< endl;
                        for (unsigned r = 0; r < _myTests[i]->getProfileRepeatCount(); ++r) {
                            _myTests[i]->run();
                        }
                        setPassedCount(getPassedCount() + _myTests[i]->getPassedCount());
                        setFailedCount(getFailedCount() + _myTests[i]->getFailedCount());
                        setExpectedFailedCount(getExpectedFailedCount() + _myTests[i]->getExpectedFailedCount());
                        _myTests[i]->teardown();
                    }
                }
            }
        } catch (std::exception & e) {
            std::cerr << TTYRED << "## A std::exception occured during execution of test suite '"
                << getMyName() << ENDCOLOR << "':" << std::endl << e.what() << std::endl;
            incrementFailedCount();
            throw;
        } catch (asl::Exception & e) {
            std::cerr << TTYRED << "## An exception occured during execution of test suite '"
                << getMyName() << "':" << ENDCOLOR << std::endl << e << std::endl;
            incrementFailedCount();
            throw;
        } catch (...) {
            std::cerr << TTYRED << "## An unknown exception occured during execution of test suite '"
                << getMyName() << "'" << ENDCOLOR << std::endl;
            incrementFailedCount();
            throw;
        }
        try {
            teardown();
        } catch (std::exception & e) {
            std::cerr << TTYRED << "## A std::exception occured during teardown of test suite '"
                << getMyName() << ENDCOLOR << "':" << std::endl << e.what() << std::endl;
            incrementFailedCount();
            throw;
        } catch (asl::Exception & e) {
            std::cerr << "## An exception occured during teardown of test suite '"
                << getMyName() << "':" << std::endl << e << std::endl;
            incrementFailedCount();
            throw;
        } catch (...) {
            std::cerr << "## An unknown exception occured during teardown of test suite '"
                << getMyName() << "'" << std::endl;
            incrementFailedCount();
            throw;
        }
    } catch (...) {
        std::cerr << "## test suite '" << getMyName() << "'"
            << " was not completed because of an unknown exception" << std::endl;
        throw;
    }
}


void
UnitTestSuite::teardown() {
    destroyMyTests();

    UnitTest::teardown();
    std::cerr << std::endl;
}
