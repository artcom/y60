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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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

