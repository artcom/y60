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
//   $RCSfile: system_functions.cpp,v $
//   $Author: ulrich $
//   $Revision: 1.3 $
//   $Date: 2005/04/06 11:14:53 $
//
//
//  Description: The watchdog restarts the application, if it is closed
//               manually or by accident
//
//
//=============================================================================

#include "system_functions.h"
#include <asl/base/Time.h>

#ifdef WIN32
#   include <windows.h>
#elif defined(LINUX)
#   include <sys/wait.h>
#   include <errno.h>
#   include <unistd.h>
#   include <linux/reboot.h>
#   include <sys/reboot.h>
#else
#   error Your platform is missing!
#endif

#include <iostream>
#include <sstream>
#include <cstdlib>

/*#include <time.h>
#include <fstream>
#include <algorithm>
*/
using namespace std;


void initiateSystemReboot() {
#ifdef WIN32 
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process.
    bool ok = OpenProcessToken(GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    if (!ok) {
        dumpLastError ("OpenProcessToken");
    }

    // Get the LUID for the shutdown privilege.
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
        &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;  // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the shutdown privilege for this process.
    ok = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
            (PTOKEN_PRIVILEGES)NULL, 0);
    if (!ok) {
        dumpLastError ("AdjustTokenPrivileges");
    }

    // Rumms.
    // Note that EWX_FORCEIFHUNG doesn't seem to work (Win 2000), so
    // we kill everything using EWX_FORCE.
    ok = ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
            SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER |
            SHTDN_REASON_FLAG_PLANNED);
    if (!ok) {
        dumpLastError("ExitWindowsEx");
    }
#elif defined(LINUX)
//   int myResult = reboot(LINUX_REBOOT_CMD_RESTART);
//   if (myResult == -1) {
//       dumpLastError("reboot");
//   }
    int myResult = system("/sbin/reboot");
    if (myResult == -1) {
        dumpLastError("reboot");
    }
#else
#error Your platform is missing!
#endif
}

void initiateSystemShutdown() {
#ifdef WIN32
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process.
    bool ok = OpenProcessToken(GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    if (!ok) {
        dumpLastError ("OpenProcessToken");
    }

    // Get the LUID for the shutdown privilege.
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
        &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;  // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the shutdown privilege for this process.
    ok = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
            (PTOKEN_PRIVILEGES)NULL, 0);
    if (!ok) {
        dumpLastError ("AdjustTokenPrivileges");
    }

    // Rumms.
    // Note that EWX_FORCEIFHUNG doesn't seem to work (Win 2000), so
    // we kill everything using EWX_FORCE.
    ok = ExitWindowsEx(EWX_POWEROFF | EWX_FORCE,
            SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER |
            SHTDN_REASON_FLAG_PLANNED);
    if (!ok) {
        dumpLastError("ExitWindowsEx");
    }
#elif defined(LINUX)
//   int myResult = reboot(LINUX_REBOOT_CMD_POWER_OFF);
//   if (myResult == -1) {
//       dumpLastError("reboot");
//   }
    int myResult = system("/sbin/halt");
    if (myResult == -1) {
        dumpLastError("reboot");
    }

#else
#error Your platform is missing!
#endif
}

ProcessResult waitForApp( const ProcessInfo & theProcessInfo, int theTimeout, Logger & theLogger ) {
#ifdef WIN32
    return WaitForSingleObject( theProcessInfo.hProcess, theTimeout );
#elif defined(LINUX)
    asl::msleep(theTimeout);
    int myStatus;
    pid_t myResult = waitpid( theProcessInfo, &myStatus, WNOHANG );
    if (myResult == -1) {
        if (getLastErrorNumber() == ECHILD ) {
            return PR_TERMINATED;
        } else {
            return PR_FAILED;
        }
    }
    if (myResult == theProcessInfo) {
        std::ostringstream myOss;
        if (WIFEXITED(myStatus)) {
            myOss << "Process exited with status " << WEXITSTATUS(myStatus) 
	    	  << ".";
            theLogger.logToFile(myOss.str());
            return PR_TERMINATED;
        } else if (WIFSIGNALED(myStatus)) {
	        int mySignal = WTERMSIG(myStatus);
            myOss << "Process terminated with signal " << mySignal 
	    	  << " (" << strsignal(mySignal) << ").";
            theLogger.logToFile(myOss.str());
            return PR_TERMINATED;
        }
    }
    return PR_RUNNING;
#else
#error Your platform is missing!
#endif
}

bool launchApp( const std::string & theFileName, 
                const std::vector<std::string> & theArguments,
                const std::string & theWorkingDirectory, 
                ProcessInfo & theProcessInfo) 
{ 
#ifdef WIN32
    STARTUPINFO StartupInfo = {
        sizeof(STARTUPINFO),
        NULL, NULL, NULL, 0, 0, 0, 0, 0, 0,
        0, STARTF_USESHOWWINDOW, SW_SHOWDEFAULT,
        0, NULL, NULL, NULL, NULL
    };

    std::string myCommandLine = theFileName;
    for (std::vector<std::string>::size_type i = 0; i < theArguments.size(); i++) {
        myCommandLine += " ";
        myCommandLine += theArguments[i];
    }

    return CreateProcess(NULL, &(myCommandLine[0]),
                         NULL, NULL, TRUE, 0,
                         NULL, 
                         theWorkingDirectory.empty() ? NULL : theWorkingDirectory.c_str(), 
                         &StartupInfo,
                         &theProcessInfo);


#elif defined(LINUX)
    theProcessInfo = fork();
    if (theProcessInfo == -1) {
        return false;
    }
    if (theProcessInfo != 0) {
        return true;
    }
    std::vector<char*> myArgv;
    std::cerr << "Starting application " << theFileName << " ";
    myArgv.push_back( const_cast<char*>(theFileName.c_str()) );
    for (std::vector<std::string>::size_type i = 0; i < theArguments.size(); i++) {
        myArgv.push_back( const_cast<char*>(theArguments[i].c_str()) );
        std::cerr << theArguments[i] << " ";
    }
    if ( ! theWorkingDirectory.empty() ) {
        std::cerr << "in directory: '" << theWorkingDirectory << "'";    
    }
    myArgv.push_back(NULL);
    std::cerr << std::endl;
    if ( ! theWorkingDirectory.empty() ) {
        if (chdir( theWorkingDirectory.c_str() ) < 0) {
            dumpLastError( std::string("chdir('") + theWorkingDirectory + "')" );
            std::abort();
        }    
    }
    execvp( theFileName.c_str(), &myArgv[0] );
    dumpLastError(theFileName);
    std::abort();
#else
#error Your platform is missing!
#endif
}

void closeApp( const std::string & theWindowTitle, const ProcessInfo & theProcessInfo,
               Logger & theLogger) 
{
#ifdef WIN32
    // Send WM_CLOSE to window
    if (!theWindowTitle.empty()) {
        HWND myHWnd = FindWindow (0, theWindowTitle.c_str());
        if (!myHWnd) {
            theLogger.logToFile(string("Could not find Window ") + theWindowTitle);
        } else {
            // Find process handle for window
            DWORD myWindowProcessId;
            DWORD myWindowThreadId = GetWindowThreadProcessId(myHWnd, &myWindowProcessId);

            SendMessage(myHWnd, WM_CLOSE, 0, 0);
            theLogger.logToFile("WM_CLOSE sent.");
            asl::msleep(500); // Give it some time to work...

            // Terminate process
            HANDLE myWindowProcessHandle = OpenProcess(0, false, myWindowProcessId);
            if (myWindowProcessHandle) {
                TerminateProcess(myWindowProcessHandle, 0);
                CloseHandle(myWindowProcessHandle);
            }
        }
    }
    else {
        // Terminate process
        theLogger.logToFile("Try to terminate application.");
        TerminateProcess(theProcessInfo.hProcess, 0);
        theLogger.logToFile("O.k., terminated.");

        // Close process and thread handles.
        CloseHandle( theProcessInfo.hProcess );
        CloseHandle( theProcessInfo.hThread );
    }
#elif defined(LINUX)
    theLogger.logToFile("Try to terminate application.");
    kill( theProcessInfo, SIGTERM );
    ProcessResult myResult = waitForApp( theProcessInfo, 500, theLogger );
    switch (myResult) {
        case PR_RUNNING:
            theLogger.logToFile("Sending SIGKILL...");
            kill( theProcessInfo, SIGKILL );
            break;
        case PR_TERMINATED: 
            theLogger.logToFile("O.k., terminated.");
            break;
        default:
            theLogger.logToFile("Error waiting for process: " + getLastError());
            std::abort();
            break;
    }
#else
#error Your platform is missing!
#endif
}

ErrorNumber getLastErrorNumber() {
#ifdef WIN32
    return GetLastError();
#elif defined(LINUX) 
    return errno;
#else 
#error Your platform is missing!
#endif
}


std::string getLastError( ErrorNumber theErrorNumber) {
#ifdef WIN32
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            theErrorNumber,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
        );
        std::string myResult((LPCSTR)lpMsgBuf);
        LocalFree(lpMsgBuf);
        return myResult;
#elif defined(LINUX) 
        return strerror( theErrorNumber );
#else 
#error Your platform is missing!
#endif
}


std::string getLastError() {
    return getLastError( getLastErrorNumber() );
}


void
dumpLastError(const string& theErrorLocation) {
    ErrorNumber myErrorNumber = getLastErrorNumber();
    cerr << "Warning: \"" << theErrorLocation << "\" failed.\n";
    cerr << "         Error was \"" << getLastError( myErrorNumber ) 
         << "\" with code : " << myErrorNumber << endl;
}
