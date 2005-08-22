//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Task.cpp,v $
//   $Author: valentin $
//   $Revision: 1.14 $
//   $Date: 2005/03/08 10:20:39 $
//
//
//=============================================================================

#include "Task.h"
#include "TaskWindow.h"
#include <asl/Exception.h>
#include <asl/string_functions.h>

#include <asl/Logger.h>
#include <direct.h>

#include <winuser.h>
using namespace std;

#define DB(x) // x

// This allows windows to be set to foreground by other processes
BOOL a = SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
namespace y60 {

    string getWindowName(HWND theWindowHandle) {
        char myWindowName[255];
        GetWindowText(theWindowHandle, myWindowName, 255);
        return string(myWindowName);
    }

    std::string getLastError() {
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
        strcpy(myMessage, (LPCSTR)lpMsgBuf);
        LocalFree(lpMsgBuf);
        return string(myMessage);
    }

    Task::Task() {
        _myProcessInfo.dwThreadId = GetCurrentThreadId();
    }

    Task::Task(std::string theCommand, std::string thePath, bool theShowFlag) {
        DB(cerr << "Create task: " << theCommand << " in path: " << thePath << " state: " << (theShowFlag ? "shown":"hidden") << endl;)

        STARTUPINFO myStartupInfo =
        {
            sizeof(STARTUPINFO),
            NULL, NULL, NULL, 0, 0, 0, 0, 0, 0,
            0, STARTF_USESHOWWINDOW, (theShowFlag ? SW_SHOWDEFAULT : SW_MINIMIZE),
            0, NULL, NULL, NULL, NULL
        };

        bool myResult = CreateProcess(
            NULL, &theCommand[0],
            NULL, NULL, TRUE, 0,
            NULL, thePath.c_str(), &myStartupInfo,
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
    Task::addExternalWindow(std::string theWindowName) {
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
        if (myResult == 0 ) {
        	cout << "Alles super" << endl;
        }
        if (myResult == WAIT_TIMEOUT ) {
        	cout << "timeout reached" << endl;
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
    Task::captureDesktop(std::string theFilename) {
        TaskWindow myDesktop(GetDesktopWindow());
        myDesktop.capture(theFilename);
    }

    std::string
    Task::getActiveWindowName() const {
        return getWindowName(getActiveWindow());
    }

    std::vector<TaskWindowPtr>
    Task::getWindows() const {
        // TODO: Recycle windows
        std::vector<TaskWindowPtr> myWindows;
        collectWindows();
        for (unsigned i = 0; i < _myTaskWindows.size(); ++i) {
            myWindows.push_back(TaskWindowPtr(new TaskWindow(_myTaskWindows[i])));
        }

        return myWindows;
    }

}
