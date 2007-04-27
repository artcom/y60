//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_TaskWindow_h_
#define _ac_y60_TaskWindow_h_

#include <asl/Ptr.h>
#include <asl/Vector234.h>

#define _WIN32_WINNT 0x0501
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
                return FindWindow(0, theWindowTitle.c_str());
            }

        private:
            HWND _myHandle;
    };

    typedef asl::Ptr<TaskWindow> TaskWindowPtr;
}

#endif
