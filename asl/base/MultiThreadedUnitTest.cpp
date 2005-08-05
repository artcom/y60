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
//    $RCSfile: MultiThreadedUnitTest.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: unit test for Ptr class 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"
#include "Time.h"

#include "MultiThreadedUnitTest.h"
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
