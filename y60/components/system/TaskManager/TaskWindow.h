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

#ifndef _ac_y60_TaskWindow_h_
#define _ac_y60_TaskWindow_h_

#include "y60_taskmanager_settings.h"

#include <asl/base/Ptr.h>
#include <asl/math/Vector234.h>

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#include <string>

namespace y60 {

    class TaskWindow {
        public:
            TaskWindow();
            TaskWindow(HWND theWindowHandle);
            TaskWindow(std::string theWindowTitle);
            ~TaskWindow();

            /// Window will stay on top
            void setAlwaysOnTop();

            /// Enable function enables or disables mouse and keyboard input to the specified window
            void enable();
            void disable();

            /// Shows/Hides the window completely
            void setVisible(bool theShowFlag);

            /// send window a message that the region of a window is uptodate
            void validateRect();

            /// Closes the window gracefully (ask user to save data, etc.)
            /// Does not work with all applicaitons (e.g. Internet Explorer)
            void close();

            /// Destroy the window non-gracefully
            /// Does not work properly with some applications (e.g. Firefox)
            void destroy();

            /// Same as windows buttons on the top right
            void maximize();
            void minimize();
            void restore();

            /// Brings the window to the foreground and sets the focus
            void activate();

            /// Takes a screenshot
            void capture(std::string theFilename);

            /// Moves/Resizes the window
            void setPosition(const asl::Vector2i & thePosition);
            asl::Vector2i getPosition() const;
            void resize(int theX, int theY);

            /// Sets the alpha of the window
            /// Does not work with OpenGL contexts
            void setAlpha(float theAlpha);

            /// Fades the window to alpha=0 in theTime
            void fade(float theTime);

            /// Hides menubar, title and scrollbar from window
            void hideDecoration();

            /// Returns the title captions
            std::string getName() const;

            /// Returns true, if window is visible
            bool isVisible() const;

            static bool windowExists(std::string theWindowTitle) {
                return 0 != FindWindow(0, theWindowTitle.c_str());
            }

        private:
            HWND _myHandle;
    };

    typedef asl::Ptr<TaskWindow> TaskWindowPtr;
}

#endif
