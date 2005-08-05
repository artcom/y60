//=============================================================================
// Copyright (C) 2003-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SDLWindow.cpp,v $
//     $Author: pavel $
//   $Revision: 1.33 $
//       $Date: 2005/04/24 00:41:20 $
//
//=============================================================================

#include "SDLWindow.h"
#include "SDLTextRenderer.h"
#include <y60/JScppUtils.h>

#include <SDL/SDL.h>

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#include <SDL/SDL_syswm.h>

#include <iostream>
#include <asl/numeric_functions.h>
#include <asl/GeometryUtils.h>
#include <asl/Box.h>

#include <y60/JSBox.h>
#include <y60/JSLine.h>
#include <y60/JSTriangle.h>
#include <y60/JSSphere.h>
#include <y60/JSVector.h>
#include <y60/JSMatrix.h>
#include <y60/JSNode.h>
#include <y60/JSNodeList.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/JSApp.h>
#include <y60/GLAlloc.h>
#include <y60/ShaderLibrary.h>

using namespace std;
using namespace y60;

#define DB(x) //x

DEFINE_EXCEPTION(SDLWindowException, asl::Exception);

asl::Ptr<SDLWindow>
SDLWindow::create() {
    asl::Ptr<SDLWindow> newWindow = asl::Ptr<SDLWindow>(new SDLWindow());
    newWindow->setSelf(newWindow);
    return newWindow;
}

SDLWindow::SDLWindow() :
    AbstractRenderWindow(SDLApp::ShellErrorReporter),
    _myWindowTitle("Y60 Renderer"),
    _myWindowPosX(100), _myWindowPosY(100),
    _myWindowInitFlag(false),
    _myFullscreenFlag(false),
    _myWinDecoFlag(true),
    _myShowMouseCursorFlag(true),
    _myCaptureMouseCursorFlag(false),
    _myScreen(0),
    _myInitialWidth(800),
    _myInitialHeight(600),
    _myUserDefinedCursor(0),
    _myStandardCursor(0),
    _myAutoPauseFlag(false)
{
}

TTFTextRendererPtr
SDLWindow::createTTFRenderer() {
    return TTFTextRendererPtr(new SDLTextRenderer());
}

void
SDLWindow::activateGLContext() {
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        ensureSDLSubsystem();
#ifdef WIN32
        unsigned int myFlags = SDL_OPENGL | SDL_NOFRAME;
#else
        unsigned int myFlags = SDL_OPENGL;
#endif
        SDL_SetVideoMode(1, 1, 32, myFlags);
    }
}

void
SDLWindow::deactivateGLContext() {
}

int
SDLWindow::getWidth() const {
    return _myScreen ? _myScreen->w : 0;
}

int
SDLWindow::getHeight() const {
    return _myScreen ? _myScreen->h : 0;
}

void
SDLWindow::postRender() {
    AbstractRenderWindow::postRender();
    MAKE_SCOPE_TIMER(SDL_GL_SwapBuffers);
    SDL_GL_SwapBuffers();
}

void
SDLWindow::onResize(y60::Event & theEvent) {
    y60::WindowEvent & myWindowEvent = dynamic_cast<y60::WindowEvent&>(theEvent);
    AC_DEBUG << "Window Resize Event: " << myWindowEvent.width << "x" << myWindowEvent.height;
#ifdef WIN32
    if (!_myWinDecoFlag) {
        // Sorry, in case of a non decorated window sdl seems to use getClientRect for getting the window size.
        // This is wrong, cause decorations are excluded, and so the height and width is not right
        // and the window is too small, use GetWindowRect instead. (VS)
        SDL_SysWMinfo wminfo;
        SDL_VERSION(&wminfo.version);
        LPCTSTR myRenderGirlWindowName = _myWindowTitle.c_str();
        HWND myRenderGirlWindow = FindWindow(0, myRenderGirlWindowName);
        if (myRenderGirlWindow) {
            RECT myRect;
            GetWindowRect(myRenderGirlWindow, &myRect);
            setVideoMode(myRect.right - myRect.left, myRect.bottom - myRect.top);
        } else {
            setVideoMode(myWindowEvent.width, myWindowEvent.height, false);
        }
    } else {
        setVideoMode(myWindowEvent.width, myWindowEvent.height, false);
    }
#else
    setVideoMode(myWindowEvent.width, myWindowEvent.height, false);
#endif
    AbstractRenderWindow::onResize(theEvent);
}


/*
    void SDLWindow::resizeTo(unsigned theWidth, unsigned theHeight) {
        if (_myWindowInitFlag && _myRenderer) {
            setVideoMode(theWidth, theHeight);
        }
        _myWindowWidth  = theWidth;
        _myWindowHeight = theHeight;
    }

    void SDLWindow::setFullScreen(bool theFullscreenFlag) {
        if (_myWindowInitFlag && (theFullscreenFlag != _myFullscreenFlag)) {
            _myFullscreenFlag = theFullscreenFlag;
            if (SDL_WM_ToggleFullScreen(_myScreen) == 0) {
                setVideoMode();
            }
        } else {
            _myFullscreenFlag = theFullscreenFlag;
        }
    }
*/

void
SDLWindow::setVideoMode(unsigned theTargetWidth, unsigned theTargetHeight,
                        bool theFullscreenFlag)
{
    DB(AC_TRACE << "setVideoMode(" << theTargetWidth << ", " << theTargetHeight <<
        ", Fullscreen: " << theFullscreenFlag << ")");
    DB(AC_TRACE << "_myWindowInitFlag: " << _myWindowInitFlag);
    if (!_myWindowInitFlag) {
        // window not yet set up, defer to later
        _myInitialWidth = theTargetWidth;
        _myInitialHeight = theTargetHeight;
    } else {
        unsigned int myFlags = SDL_OPENGL;

        if (!_myWinDecoFlag) {
            myFlags |= SDL_NOFRAME;
        }
        if (theFullscreenFlag) {
            myFlags |= SDL_FULLSCREEN;
        } else {
            myFlags |= SDL_RESIZABLE;
        }

        if (theTargetWidth == 0) {
            theTargetWidth = getWidth();
            AC_DEBUG << "keeping width=" << theTargetWidth;
        }
        if (theTargetHeight == 0) {
            theTargetHeight = getHeight();
            AC_DEBUG << "keeping height=" << theTargetHeight;
        }

#ifdef LINUX
        SDL_SysWMinfo wminfo;
        SDL_VERSION(&wminfo.version);
        if (SDL_GetWMInfo(&wminfo) >= 0) {
            wminfo.info.x11.lock_func();
            XSync(wminfo.info.x11.display, true);
            wminfo.info.x11.unlock_func();
        }
#endif

        DB(AC_TRACE << "SDL_SetVideoMode(" << theTargetWidth << ", " << theTargetHeight <<
                ", FS=" << theFullscreenFlag << ", Deco=" << _myWinDecoFlag << ")" << endl);
        if ((_myScreen = SDL_SetVideoMode(theTargetWidth, theTargetHeight, 32, myFlags)) ==
                NULL)
        {
            throw SDLWindowException(string("Couldn't set SDL-GL mode: ") + SDL_GetError(), PLUS_FILE_LINE);
        }

        // if we are resetting the video mode (e.g. Fullscreen toggle), then the GL context
        // will be lost. Reinit GL and setup the textures again
        if (_myRenderer) {
            _myRenderer->initGL();
            // XXX is this necessary?
            ShaderLibraryPtr myShaderLibrary = dynamic_cast_Ptr<ShaderLibrary> (_myRenderer->getShaderLibrary());
            myShaderLibrary->reload();
            asl::VertexMemory()->enableDMA();
            //_myRenderer->reshape(getWidth(), getHeight());
        }
        if (_myScene) {
            _myScene->getTextureManager()->setupTextures();
        }
    }
    _myFullscreenFlag = theFullscreenFlag;
}

void
SDLWindow::initDisplay() {
    ensureSDLSubsystem();
    _myWindowInitFlag = true;

    setVideoMode(_myInitialWidth, _myInitialHeight, _myFullscreenFlag);
    setWindowTitle("Y60 Renderer");

    if (!_myFullscreenFlag) {
        setPosition(asl::Vector2i(_myWindowPosX, _myWindowPosY));
    }
    //dumpSDLGLParams();

    y60::EventDispatcher::get().addSource(&_mySDLEventSource);
#if WIN32
#if ENABLE_DINPUT
    y60::EventDispatcher::get().addSource(&_myDIEventSource);
#endif
#endif
    // Uncomment this to get output on the events that pass through the event queue.
    // y60::EventDispatcher::get().addSink(&_myEventDumper);
    y60::EventDispatcher::get().addSink(this);

#ifdef LINUX
    if (getenv("__GL_SYNC_TO_VBLANK") == 0) {
        AC_WARNING << "__GL_SYNC_TO_VBLANK not set." << endl;
    }
#endif

    AC_INFO << "GL Version  : " << glGetString(GL_VERSION)<< endl;
    AC_INFO << "   Vendor   : " << glGetString(GL_VENDOR)<< endl;
    AC_INFO << "   Renderer : " << glGetString(GL_RENDERER)<< endl;
    AC_DEBUG << "   Extensions : " << glGetString(GL_EXTENSIONS)<< endl;

    // retrieve standard cursor
    _myStandardCursor =  SDL_GetCursor();
}

void
SDLWindow::dumpSDLGLParams() {
    int value;
    AC_PRINT << "SDL display parameters used: "<< endl;
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
    AC_PRINT << "  SDL_GL_RED_SIZE = " << value << endl;
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE,  &value);
    AC_PRINT << "  SDL_GL_GREEN_SIZE = " << value << endl;
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE , &value);
    AC_PRINT << "  SDL_GL_BLUE_SIZE = " << value << endl;
    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE , &value);
    AC_PRINT << "  SDL_GL_ALPHA_SIZE = " << value << endl;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE , &value);
    AC_PRINT << "  SDL_GL_DEPTH_SIZE = " << value << endl;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER , &value);
    AC_PRINT << "  SDL_GL_DOUBLEBUFFER = " << value << endl;
    SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE , &value);
    AC_PRINT << "  SDL_GL_BUFFER_SIZE = " << value << endl;
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE , &value);
    AC_PRINT << "  SDL_GL_STENCIL_SIZE = " << value << endl;
}

asl::Ptr<y60::OffscreenBuffer>
SDLWindow::createOffscreenBuffer(unsigned long theWidth, unsigned long theHeight, const std::string & theFormat) {
    if (theWidth != 0 && theHeight !=0) {
        asl::Ptr<y60::OffscreenBuffer> myResult = asl::Ptr<y60::OffscreenBuffer>(new y60::OffscreenBuffer(theWidth, theHeight));
        if (myResult->ok()) {
            return myResult;
        }
    }
    return asl::Ptr<y60::OffscreenBuffer>(0);
}

void
SDLWindow::resetCursor() {
    if (_myStandardCursor) {
        SDL_SetCursor(_myStandardCursor);
    }
}

// <CursorDesc data="[]" size="[]", hotSize="[]" />
void
SDLWindow::createCursor(dom::NodePtr & theCursorInfo) {
    if (_myUserDefinedCursor) {
        SDL_FreeCursor(_myUserDefinedCursor);
    }

    //      Data / Mask Resulting pixel on screen
    //      0 / 1 White
    //      1 / 1 Black
    //      0 / 0 Transparent
    //      1 / 0 Inverted color if possible, black if not.
    //
    //      'X' = black
    //      ' ' = transparent
    //      '.' = white
    std::string image = theCursorInfo->getAttributeString("data");

    asl::Vector2i mySize = theCursorInfo->getAttributeValue<asl::Vector2i>("size");
    asl::Vector2i myHotSize = theCursorInfo->getAttributeValue<asl::Vector2i>("hotSize");

    if ( (mySize[0] % 8 !=0) || (mySize[1] % 8 !=0) ||(myHotSize[0] % 8 !=0) ||(myHotSize[0] % 8 !=0)) {
        throw SDLWindowException(string("Sorry, cursor dimensions must be dividable by 8: ") + as_string(*theCursorInfo), PLUS_FILE_LINE);
    }

    if (image.size() != mySize[0] * mySize[1]) {
        throw SDLWindowException(string("Sorry, cursor image data is not correct: ") +  as_string(*theCursorInfo), PLUS_FILE_LINE);
    }
    vector<char> myData(4*mySize[0]);
    vector<char> myMask(4*mySize[0]);

    int myIndex = -1;
    for ( int myRow=0; myRow<mySize[0]; ++myRow ) {
        for ( int myCol=0; myCol<mySize[1]; ++myCol ) {
            if ( myCol % 8 ) {
                myData[myIndex] <<= 1;
                myMask[myIndex] <<= 1;
            } else {
                ++myIndex;
                myData[myIndex] = myMask[myIndex] = 0;
            }
            switch (image[((myRow)*mySize[1])+myCol]) {
                case 'X':
                      myData[myIndex] |= 0x01;
                      myMask[myIndex] |= 0x01;
                      break;
                case '.':
                      myMask[myIndex] |= 0x01;
                      break;
                case ' ':
                      break;
            }
        }
    }
    _myUserDefinedCursor= SDL_CreateCursor((Uint8 *)&(myData[0]), (Uint8 *)&(myMask[0]), mySize[0], mySize[1], myHotSize[0], myHotSize[1]);
    SDL_SetCursor(_myUserDefinedCursor);
}

asl::Vector2i
SDLWindow::getScreenSize(unsigned theScreen) const {
    int myWidth  = -1;
    int myHeight = -1;

#ifdef WIN32
    HWND myDesktopWindow = GetDesktopWindow();
    if (myDesktopWindow) {
        RECT myWindowRect;
        GetWindowRect(myDesktopWindow, &myWindowRect);
        myWidth  = myWindowRect.right - myWindowRect.left;
        myHeight = myWindowRect.bottom - myWindowRect.top;
    }
#else
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    if (SDL_GetWMInfo(&wminfo) < 0) {
        AC_ERROR << "SDL_GetWMInfo: " << SDL_GetError() << endl;
    } else {
        Display * myDisplay = wminfo.info.x11.display;
        myWidth  = DisplayWidth(myDisplay, theScreen);
        myHeight = DisplayHeight(myDisplay, theScreen);
    }
#endif
    if (myWidth >= 0 && myHeight >= 0) {
        AC_DEBUG << "screen size=" << myWidth << "x" << myHeight;
    }
    return asl::Vector2i(myWidth, myHeight);
}

void SDLWindow::setPosition(asl::Vector2i thePos) {
    _myWindowPosX = thePos[0];
    _myWindowPosY = thePos[1];

    if (_myWindowInitFlag == false) {
        // _myWindowPosX/_myWindowPosY will be used, when window is opened
        return;
    }

    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    if (SDL_GetWMInfo(&wminfo) < 0) {
        AC_ERROR << "SDL_GetWMInfo: " << SDL_GetError() << endl;
        return;
    }
#ifdef WIN32
    LPCTSTR myRenderGirlWindowName = _myWindowTitle.c_str();
    HWND myRenderGirlWindow = FindWindow(0, myRenderGirlWindowName);
    if (myRenderGirlWindow != wminfo.window) {
        AC_ERROR << "Windows don't match!\n";
        myRenderGirlWindow = wminfo.window;
    }
    if (myRenderGirlWindow) {
        RECT myRect;
        GetWindowRect(myRenderGirlWindow, &myRect);
        MoveWindow(myRenderGirlWindow, thePos[0], thePos[1],
                   myRect.right-myRect.left,
                   myRect.bottom-myRect.top, true);
    }
#else
    wminfo.info.x11.lock_func();
    XMoveWindow(wminfo.info.x11.display, wminfo.info.x11.wmwindow,
                thePos[0], thePos[1]);
    XSync(wminfo.info.x11.display, false);
    wminfo.info.x11.unlock_func();
#endif
}

void SDLWindow::setWindowTitle(const std::string & theTitle) {
    _myWindowTitle = theTitle;
    SDL_WM_SetCaption(theTitle.c_str(), 0);
}

const std::string &
SDLWindow::getWindowTitle() const {
    return _myWindowTitle;
}

void SDLWindow::setWinDeco(bool theWinDecoFlag) {
    if (_myWindowInitFlag && (theWinDecoFlag != _myWinDecoFlag)) {
        _myWinDecoFlag = theWinDecoFlag;
        setVideoMode();
    } else {
        _myWinDecoFlag = theWinDecoFlag;
    }
}

bool SDLWindow::getWinDeco() {
    return _myWinDecoFlag;
}

bool SDLWindow::getFullScreen() {
    return _myFullscreenFlag;
}

void SDLWindow::ensureSDLSubsystem() {
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1) {
            throw SDLWindowException(string("Could not init SDL video subsystem: ") + SDL_GetError(), PLUS_FILE_LINE);
        }
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    }
}

void SDLWindow::setShowMouseCursor(bool theShowMouseCursor) {
    SDL_ShowCursor(theShowMouseCursor);
    _myShowMouseCursorFlag = theShowMouseCursor;
}

void
SDLWindow::setShowTaskbar(bool theFlag) {
#ifdef WIN32
    HWND myWindowHandle = FindWindow("Shell_TrayWnd", 0);
    ShowWindow(myWindowHandle, (theFlag ? SW_SHOW : SW_HIDE));
#endif
}

bool
SDLWindow::getShowTaskbar() const {
#ifdef WIN32
    HWND myWindowHandle = FindWindow("Shell_TrayWnd", 0);
    return (GetWindowLong(myWindowHandle, GWL_STYLE) & WS_VISIBLE);
#else
    return true;
#endif
}

void SDLWindow::setMousePosition(int theX, int theY) {
    SDL_WarpMouse(theX, theY);
}

void
SDLWindow::setCaptureMouseCursor(bool theCaptureFlag) {
    if (theCaptureFlag == _myCaptureMouseCursorFlag) {
        return;
    }

#ifdef WIN32
    if (theCaptureFlag) {
        LPCTSTR myWindowName = _myWindowTitle.c_str();
        HWND myWindow = FindWindow(0, myWindowName);
        if (myWindow == NULL) {
            AC_ERROR << "Unable to find window '" << _myWindowTitle << "'";
            return;
        }
        RECT myRect;
        GetWindowRect(myWindow, &myRect);
        ClipCursor(&myRect);
    } else {
        ClipCursor(NULL); // free to move
    }
#else
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    if (SDL_GetWMInfo(&wminfo) < 0) {
        AC_ERROR << "SDL_GetWMInfo: " << SDL_GetError() << endl;
        return;
    }
    wminfo.info.x11.lock_func();
    if (theCaptureFlag) {
        XGrabPointer(wminfo.info.x11.display, wminfo.info.x11.wmwindow,
                     True, 0, GrabModeAsync, GrabModeAsync,
                     wminfo.info.x11.wmwindow, None, CurrentTime);
    } else {
        XUngrabPointer(wminfo.info.x11.display, CurrentTime);
    }
    XSync(wminfo.info.x11.display, true);
    wminfo.info.x11.unlock_func();
#endif
    _myCaptureMouseCursorFlag = theCaptureFlag;
}

void
SDLWindow::setAutoPause(bool theAutoPauseFlag) {
    _myAutoPauseFlag = theAutoPauseFlag;
}

void
SDLWindow::handle(y60::EventPtr theEvent) {
    MAKE_SCOPE_TIMER(handleEvents);
    switch (theEvent->type) {
        case y60::Event::QUIT:
            _myAppQuitFlag = true;
            break;
        default:
            AbstractRenderWindow::handle(theEvent);
    }
}

void
SDLWindow::onKey(y60::Event & theEvent) {
    y60::KeyEvent & myKeyEvent = dynamic_cast<y60::KeyEvent&>(theEvent);

    // Hardcoded key event hanlder
    if ((myKeyEvent.keyString == "q" && myKeyEvent.modifiers & y60::KEYMOD_CTRL) ||
        (myKeyEvent.keyString == "f4" && myKeyEvent.modifiers & y60::KEYMOD_ALT)) {
        _myAppQuitFlag = true;
    }

    AbstractRenderWindow::onKey(theEvent);
}

void
SDLWindow::mainLoop() {
    _myAppQuitFlag = false;
    // do one update before entering the mainloop. This ensures everything
    // is up to date during first onIdle
    if (_myRenderer) {
        _myRenderer->getCurrentScene()->updateAllModified();
    }

    while ( ! _myAppQuitFlag ) {
#ifdef WIN32
        bool isOnTop = false;
        if (_myAutoPauseFlag) {
            LPCTSTR myRenderGirlWindowName = _myWindowTitle.c_str();
            HWND myTopWindow        = GetForegroundWindow();
            HWND myRenderGirlWindow = FindWindow(0, myRenderGirlWindowName);
            isOnTop = (myTopWindow == myRenderGirlWindow);
        }
        asl::Time myStartFrameTime;
#endif
        STOP_TIMER(frames);
        asl::getDashboard().cycle();
        START_TIMER(frames);

        onIdle();

        START_TIMER(dispatchEvents);
         y60::EventDispatcher::get().dispatch();
        STOP_TIMER(dispatchEvents);

        START_TIMER(handleRequests);
        _myRequestManager.handleRequests();
        STOP_TIMER(handleRequests);

        if (_myRenderer && _myRenderer->getCurrentScene()) {
            _myRenderer->getCurrentScene()->updateAllModified();

            preRender();
            render();
            postRender();
        }

        asl::AGPMemoryFlushSingleton::get().resetGLAGPMemoryFlush();
#ifdef WIN32
        long long myFrameTimeInMillis = asl::Time().millis() -  myStartFrameTime.millis();
        if (_myAutoPauseFlag && isOnTop == false) {
            asl::msleep(unsigned long(myFrameTimeInMillis) * 1); // sleep 100% of cpu framtime
        }
#endif
        if (jslib::JSApp::getQuitFlag() == JS_TRUE) {
            _myAppQuitFlag = true;
        }
    }

    jsval argv[1], rval;
    jslib::JSA_CallFunctionName(_myJSContext, _myEventListener, "onExit", 0, argv, &rval);
}

int SDLWindow::go() {
    int myRetValue = -1;
    AC_DEBUG << "SDLWindow::Go" << endl;

    if (!_myJSContext) {
        AC_WARNING << "SDLWindow::Go() - No js context found, please assign an eventHandler to window before calling go" << endl;
        return myRetValue;
    }
    try {
        mainLoop();
        myRetValue = 0;
    } catch (const asl::Exception & ex) {
        AC_ERROR << "Exception caught in SDLWindow::go(): " << ex << endl;
    } catch (const exception & ex) {
        AC_ERROR << "Exception caught in SDLWindow::go(): " << ex.what() << endl;
    } catch (...) {
        AC_ERROR << "Unknown exception in SDLWindow::go()" << endl;
    }
    // release mouse capture
    setCaptureMouseCursor(false);
    return myRetValue;
}

/*
void SDLWindow::setViewport(float theMinX, float theMinY, float theMaxX, float theMaxY) {
        asl::Box2f myBox(theMinX, theMinY, theMaxX, theMaxY);
    if (_myRenderer) {
        _myRenderer->setViewport(myBox);
    }
}
*/

// DS: DTM legacy code .... think we should (re-) move this ...
#if 0
template <class T>
bool getProperty(JSContext * cx, JSObject * obj, const char * thePropertyName, T & theValue) {
    jsval myVal;
    IF_NOISY(AC_TRACE << "getProperty:" << thePropertyName << endl);
    if (!JS_GetProperty(cx,obj,thePropertyName,&myVal)) {
        JS_ReportError(cx,"getProperty: not found expected property with name '%s'", thePropertyName);
        return false;
    }
    if (!convertFrom(cx, myVal, theValue)) {
        JS_ReportError(cx,"getProperty: property with name '%s' has bad type", thePropertyName);
        return false;
    }
    IF_NOISY(AC_TRACE << "getProperty ok:" << thePropertyName << endl);
    return true;
}

void SDLWindow::calcWavePosition(float deltaT, Anchor & theAnchor, float theTime,
        const asl::Vector3f & theWave1Direction, float theWave1Frequency, float theWave1Amplitude, float theWave1Speed,
        const asl::Vector3f & theWave2Direction, float theWave2Frequency, float theWave2Amplitude, float theWave2Speed)
{
    theAnchor.waveHeight = 0;

    float d = dot(asVector(theAnchor.globalPosition),theWave1Direction);
    d = d * theWave1Frequency + fmod(theTime * theWave1Frequency * theWave1Speed, float(2*asl::PI));
    theAnchor.waveHeight += theWave1Amplitude * sin(d);

    d = dot(asVector(theAnchor.globalPosition),theWave2Direction);
    d = d * theWave2Frequency + fmod(theTime * theWave2Frequency * theWave2Speed, float(2*asl::PI));
    theAnchor.waveHeight += theWave2Amplitude * sin(d);

    theAnchor.globalPosition[1] = theAnchor.waveHeight;
}

bool SDLWindow::floatBody(float theTime,
    float theIntensity,
    dom::NodePtr theBody,
    JSObject * theFloatProperties,
    JSObject * theWave1Properties,
    JSObject * theWave2Properties) {

    const float SIMULATION_INTERVAL = 0.03f;

    JSContext * cx = _myJSContext;

    // get the float properties first
    float myWeightFactor = 1.0;
    getProperty(cx,theFloatProperties,"myWeightFactor",myWeightFactor);
    float myPitchFactor = 1.0;
    getProperty(cx,theFloatProperties,"myPitchFactor",myPitchFactor);
    float myRollFactor = 1.0;
    getProperty(cx,theFloatProperties,"myRollFactor",myRollFactor);
    asl::Vector3f myAnchor1(-1,0,0);
    getProperty(cx,theFloatProperties,"myAnchor1",myAnchor1);
    asl::Vector3f myAnchor2(0,0,1);
    getProperty(cx,theFloatProperties,"myAnchor2",myAnchor2);
    asl::Vector3f myAnchor3(0,0,-1);
    getProperty(cx,theFloatProperties,"myAnchor3",myAnchor3);
    std::vector<Anchor> myAnchors;
    myAnchors.push_back(Anchor(myAnchor1));
    myAnchors.push_back(Anchor(myAnchor2));
    myAnchors.push_back(Anchor(myAnchor3));

    // now the Wave Properties
    asl::Vector3f myWave1Direction(1,0,0);
    getProperty(cx,theWave1Properties,"direction",myWave1Direction);
    float myWave1Frequency = 1;
    getProperty(cx,theWave1Properties,"frequency",myWave1Frequency);
    float myWave1Amplitude = 1;
    getProperty(cx,theWave1Properties,"amplitude",myWave1Amplitude);
    float myWave1Speed = 1;
    getProperty(cx,theWave1Properties,"waveSpeed",myWave1Speed);

    asl::Vector3f myWave2Direction(1,0,0);
    getProperty(cx,theWave2Properties,"direction",myWave2Direction);
    float myWave2Frequency = 1;
    getProperty(cx,theWave2Properties,"frequency",myWave2Frequency);
    float myWave2Amplitude = 1;
    getProperty(cx,theWave2Properties,"amplitude",myWave2Amplitude);
    float myWave2Speed = 1;
    getProperty(cx,theWave2Properties,"waveSpeed",myWave2Speed);

    // now lets simulate

    // reset roll and pitch, but keep heading
    dom::NodePtr myOrientationAttrib = theBody->getAttribute(y60::ORIENTATION_ATTRIB);
    asl::Vector3f & myOrientationRef = myOrientationAttrib->nodeValueRefOpen<asl::Vector3f>();
    myOrientationRef[0] = 0;
    myOrientationRef[2] = 0;
    // theBody->setAttributeValue<Vector3f>(y60::ORIENTATION_ATTRIB, myOrientation);
    _myRenderer->getCurrentScene()->updateGlobalMatrix(&(*theBody));

    //const asl::Matrix4f & myShipMatrix(theBody->getAttributeValue<asl::Matrix4f>(y60::GLOBAL_MATRIX_ATTRIB));
    y60::BodyPtr myBody = theBody->getFacade<Body>();
    const asl::Matrix4f & myShipMatrix = myBody->get<GlobalMatrixTag>();
    for (int i = 0; i < myAnchors.size(); ++i) {
        float myLastHeight = myAnchors[i].globalPosition[1];
        myAnchors[i].globalPosition = product(myAnchors[i].localPosition, myShipMatrix);
        myAnchors[i].globalPosition[1] = myLastHeight;
    }

    for (int i = 0; i < myAnchors.size(); ++i) {
        calcWavePosition(SIMULATION_INTERVAL, myAnchors[i], theTime,
            myWave1Direction,
            myWave1Frequency, myWave1Amplitude, myWave1Speed,
            myWave2Direction,
            myWave2Frequency, myWave2Amplitude, myWave2Speed);
    }

    // Interpolate height from anchor point positions
    float myHeight = (myAnchors[1].globalPosition[1] + myAnchors[2].globalPosition[1]) / 2;
    myHeight = (myHeight + myAnchors[0].globalPosition[1]) / 2;

    // Set ship height
    dom::NodePtr myShipPositionAttrib = theBody->getAttribute(y60::POSITION_ATTRIB);
    {
        asl::Vector3f & myShipPositionRef = myShipPositionAttrib->nodeValueRefOpen<asl::Vector3f>();
        myShipPositionRef[1] = myHeight;
        myShipPositionAttrib->nodeValueRefClose<asl::Vector3f>();
    }
    // Calculate pitch and roll from anchor points
    //asl::Matrix4f myInvertedShipMatrix = theBody->getAttributeValue<asl::Matrix4f>(y60::GLOBAL_MATRIX_ATTRIB);
    asl::Matrix4f myInvertedShipMatrix = myBody->getInvertedGlobalMatrix();

    std::vector<asl::Point3f> myLocalAnchorPoints;

    myLocalAnchorPoints.push_back(product(myAnchors[0].globalPosition, myInvertedShipMatrix));
    myLocalAnchorPoints.push_back(product(myAnchors[1].globalPosition, myInvertedShipMatrix));
    myLocalAnchorPoints.push_back(product(myAnchors[2].globalPosition, myInvertedShipMatrix));

    asl::Vector3f myNormal(asl::generateFaceNormal<float>(asVector(myLocalAnchorPoints[0]),
        asVector(myLocalAnchorPoints[1]), asVector(myLocalAnchorPoints[2])));
    asl::Vector3f myNormal_XY = normalized(asl::Vector3f(myNormal[0], myNormal[1], 0));
    asl::Vector3f myNormal_YZ = normalized(asl::Vector3f(0, myNormal[1], myNormal[2]));

    float myPitch = asin(myNormal_XY[0]);
    float myRoll  = asin(myNormal_YZ[2]);

    // myOrientation = theBody->getAttributeValue<Vector3f>(y60::ORIENTATION_ATTRIB);
    myOrientationRef[0] = myRoll * theIntensity * myRollFactor;
    myOrientationRef[2] = myPitch * theIntensity * myPitchFactor;
    myOrientationAttrib->nodeValueRefClose<asl::Vector3f>();
    // AC_TRACE << "setting orientation to " << myOrientationRef << endl;
    // theBody->setAttributeValue<Vector3f>(y60::ORIENTATION_ATTRIB, myOrientation);
    return true;
}
#endif // DTM legacy code

bool SDLWindow::getGlyphMetrics(const std::string & theFontName, const std::string & theCharacter,
                         asl::Box2f & theGlyphBox, double & theAdvance)
{
    return _myRenderer->getTextManager().getGlyphMetrics(theFontName, theCharacter, theGlyphBox, theAdvance);
}

double SDLWindow::getKerning(const std::string & theFontName, const std::string & theFirstCharacter, const std::string & theSecondCharacter)
{
    return _myRenderer->getTextManager().getKerning(theFontName, theFirstCharacter, theSecondCharacter);
}

void SDLWindow::draw(const asl::LineSegment<float> & theLine,
              const asl::Vector4f & theColor,
              const asl::Matrix4f & theTransformation)
{
     _myRenderer->draw(theLine, theColor, theTransformation);
}
void SDLWindow::draw(const asl::Box3<float> & theBox, const asl::Vector4f & theColor,
              const asl::Matrix4f & theTransformation)
{
     _myRenderer->draw(theBox, theColor, theTransformation);
}
void SDLWindow::draw(const asl::Sphere<float> & theSphere,
              const asl::Vector4f & theColor,
              const asl::Matrix4f & theTransformation)
{
     _myRenderer->draw(theSphere, theColor, theTransformation);
}
void SDLWindow::draw(const asl::Triangle<float> & theTriangle,
              const asl::Vector4f & theColor,
              const asl::Matrix4f & theTransformation)
{
    _myRenderer->draw(theTriangle, theColor, theTransformation);
}
