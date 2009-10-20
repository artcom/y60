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

#ifndef _WIN32

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#endif

namespace asl {

#ifndef _WIN32

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
        int res;

        if(_myWorkingDirectory.length() > 0) {
            res = chdir(_myWorkingDirectory.c_str());
            if(res == -1) {
                goto bail;
            }
        }

        res = execl("/bin/sh", "sh", "-c", _myCommand.c_str(), NULL);
        if(res == -1) {
            goto bail;
        }

bail:
        // NOTE We exit() with errno to pass the cause.
        //      This could be used in the parent to
        //      explain failure, but currently isn't
        exit(errno);
    }
}

void
Process::kill() {
    if(_myState != PROCESS_LAUNCHED) {
        throw ProcessException("Can't kill a process that has never been launched", PLUS_FILE_LINE);
    }

    int myResult = ::kill(_myPid, SIGTERM);
    if(myResult == -1) {
        throw ProcessException("Failed to send SIGTERM to process: " + std::string(strerror(errno)), PLUS_FILE_LINE);
    }

    // XXX we should have some kind of timeout here and send SIGKILL when it is reached

    waitForTermination();
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

#else

void
Process::launch() {
    if(_myState != PROCESS_INITIALIZED) {
        throw ProcessException("Can't launch a process that has allready been launched", PLUS_FILE_LINE);
    }

    // NOTE CreateProcess mutates this string
    char *myCommandLine = strdup(_myCommand.c_str());

    STARTUPINFO myStartupInfo;
    ZeroMemory(&myStartupInfo, sizeof(STARTUPINFO));
    myStartupInfo.cb = sizeof(STARTUPINFO);

    const char* myWorkingDirectory = NULL;
    if(_myWorkingDirectory.length()) {
        myWorkingDirectory = _myWorkingDirectory.c_str();
    }

    PROCESS_INFORMATION myProcessInfo;
    if(!CreateProcess(NULL, myCommandLine, NULL, NULL, TRUE, 0, NULL, myWorkingDirectory, &myStartupInfo, &myProcessInfo)) {
        free(myCommandLine);
        handleSystemError("CreateProcess", GetLastError(), PLUS_FILE_LINE);
    }

    free(myCommandLine);

    _myHandle = myProcessInfo.hProcess;
    _myState = PROCESS_LAUNCHED;
}

void
Process::kill() {
    if(_myState != PROCESS_LAUNCHED) {
        throw ProcessException("Can't kill a process that has never been launched", PLUS_FILE_LINE);
    }

    DWORD myThreadId;
    if(!CreateRemoteThread(_myHandle, NULL, 0, (LPTHREAD_START_ROUTINE)ExitProcess, (LPVOID)0, 0, &myThreadId)) {
        handleSystemError("CreateRemoteThread", GetLastError(), PLUS_FILE_LINE);
    }

    DWORD myResult = WaitForSingleObject(_myHandle, 1000);

    if(myResult == WAIT_OBJECT_0) {
        DWORD myExitCode;
        if(!GetExitCodeProcess(_myHandle, &myExitCode)) {
            handleSystemError("GetExitCodeProcess", GetLastError(), PLUS_FILE_LINE);
        }

        _myStatusCode = myExitCode;
        _myState = PROCESS_TERMINATED;

        CloseHandle(_myHandle);

        return;
    }
    
    if(myResult == WAIT_FAILED) {
        handleSystemError("WaitForSingleObject", GetLastError(), PLUS_FILE_LINE);
    }

    if(!TerminateProcess(_myHandle, 0)) {
        handleSystemError("TerminateProcess", GetLastError(), PLUS_FILE_LINE);
    }

    DWORD myExitCode;
    if(!GetExitCodeProcess(_myHandle, &myExitCode)) {
        handleSystemError("GetExitCodeProcess", GetLastError(), PLUS_FILE_LINE);
    }

    _myStatusCode = myExitCode;
    _myState = PROCESS_TERMINATED;

    CloseHandle(_myHandle);
}

bool
Process::pollForTermination() {
    if(_myState != PROCESS_LAUNCHED) {
        throw ProcessException("Can't poll for a process that is not running", PLUS_FILE_LINE);
    }

    DWORD myResult = WaitForSingleObject(_myHandle, 0);

    if(myResult == WAIT_OBJECT_0) {
        DWORD myExitCode;
        if(!GetExitCodeProcess(_myHandle, &myExitCode)) {
            handleSystemError("GetExitCodeProcess", GetLastError(), PLUS_FILE_LINE);
        }

        _myStatusCode = myExitCode;
        _myState = PROCESS_TERMINATED;

        CloseHandle(_myHandle);

        return true;
    }

    if(myResult == WAIT_FAILED) {
        handleSystemError("WaitForSingleObject", GetLastError(), PLUS_FILE_LINE);
    }

    return false;
}

void
Process::waitForTermination() {
    if(_myState != PROCESS_LAUNCHED) {
        throw ProcessException("Can't wait for a process that is not running", PLUS_FILE_LINE);
    }

    DWORD myResult = WaitForSingleObject(_myHandle, INFINITE);

    if(myResult == WAIT_OBJECT_0) {
        DWORD myExitCode;
        if(!GetExitCodeProcess(_myHandle, &myExitCode)) {
            handleSystemError("GetExitProcessCode", GetLastError(), PLUS_FILE_LINE);
        }

        _myStatusCode = myExitCode;
        _myState = PROCESS_TERMINATED;

        CloseHandle(_myHandle);

        return;
    }

    if(myResult == WAIT_FAILED) {
        handleSystemError("WaitForSingleObject", GetLastError(), PLUS_FILE_LINE);
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
