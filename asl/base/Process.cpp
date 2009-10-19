/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2009, ART+COM AG Berlin, Germany <www.artcom.de>
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

#include "Process.h"

namespace asl {

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

void
Process::launch() {
    if(_myState != PROCESS_INITIALIZED) {
        throw ProcessException("Can't launch a process that has allready been launched", PLUS_FILE_LINE);
    }

    pid_t myPid = fork();
    if(myPid) {
        _myPid = myPid;
        _myState = PROCESS_LAUNCHED;
    } else {
        execl("/bin/sh", "sh", "-c", _myCommand.c_str(), NULL);

        // if we reach this point, an error has occured
        // we exit() with errno to pass the cause
        exit(errno);
    }
}

bool
Process::pollForTermination() {
    if(_myState == PROCESS_TERMINATED) {
        return true;
    }
    if(_myState != PROCESS_LAUNCHED) {
        throw ProcessException("Can't poll for termination of a process that has never been launched.", PLUS_FILE_LINE);
    }

    pid_t myPid = waitpid(_myPid, &_myStatusCode, WNOHANG);
    if(myPid == 0) {
        return false;
    } else if (myPid == _myPid) {
        _myState = PROCESS_TERMINATED;
        return true;
    } else {
        throw ProcessException("Failed to wait for process: " + std::string(strerror(errno)), PLUS_FILE_LINE);
    }
}

void
Process::waitForTermination() {
    if(_myState == PROCESS_TERMINATED) {
        return;
    }
    if(_myState != PROCESS_LAUNCHED) {
        throw ProcessException("Can't wait for termination of a process that has never been launched.", PLUS_FILE_LINE);
    }

    pid_t myPid = waitpid(_myPid, &_myStatusCode, 0);
    if(myPid < 0) {
        throw ProcessException("Failed to wait for process: " + std::string(strerror(errno)), PLUS_FILE_LINE);
    } else {
        _myState = PROCESS_TERMINATED;
    }
}

}
