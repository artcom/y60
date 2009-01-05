/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
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

#include "Task.h"
#include "TaskWindow.h"
#include <asl/base/Exception.h>
#include <asl/base/string_functions.h>

#include <asl/base/Logger.h>
#include <direct.h>

#include <winuser.h>
using namespace std;

#define DB(x) //x

// This allows windows to be set to foreground by other processes
BOOL a = SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);

namespace y60 {

    string getWindowName(HWND theWindowHandle) {
        char myWindowName[255];
        GetWindowText(theWindowHandle, myWindowName, 255);
        return string(myWindowName);
    }

    string getLastError() {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
        );

        char myMessage[255];
        strncpy(myMessage, (LPCSTR)lpMsgBuf, sizeof(myMessage)-1);
        LocalFree(lpMsgBuf);

        return string(myMessage);
    }

    Task::Task() {
        _myProcessInfo.dwThreadId = GetCurrentThreadId();
    }

    Task::Task(string theCommand, string thePath, bool theShowFlag) {
        DB(cerr << "Create task: " << theCommand << " in path: " << thePath << " state: " << (theShowFlag ? "shown":"hidden") << endl;)

        STARTUPINFO myStartupInfo =
        {
            sizeof(STARTUPINFO),
            NULL, NULL, NULL, 0, 0, 0, 0, 0, 0,
            0, STARTF_USESHOWWINDOW, (theShowFlag ? SW_SHOWDEFAULT : SW_MINIMIZE),
            0, NULL, NULL, NULL, NULL
        };

        string myCWD(".");
        if (thePath.size() > 0) {
            myCWD = thePath;
        }

        bool myResult = 0 != CreateProcess( NULL, &theCommand[0],
                                            NULL, NULL, TRUE, 0,
                                            NULL, myCWD.c_str(), &myStartupInfo,
                                            &_myProcessInfo);

        // Error handling
        if (!myResult) {
            throw asl::Exception(string("Could not start process: ") + theCommand
                + "\n" + getLastError(), PLUS_FILE_LINE);
        }
    }
    
    Task::~Task() {
        CloseHandle(_myProcessInfo.hProcess);
        CloseHandle(_myProcessInfo.hThread);
    }

    BOOL CALLBACK WindowCollector(HWND theWindowHandle, LPARAM lParam) {
        Task * myTask = (Task *)lParam;
        myTask->addTaskWindow(theWindowHandle);
        return TRUE;
    }

    void
    Task::collectWindows() const {
        _myTaskWindows.clear();

        // Add external windows to task windows
        for (unsigned i = 0; i < _myExternalWindows.size(); ++i) {
            _myTaskWindows.push_back(_myExternalWindows[i]);
        }

        // Add thread windows to task windows
        if (_myProcessInfo.dwThreadId) {
            EnumThreadWindows(_myProcessInfo.dwThreadId, WindowCollector, (LPARAM)this);            
        }
    }   

    void
    Task::addExternalWindow(string theWindowName) {
        HWND myHandle = FindWindow(0, theWindowName.c_str());
        if (myHandle) {
            _myExternalWindows.push_back(myHandle);
        } else {
            AC_WARNING << "Task::addExternalWindow() - Could not find window: " << theWindowName;
        }
    }

    void
    Task::terminate() {
        collectWindows();

        // Destroy all task windows
        for (unsigned i = 0; i < _myTaskWindows.size(); ++i) {
            SendMessage(_myTaskWindows[i], WM_DESTROY, 0, 0);
        }

        TerminateProcess(_myProcessInfo.hProcess, 0);
    }
 
    void
    Task::setPriority(int thePrio) {                                   
        switch (PRIORITY(thePrio)) {
            case ABOVE_NORMAL:
                SetPriorityClass(_myProcessInfo.hProcess, ABOVE_NORMAL_PRIORITY_CLASS);
                break;
            case BELOW_NORMAL:
                SetPriorityClass(_myProcessInfo.hProcess, BELOW_NORMAL_PRIORITY_CLASS);
                break;
            case HIGH_PRIORITY:
                SetPriorityClass(_myProcessInfo.hProcess, HIGH_PRIORITY_CLASS);
                break;
            case IDLE_PRIORITY:
                SetPriorityClass(_myProcessInfo.hProcess, IDLE_PRIORITY_CLASS);
                break;
            case NORMAL_PRIORITY:
                SetPriorityClass(_myProcessInfo.hProcess, NORMAL_PRIORITY_CLASS);
                break;
            case REALTIME_PRIORITY:
                SetPriorityClass(_myProcessInfo.hProcess, REALTIME_PRIORITY_CLASS);
                break;
            default:
                throw asl::Exception(string("### ERROR: unknonw thread priority: ") +  asl::as_string(thePrio),
                                     PLUS_FILE_LINE);            
        }        
    }

    bool
    Task::waitForInputIdle(unsigned theTimeout) {
        DWORD myResult = WaitForInputIdle(_myProcessInfo.hProcess, theTimeout);
        if (myResult == WAIT_FAILED) {
            throw asl::Exception("Sorry, unknown error, while waiting for a task to take input.", PLUS_FILE_LINE);
        }
        if (myResult == WAIT_TIMEOUT ) {
        	AC_ERROR << "Task::waitForInputIdle: timeout reached";
        }
        return (myResult == 0); // alles super
    }

    HWND 
    Task::getActiveWindow() const {
        GUITHREADINFO myGuiThreadInfo;
        myGuiThreadInfo.cbSize = sizeof(GUITHREADINFO);
        GetGUIThreadInfo(_myProcessInfo.dwThreadId, &myGuiThreadInfo);                        
        
        HWND myActiveWindow = myGuiThreadInfo.hwndActive;
        if (!myActiveWindow) {
            collectWindows();

            // Find last visible window            
            for (unsigned i = 0; i < _myTaskWindows.size(); ++i) {
                if (IsWindowVisible(_myTaskWindows[i])) {
                    myActiveWindow = _myTaskWindows[i];
                }
            }
        }

        return myActiveWindow;
    }    

    bool
    Task::isActive() const {
        // First check if thread is still active
        DWORD myStatus;
        GetExitCodeThread(_myProcessInfo.hThread, &myStatus);
        if (myStatus == STILL_ACTIVE) {
            return true;
        }

        // Second check if there is still an active window
        return (getActiveWindow() != NULL);
    }

    void 
    Task::captureDesktop(string theFilename) {
        TaskWindow myDesktop(GetDesktopWindow());
        myDesktop.capture(theFilename);
    }

    string
    Task::getActiveWindowName() const {
        return getWindowName(getActiveWindow());
    }

    vector<TaskWindowPtr>
    Task::getWindows() const {
        // TODO: Recycle windows
        vector<TaskWindowPtr> myWindows;
        collectWindows();
        for (unsigned i = 0; i < _myTaskWindows.size(); ++i) {
            myWindows.push_back(TaskWindowPtr(new TaskWindow(_myTaskWindows[i])));
        }

        return myWindows;
    }

}
