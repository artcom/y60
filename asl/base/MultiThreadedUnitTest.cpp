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
//    $RCSfile: MultiThreadedUnitTest.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: unit test for Ptr class
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "MultiThreadedUnitTest.h"

#include "UnitTest.h"
#include "Time.h"

#include "string_functions.h"

#include <iostream>

using namespace std;

namespace asl {

    void MultiThreadedTestSuite::run() {
        try {
            asl::Time theStart;
            for (int i = 0; i < getNumTests(); ++i) {
                getTest(i)->setup();
                getTest(i)->setSilentSuccess();
                getTest(i)->setAbortOnFailure();
                ThreadInterface * myThread = dynamic_cast<ThreadInterface*>(getTest(i));

                if (myThread) {
                    myThread->fork();
                } else {
                    FAILURE("Test is not a PosixThread derived class - not executed");
                }
            }
            //sleep(2);
            for (int i = 0; i < getNumTests(); ++i) {

                ThreadInterface * myThread = dynamic_cast<ThreadInterface*>(getTest(i));
                if (myThread) {
                    myThread->join();
                } else {
                    FAILURE("Test is not a PosixThread derived class - not executed");
                }

                setFailedCount(getFailedCount() + getTest(i)->getFailedCount());
                setPassedCount(getPassedCount() + getTest(i)->getPassedCount());
                getTest(i)->teardown();
            }
            asl::Time theFinish;
            cerr << ">>>> Time required for test suite '" << getMyName() << "' :"
                 << theFinish - theStart << " sec. " << endl;
        }
        catch (asl::Exception & ex) {
            cerr << "## An exception occured during execution of test suite '"
                << getMyName() << "'" << endl;
            cerr << ex << endl;
            incrementFailedCount();
        }
        catch (...) {
            cerr << "## An unknown exception occured during execution of test suite '"
                << getMyName() << "'" << endl;
            incrementFailedCount();
        }
    }
    void CatchingUnitTestSuite::run() {
        cerr << "Running CatchingUnitTestSuite '" << getMyName() << "'"<< endl;
        try {
            asl::Time theStart;
            UnitTestSuite::run();
            asl::Time theFinish;
            cerr << ">>>> Time required for test suite '" << getMyName() << "' :"
                 << theFinish - theStart << " sec. " << endl;
        }
        catch (asl::Exception & ex) {
            cerr << "## An exception occured during execution of test suite '"
                << getMyName() << "'" << endl;
            cerr << ex << endl;
            incrementFailedCount();
            throw;
        }
        catch (...) {
            cerr << "## An unknown exception occured during execution of test suite '"
                << getMyName() << "'" << endl;
            incrementFailedCount();
            throw;
        }
    }
}
