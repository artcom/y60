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

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>

/*#include <time.h>
#include <fstream>
#include <algorithm>
*/
using namespace std;


void initiateSystemReboot() {
    
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process.
    bool ok = OpenProcessToken(GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    if (!ok) {
        dumpWinError ("OpenProcessToken");
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
        dumpWinError ("AdjustTokenPrivileges");
    }

    // Rumms.
    // Note that EWX_FORCEIFHUNG doesn't seem to work (Win 2000), so
    // we kill everything using EWX_FORCE.
    ok = ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
            SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER |
            SHTDN_REASON_FLAG_PLANNED);
    if (!ok) {
        dumpWinError("ExitWindowsEx");
    }
}

void initiateSystemShutdown() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process.
    bool ok = OpenProcessToken(GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    if (!ok) {
        dumpWinError ("OpenProcessToken");
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
        dumpWinError ("AdjustTokenPrivileges");
    }

    // Rumms.
    // Note that EWX_FORCEIFHUNG doesn't seem to work (Win 2000), so
    // we kill everything using EWX_FORCE.
    ok = ExitWindowsEx(EWX_POWEROFF | EWX_FORCE,
            SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER |
            SHTDN_REASON_FLAG_PLANNED);
    if (!ok) {
        dumpWinError("ExitWindowsEx");
    }
    
}
void
dumpWinError(const string& theErrorLocation) {
    TCHAR sysMsg[256];
    TCHAR* p;
    DWORD eNum = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  eNum,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  sysMsg,
                  256,
                  NULL
                  );
     // Trim the end of the line and terminate it with a null
      p = sysMsg;
      while( ( *p > 31 ) || ( *p == 9 ) )
        ++p;
      do { *p-- = 0; } while( ( p >= sysMsg ) &&
                          ( ( *p == '.' ) || ( *p < 33 ) ) );
    cerr << "Warning: " << theErrorLocation << " failed." << endl;
    cerr << "         Error was " << (char*)sysMsg << " with code : " << eNum << endl;
}
