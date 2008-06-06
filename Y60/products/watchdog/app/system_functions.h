//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: system_functions.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2005/04/27 12:42:06 $
//
//
//  Description: The watchdog restarts the application, if it is closed
//               manually or by accident
//
//
//=============================================================================

#ifndef INCL_SYSTEM_FUNCTIONS
#define INCL_SYSTEM_FUNCTIONS

#include <string>
#include <vector>

#include "Logger.h"

#ifdef WIN32
#   include <windows.h>
#elif defined(LINUX)
#   include <sys/types.h>
#   include <unistd.h>
#else
#error your platform is missing!
#endif

#ifdef WIN32
    typedef DWORD ProcessResult;
    enum {
        PR_RUNNING = WAIT_TIMEOUT,
        PR_TERMINATED = WAIT_OBJECT_0,
        PR_FAILED = WAIT_FAILED
    };
    typedef PROCESS_INFORMATION ProcessInfo;
    typedef DWORD ErrorNumber;
#elif defined(LINUX)
    typedef pid_t ProcessResult;
    enum {
        PR_RUNNING,
        PR_TERMINATED,
        PR_FAILED
    };
    typedef int ProcessInfo;
    typedef int ErrorNumber;
#else
#error your platform is missing!
#endif


void initiateSystemReboot();
void initiateSystemShutdown();
ProcessResult waitForApp( const ProcessInfo & theProcessInfo, int theTimeout );
bool launchApp( const std::string & theFileName, 
                const std::vector<std::string> & theArguments,
                const std::string & theWorkingDirectory, 
                ProcessInfo & theProcessInfo);
void closeApp( const std::string & theWindowTitle, const ProcessInfo & theProcessInfo,
               Logger & theLogger); 
ErrorNumber getLastErrorNumber();
std::string getLastError();
std::string getLastError( ErrorNumber theErrorNumber);
void dumpLastError(const std::string& theErrorLocation);

#endif

