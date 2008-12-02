//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Task.h,v $
//   $Author: valentin $
//   $Revision: 1.12 $
//   $Date: 2005/02/23 15:29:10 $
//
//
//=============================================================================

#ifndef _ac_y60_Task_h_
#define _ac_y60_Task_h_

#include <asl/base/Ptr.h>
#include <string>
#include <vector>
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>

namespace y60 {
            
    enum TaskCallback;
    class TaskWindow;

    class Task {
        public:
            enum PRIORITY {
                ABOVE_NORMAL = -99,
                BELOW_NORMAL,
                HIGH_PRIORITY,
                IDLE_PRIORITY,
                NORMAL_PRIORITY,
                REALTIME_PRIORITY
            };

            Task(std::string theCommand, std::string thePath, bool theShowFlag);
            Task();
            ~Task();
           
            // Terminates a process non-gracefully
            void terminate(); 
            
            // returns if the task is ready for user input or timeout expired
            bool waitForInputIdle(unsigned theTimeout);
            
            /// Takes a screenshot
            void captureDesktop(std::string theFilename); 
            
            bool isActive() const;  
            void setPriority(int thePrio);     

            /// Add windows to the task that do not belong to the process
            void addExternalWindow(std::string theWindowName);

            /// Returns the name of the active window
            std::string getActiveWindowName() const;

            /// Returns all Windows attached to this task
            std::vector<asl::Ptr<TaskWindow> > getWindows() const;

            void addTaskWindow(HWND theWindow) {
                _myTaskWindows.push_back(theWindow);
            }

        private:
            void collectWindows() const;
            HWND getActiveWindow() const;

            PROCESS_INFORMATION       _myProcessInfo;
            mutable std::vector<HWND> _myTaskWindows;
            std::vector<HWND>         _myExternalWindows;
    };
}

#endif
