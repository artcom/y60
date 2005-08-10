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
//    $RCSfile: SDLWindow.h,v $
//     $Author: pavel $
//   $Revision: 1.21 $
//       $Date: 2005/04/24 00:41:20 $
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_SDLWINDOW_INCLUDED_
#define _Y60_ACXPSHELL_SDLWINDOW_INCLUDED_

//#define ENABLE_DINPUT

#include "SDLEventSource.h"
#include "SDLApp.h"

#include <asl/settings.h>
#include <asl/Exception.h>

#include <y60/jssettings.h>
#include <y60/JScppUtils.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/KeyEvent.h>
#include <y60/WindowEvent.h>

#ifdef ENABLE_DINPUT
#include <y60/DIEventSource.h>
#endif

#include <y60/DebugEventSink.h>
#include <y60/EventDispatcher.h>

class SDLWindow : public jslib::AbstractRenderWindow {

public:
    static asl::Ptr<SDLWindow> create();

    void printHelp() {
        AC_PRINT << "Y60 Renderer copyright (c) 2003 ART+COM AG" << std::endl;
    }

    void postRender();
    virtual void onResize(y60::Event & theEvent);

    void setVideoMode(unsigned theTargetWidth=0, unsigned theTargetHeight=0,
                      bool theFullscreen=false);
    void dumpSDLGLParams();
    void initDisplay();
    // single context system
    virtual void activateGLContext();
    virtual void deactivateGLContext();

    virtual y60::TTFTextRendererPtr createTTFRenderer();
    void resetCursor();
    // <CursorDesc data="[]" size="[]", hotSize="[]" />
    void createCursor(dom::NodePtr & theCursorInfo);
    bool getSleepMode();
    void setSleepMode(bool theFlag);

    // Get desktop/screen size.
    asl::Vector2i getScreenSize(unsigned theScreen = 0) const;

    /// Set/get window position.
    void setPosition(asl::Vector2i thePos);
    asl::Vector2i getPosition() const {
        return asl::Vector2i(_myWindowPosX, _myWindowPosY);
    }

    /// Get window width.
    int getWidth() const;
 
    /// Get window height.
    int getHeight() const;

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

    virtual void handle(y60::EventPtr theEvent);
    void onKey(y60::Event & theEvent);
    void mainLoop();
    int go();
    // void setViewport(float theMinX, float theMinY, float theMaxX, float theMaxY);

// DTM legacy code
#if 0
    struct Anchor {
        Anchor(const asl::Point3f & thePosition) : localPosition(thePosition),
            globalPosition(0,0,0), myVelocity(0), waveHeight(0) {};
        asl::Point3f  localPosition;
        asl::Point3f  globalPosition;
        float         myVelocity;
        float         waveHeight;
    };

    void calcWavePosition(float deltaT, Anchor & theAnchor, float theTime,
        const asl::Vector3f & theWave1Direction, float theWave1Frequency, float theWave1Amplitude, float theWave1Speed,
        const asl::Vector3f & theWave2Direction, float theWave2Frequency, float theWave2Amplitude, float theWave2Speed);

    bool floatBody(float theTime,
        float theIntensity,
        dom::NodePtr theBody,
        JSObject * theFloatProperties,
        JSObject * theWave1Properties,
        JSObject * theWave2Properties);
#endif
    // ////////////////////////////////////////////////////////////////////////

    bool getGlyphMetrics(const std::string & theFontName, const std::string & theCharacter,
                         asl::Box2f & theGlyphBox, double & theAdvance);

    double getKerning(const std::string & theFontName, const std::string & theFirstCharacter, const std::string & theSecondCharacter);

    void draw(const asl::LineSegment<float> & theLine,
              const asl::Vector4f & theColor,
              const asl::Matrix4f & theTransformation);
    void draw(const asl::Box3<float> & theBox, const asl::Vector4f & theColor,
              const asl::Matrix4f & theTransformation);
    void draw(const asl::Sphere<float> & theSphere,
              const asl::Vector4f & theColor,
              const asl::Matrix4f & theTransformation);
    void draw(const asl::Triangle<float> & theTriangle,
              const asl::Vector4f & theColor,
              const asl::Matrix4f & theTransformation);

private:
    SDLWindow();
    SDLWindow(const SDLWindow &);
    SDLWindow & operator=(const SDLWindow &);

    SDLEventSource       _mySDLEventSource;
    y60::DebugEventSink  _myEventDumper;
#ifdef WIN32
#ifdef ENABLE_DINPUT
    y60::DIEventSource   _myDIEventSource;
#endif
#endif

    std::string _myWindowTitle  ;

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

    // KeyMap      _myPressedKeyMap;
    SDL_Surface * _myScreen;
};

#endif

