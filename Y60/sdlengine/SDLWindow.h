//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_SDLWINDOW_INCLUDED_
#define _Y60_ACXPSHELL_SDLWINDOW_INCLUDED_

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

    class SDLWindow : public jslib::AbstractRenderWindow {

        public:
            static asl::Ptr<SDLWindow> create();

            virtual void swapBuffers();
            virtual void onResize(Event & theEvent);

            void setVideoMode(unsigned theTargetWidth=0, unsigned theTargetHeight=0,
                              bool theFullscreen=false, bool theInitializeCallFlag = false);
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
            void ensureSDLSubsystem();

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

            SDLEventSource  _mySDLEventSource;
            DebugEventSink  _myEventDumper;
            EventRecorder   _myEventRecorder;

            std::string _myWindowTitle;

            int         _myWindowPosX;
            int         _myWindowPosY;

            SDL_Cursor * _myStandardCursor;
            SDL_Cursor * _myUserDefinedCursor;

            bool        _myWindowInitFlag;
            bool        _myFullscreenFlag;
            bool        _myWinDecoFlag;
            int         _myInitialWidth;
            int         _myInitialHeight;

            bool        _myShowMouseCursorFlag ;
            bool        _myCaptureMouseCursorFlag;

            bool        _myAppQuitFlag;
            bool        _myAutoPauseFlag;

            bool        _myHasVideoSync;

            SDL_Surface * _myScreen;
            unsigned    _mySwapInterval;
            unsigned    _myLastSwapCounter;
    };

}
#endif
