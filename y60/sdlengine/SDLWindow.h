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

#ifndef _Y60_ACXPSHELL_SDLWINDOW_INCLUDED_
#define _Y60_ACXPSHELL_SDLWINDOW_INCLUDED_

#include "y60_sdlengine_settings.h"

#include "SDLEventSource.h"
#include "SDLApp.h"

#include <asl/base/settings.h>
#include <asl/base/Exception.h>
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/jssettings.h>
#include <y60/jslib/AbstractRenderWindow.h>
#include <y60/input/KeyEvent.h>
#include <y60/input/WindowEvent.h>

#include <y60/input/DebugEventSink.h>
#include <y60/input/EventDispatcher.h>
#include <y60/input/EventRecorder.h>

namespace y60 {

    class Y60_SDLENGINE_DECL SDLWindow : public jslib::AbstractRenderWindow {

        public:
            static asl::Ptr<SDLWindow> create();

            virtual void swapBuffers();
            virtual void onResize(Event & theEvent);

            void setVideoMode(unsigned theTargetWidth=0, unsigned theTargetHeight=0,
                              bool theFullscreen=false);
            void dumpSDLGLParams();
            void initDisplay();

            // single context system
            virtual void activateGLContext();
            virtual void deactivateGLContext();

            virtual TTFTextRendererPtr createTTFRenderer();
            void resetCursor();
            // <CursorDesc data="[]" size="[]", hotSize="[]" />
            void createCursor(dom::NodePtr & theCursorInfo);

            /// Get desktop/screen size.
            asl::Vector2i getScreenSize(unsigned theScreen = 0) const;

            /// Set/get window position.
            void setPosition(asl::Vector2i thePos);
            asl::Vector2i getPosition() const {
                return asl::Vector2i(_myWindowPosX, _myWindowPosY);
            }

            /// Get window width.
            virtual int getWidth() const;

            /// Get window height.
            virtual int getHeight() const;

            /// Set visibility
            void setVisibility(bool theFlag);

            /// Set/get window title.
            void setWindowTitle(const std::string & theTitle);
            const std::string & getWindowTitle() const;

            void setWinDeco(bool theWinDecoFlag);
            bool getWinDeco();

            bool getFullScreen();

            /// Set/get mouse cursor visibility.
            void setShowMouseCursor(bool theShowMouseCursor);
            bool getShowMouseCursor() const {
                return _myShowMouseCursorFlag;
            }

            // Show/Hide the windows task bar
            void setShowTaskbar(bool theFlag);
            bool getShowTaskbar() const;

            /// Set mouse cursor position.
            void setMousePosition(int theX, int theY);

            /// Set/get mouse capture flag.
            void setCaptureMouseCursor(bool theCaptureFlag);
            bool getCaptureMouseCursor() const {
                return _myCaptureMouseCursorFlag;
            }

            /**
             * Set/get auto-pause flag.
             * If the window is not active (not on-top), the application sleeps for
             * an additional frame (time) to give the foreground application more CPU.
             * @note Windows-only
             */
            void setAutoPause(bool theAutoPauseFlag);
            bool getAutoPause() const {
                return _myAutoPauseFlag;
            }

            virtual void handle(EventPtr theEvent);
            void onKey(Event & theEvent);
            void mainLoop();
            bool go();
            void stop();

            // Sets the number of screen-redraws per swap buffer
            void setSwapInterval(unsigned theInterval);
            int getSwapInterval();

            // Sets the number of multisampling samples (e.g. 1, 2, 4, 8)
            virtual void setMultisamples(unsigned theSampleSize);
            virtual unsigned getMultisamples();

            //////////////////////////////////////////////////////////////////////////

            /// Set EventRecorder mode to STOP/PLAY/RECORD.
            void setEventRecorderMode(EventRecorder::Mode theMode, bool theDiscardFlag);
            EventRecorder::Mode getEventRecorderMode() const;

            /// Load events from file.
            void loadEvents(const std::string & theFilename);

            /// Save recorded events to file.
            void saveEvents(const std::string & theFilename);

            virtual ~SDLWindow();

        private:
            SDLWindow();
            SDLWindow(const SDLWindow &);

            SDLWindow & operator=(const SDLWindow &);

            void initGL();
            void updateSDLVideoMode();

            void ensureVideoInitialized();

            bool _myVideoInitializedFlag;

            unsigned int _myWidth;
            unsigned int _myHeight;

            bool _myFullscreenFlag;
            bool _myDecorationFlag;
            bool _myResizableFlag;

            SDLEventSource  _mySDLEventSource;
            DebugEventSink  _myEventDumper;
            EventRecorder   _myEventRecorder;

            std::string _myWindowTitle;

            int         _myWindowPosX;
            int         _myWindowPosY;

            SDL_Cursor * _myStandardCursor;
            SDL_Cursor * _myUserDefinedCursor;

            bool        _myShowMouseCursorFlag ;
            bool        _myCaptureMouseCursorFlag;

            bool        _myAppQuitFlag;
            bool        _myAutoPauseFlag;

            bool        _myHasVideoSync;

            SDL_Surface * _myScreen;
            unsigned    _mySwapInterval;
            unsigned    _myLastSwapCounter;
            int _myWindowWidthCorrection;

    };

}
#endif
