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

#ifdef UNIX

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

#endif

#ifdef WIN32

void
Process::launch() {
    if(_myState != PROCESS_INITIALIZED) {
        throw ProcessException("Can't launch a process that has allready been launched", PLUS_FILE_LINE);
    }

    // NOTE CreateProcess mutates this string
    char *myCommandLine = strdup(_myCommand.c_str());

    PROCESS_INFORMATION myProcessInfo;
    if(!CreateProcess(NULL, myCommandLine, NULL, NULL, TRUE, 0, NULL, NULL, NULL, &myProcessInfo)) {
        LocalFree(myCommandLine);
        handleSystemError("CreateProcess", GetLastError());
    }

    LocalFree(myCommandLine);

    _myHandle = myProcessInfo.hProcess;
    _myState = PROCESS_LAUNCHED;
}

bool
Process::pollForTermination() {
    DWORD myResult = WaitForSingleObject(_myHandle, 0);

    if(myResult == WAIT_OBJECT_0) {
        DWORD myExitCode;
        if(!GetExitCodeProcess(_myHandle, &myExitCode)) {
            handleSystemError("Error retrieving exit code", GetLastError());
        }

        _myStatusCode = myExitCode;
        _myState = PROCESS_TERMINATED;

        CloseHandle(_myHandle);

        return true;
    }

    if(myResult == WAIT_FAILED) {
        handleSystemError("WaitForSingleObject", GetLastError());
    }

    return false;
}

void
Process::waitForTermination() {
    DWORD myResult = WaitForSingleObject(_myHandle, INFINITE);

    if(myResult == WAIT_OBJECT_0) {
        DWORD myExitCode;
        if(!GetExitCodeProcess(_myHandle, &myExitCode)) {
            handleSystemError("GetExitProcessCode", GetLastError());
        }

        _myStatusCode = myExitCode;
        _myState = PROCESS_TERMINATED;

        CloseHandle(_myHandle);

        return;
    }

    if(myResult == WAIT_FAILED) {
        handleSystemError("WaitForSingleObject", GetLastError());
    }
}

void
Process::handleSystemError(const std::string & theSystemCall, const DWORD theError, const std::string & theLocationString) {
    if (theError) {
        LPVOID myMessageBuffer;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS, NULL, theError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR) & myMessageBuffer, 0, NULL);
        std::string myErrorString = theSystemCall + " failed: ";
        myErrorString.append((LPTSTR) myMessageBuffer);
        LocalFree(myMessageBuffer);
        throw ProcessException(myErrorString, theLocationString);
    }
}

#endif

}
