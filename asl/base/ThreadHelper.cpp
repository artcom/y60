//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: ThreadHelper.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.2 $
//
// Description: 
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "ThreadHelper.h"
#include "PosixThread.h"
#include "Logger.h"

#include <iostream>

using namespace std;
using namespace asl;

bool
threadPriorityFromStrings (const string & thePriorityClassName, const string & thePriorityName,
        int & thePriorityClass, int & thePriority)
{
    if (thePriorityClassName == "SCHED_FIFO") {
        thePriorityClass = SCHED_FIFO;
    } else if (thePriorityClassName == "SCHED_RR") {
        thePriorityClass = SCHED_FIFO;
    } else if (thePriorityClassName == "SCHED_OTHER") {
        thePriorityClass = SCHED_FIFO;
    } else {
        AC_ERROR << "unknown value in thePriorityClassString: " << thePriorityClassName << endl;
        return false;
    }

    if (thePriorityName == "MAX") {
        thePriority = PosixThread::getMaxPriority(thePriorityClass);
    } else if (thePriorityName == "MAX-1") {
        thePriority = PosixThread::getMaxPriority(thePriorityClass) - 1;
    } else if (thePriorityName == "MED") {
        thePriority = (PosixThread::getMinPriority(thePriorityClass)
                + PosixThread::getMaxPriority(thePriorityClass))/2;
    } else if (thePriorityName == "MIN+1") {
        thePriority = PosixThread::getMinPriority(thePriorityClass) + 1;
    } else if (thePriorityName == "MIN") {
        thePriority = PosixThread::getMinPriority(thePriorityClass);
    } else {
        AC_ERROR << "unknown value in thePriorityString: " << thePriorityName << endl;
        return false;
    }
    return true;
}

void
pthreadCheckReturnValue(int theReturnValue, const char *theFunctionName) {
    if(theReturnValue != 0) {
        static bool myRecursionGuard = false;

#define ERROR_MESSAGE \
        "pthread fault: " \
            << theFunctionName << " returned " \
            << theReturnValue << "(" << strerror(theReturnValue) << ")"

        if(myRecursionGuard) {
            cerr << ERROR_MESSAGE;
        } else {
            myRecursionGuard = true;

            AC_FATAL << ERROR_MESSAGE;
        }

#undef ERROR_MESSAGE

        abort();
    }
}
