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

// own header
#include "SDLWindow.h"

#include <y60/glrender/GLResourceManager.h>

#ifdef OSX
#import <Carbon/Carbon.h>
#import "SDLWindowOSXUtils.h"
#endif

#include "SDLTextRenderer.h"

#include <y60/jsbase/JScppUtils.h>

#include <SDL/SDL.h>

#ifdef WIN32
static const int ourMagicDecorationHeight = 15;
#else
static const int ourMagicDecorationHeight = 0;
#endif

#include <SDL/SDL_syswm.h>

#include <iostream>
#include <asl/math/numeric_functions.h>
#include <asl/math/GeometryUtils.h>
#include <asl/math/Box.h>

#include <y60/glrender/GLResourceManager.h>
#include <y60/jsbase/JSBox.h>
#include <y60/jsbase/JSLine.h>
#include <y60/jsbase/JSTriangle.h>
#include <y60/jsbase/JSSphere.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSMatrix.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/jslib/JSApp.h>
#include <y60/jsbase/JScppUtils.h>
#include <y60/glutil/GLAlloc.h>
#include <y60/glrender/ShaderLibrary.h>
#include <y60/jsbase/ArgumentHolder.impl>

#ifdef _WIN32
#include <asl/base/SystemInfo.h>
#endif

#include <asl/xpath/xpath_api.h>

using namespace std;
using namespace y60;

namespace jslib {
    template struct ResultConverter<y60::EventRecorder::Mode>;
}

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

    _myVideoInitializedFlag(false),

    _myWidth(800),
    _myHeight(600),
    _myFullscreenFlag(false),
    _myDecorationFlag(true),
    _myResizableFlag(true), // XXX: backwards compatibility

    _myWindowTitle("Y60 Renderer"),
    _myWindowPosX(100),
    _myWindowPosY(100),

    _myStandardCursor(0),
    _myUserDefinedCursor(0),
    _myShowMouseCursorFlag(true),
    _myCaptureMouseCursorFlag(false),
    _myAppQuitFlag(false),
    _myAutoPauseFlag(false),
    _myHasVideoSync(false),
    _myScreen(0),
    _mySwapInterval(0),
    _myLastSwapCounter(0),
    _myWindowWidthCorrection(0)
{
    setGLContext(GLContextPtr(new GLContext()));

}

SDLWindow::~SDLWindow() {
    AC_DEBUG << "SDLWindow::~SDLWindow";
    if (_myScene) {
        // unbind all created textures while we still have a valid context
        _myScene->getTextureManager()->unbindTextures();
        // We do not have a valid GLContext after SLD_Quit
        _myScene->getTextureManager()->validateGLContext(false);
    }
    SDL_Quit();
}

TTFTextRendererPtr
SDLWindow::createTTFRenderer() {
    return TTFTextRendererPtr(new SDLTextRenderer());
}

void
SDLWindow::activateGLContext() {
    AC_DEBUG << "SDLWindow::activateGLContext";
    ensureVideoInitialized();
}

void
SDLWindow::deactivateGLContext() {
    AC_DEBUG << "SDLWindow::deactivateGLContext";
}

int
SDLWindow::getWidth() const {
    return _myScreen ? _myScreen->w - _myWindowWidthCorrection: 0;
}

int
SDLWindow::getHeight() const {
    return _myScreen ? _myScreen->h : 0;
}


void
SDLWindow::swapBuffers() {
    MAKE_GL_SCOPE_TIMER(SDL_GL_SwapBuffers);
    AbstractRenderWindow::swapBuffers();
#ifdef AC_USE_X11
    if (_myHasVideoSync && _mySwapInterval) {
        unsigned counter;
        glXGetVideoSyncSGI(&counter);
        glXWaitVideoSyncSGI(_mySwapInterval, 0, &counter);
        /*
        if (_myLastSwapCounter == 0) {
            _myLastSwapCounter = counter;
        }
        if ((counter - _myLastSwapCounter) != _mySwapInterval) {
            AC_DEBUG << "Missed frame diff=" << (counter - _myLastSwapCounter);
        }
        */
        _myLastSwapCounter = counter;
    }
#endif
    SDL_GL_SwapBuffers();
}

void
SDLWindow::onResize(Event & theEvent) {
    WindowEvent & myWindowEvent = dynamic_cast<WindowEvent&>(theEvent);
    AC_DEBUG << "Window Resize Event: " << myWindowEvent.width << "x" << myWindowEvent.height;
    if (_myDecorationFlag && !_myFullscreenFlag 
        && (myWindowEvent.width != _myWidth || myWindowEvent.height != _myHeight)) {
        unsigned myHeightOffset = 0;
#ifdef _WIN32
        asl::Vector2i myScreenSize(0,0);
        myScreenSize = getScreenSize();
        // in case of a decorated window, that has a bigger height as the desktop,
        // or uneven height, we need to correct SDL-correction
        if (myWindowEvent.height > (myScreenSize[1] - ourMagicDecorationHeight) || myWindowEvent.height/2.0  != myWindowEvent.height/2) {
            myHeightOffset = 1;
        }
#endif
        setVideoMode(myWindowEvent.width, myWindowEvent.height - myHeightOffset, false);
        AbstractRenderWindow::onResize(theEvent);
    }
    
}

void
SDLWindow::unbindTextures() {
    if (_myScene) {
        // unbind all created textures while we still have a valid context
        _myScene->getTextureManager()->unbindTextures();
    }
}

void
SDLWindow::initGL() {
    if (_myRenderer) {
        _myRenderer->initGL();
        ShaderLibraryPtr myShaderLibrary = dynamic_cast_Ptr<ShaderLibrary> (_myRenderer->getShaderLibrary());
        if (myShaderLibrary) {
            myShaderLibrary->reload();
        }
    }
    // reinit all extensions, because since the context is reset all opengl bound stuff is invalid
    for (ExtensionList::iterator it = _myExtensions.begin(); it != _myExtensions.end(); ++it) {
        IRendererExtensionPtr curExtension(it->lock());
        std::string myName = curExtension->getName() + "::onStartup";
        try {
            curExtension->onStartup(this);
        } catch (const asl::Exception & ex) {
            AC_ERROR << "Exception while calling " << myName << ": " << ex;
        } catch (...) {
            AC_ERROR << "Unknown exception while calling " << myName;
        }
    }

    if (_myScene) {
        _myScene->updateAllModified();
        xpath::NodeList myResult;
        xpath::findAll(xpath::Path(std::string("//") + SHAPE_NODE_NAME), _myScene->getShapesRoot(), myResult);
        for (xpath::NodeList::size_type myIndex = 0; myIndex < myResult.size(); ++myIndex) {
           ShapePtr myShape = myResult[myIndex]->getFacade<Shape>();
           myShape->enforceReload();
        }
    }
}

void
SDLWindow::updateSDLVideoMode() {
    unsigned int myFlags = 0;
    AC_DEBUG << "setting SDL video mode decorations: " << _myDecorationFlag << " resizable: " << _myResizableFlag << " fullscreen: " << _myFullscreenFlag;
    myFlags |= SDL_OPENGL;

    if(!_myDecorationFlag) {
        myFlags |= SDL_NOFRAME;
    }

    if(_myFullscreenFlag) {
        myFlags |= SDL_FULLSCREEN;
    } else {
        if(_myResizableFlag) {
            myFlags |= SDL_RESIZABLE;
        }
    }

#ifdef _WIN32
    // windows 7 does some strange things running a window in fullscreen mode,
    // that means opening a window with no decorations in desktop resolution on
    // ONE monitor. This causes in a black-screen when remote access via any vnc
    // client. 
    // Workaround/Solution->open a window slightly wider than desktop size not
    // disturbing opengl canvas/viewport, but to prevent windows 7 to behave
    // that way. (vs/2011)
    asl::Vector2i myScreenSize = getScreenSize();

    asl::SystemInfo mySystemInfo;
    if (!_myFullscreenFlag && !getenv("DISABLE_WINDOWS_7_FULLSCREEN_WORKAROUND") && 
        mySystemInfo.GetWindowsVersion() == asl::Windows7 && 
        myScreenSize[0] == _myWidth && myScreenSize[1] == _myHeight &&
        _myWindowPosX == 0 && _myWindowPosY == 0) {
        _myWindowWidthCorrection = 1;
        AC_WARNING << "Adapting window width to " << _myWidth + _myWindowWidthCorrection << " to prevent windows 7 to go in some strange fullscreen mode";
    }
#endif

#ifdef AC_USE_X11
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    if (SDL_GetWMInfo(&wminfo) >= 0) {
        wminfo.info.x11.lock_func();
        XSync(wminfo.info.x11.display, true);
        wminfo.info.x11.unlock_func();
    }
#endif

    if ((_myScreen = SDL_SetVideoMode(_myWidth + _myWindowWidthCorrection, _myHeight, 32, myFlags)) == NULL) {
        throw SDLWindowException(string("Couldn't set SDL-GL mode: ") + SDL_GetError(), PLUS_FILE_LINE);
    }

#ifdef AC_USE_X11
    SDL_VERSION(&wminfo.version);
    if (SDL_GetWMInfo(&wminfo) >= 0) {
        wminfo.info.x11.lock_func();
        XSync(wminfo.info.x11.display, true);
        wminfo.info.x11.unlock_func();
    }
#endif
}

void
SDLWindow::setVideoMode(unsigned theTargetWidth, unsigned theTargetHeight,
                        bool theFullscreenFlag)
{
    bool reinitGL = _myFullscreenFlag != theFullscreenFlag; //SDL resets gl context when toggling fullscreen mode
    if (reinitGL || theTargetWidth != _myWidth || theTargetHeight != _myHeight) {
        _myWidth = theTargetWidth;
        _myHeight = theTargetHeight;
        _myFullscreenFlag = theFullscreenFlag;
        if (_myVideoInitializedFlag) {
            if (reinitGL) {
                unbindTextures();
            }
            updateSDLVideoMode();
            if (reinitGL) {
                initGL(); //XXX: handle shader and offscreen rendering here
            }
        }
    }
}

void
SDLWindow::initDisplay() {
    ensureVideoInitialized();

    setWindowTitle("Y60 Renderer");

    if (!_myFullscreenFlag) {
        setPosition(asl::Vector2i(_myWindowPosX, _myWindowPosY));
    }
    //dumpSDLGLParams();

    EventDispatcher::get().addSource(&_mySDLEventSource);
    EventDispatcher::get().addSource(&_myEventRecorder);
    EventDispatcher::get().addSink(&_myEventRecorder);

    // Uncomment this to get output on the events that pass through the event queue.
    // EventDispatcher::get().addSink(&_myEventDumper);
    EventDispatcher::get().addSink(this);

#ifdef LINUX
    if (getenv("__GL_SYNC_TO_VBLANK") == 0) {
        AC_INFO << "__GL_SYNC_TO_VBLANK not set.";
    }
#endif

    AC_INFO  << "GL Version:     " << glGetString(GL_VERSION);
    AC_INFO  << "   Vendor:      " << glGetString(GL_VENDOR);
    AC_INFO  << "   Renderer:    " << glGetString(GL_RENDERER);
    AC_DEBUG << "   Extensions:  " << glGetString(GL_EXTENSIONS);
#ifndef _AC_NO_CG_
    AC_INFO  << "Cg Version:     " << CG_VERSION_NUM;
    AC_INFO  << "Cg Profiles:    " << getLatestCgProfileString();
#endif

    // retrieve standard cursor
    _myStandardCursor =  SDL_GetCursor();
    ShaderLibrary::setGLisReadyFlag(true);
    GLResourceManager::get().loadShaderLibrary();
    getGLContext()->getStateCache()->init();
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
    SDL_GL_GetAttribute(SDL_GL_ACCUM_RED_SIZE , &value);
    AC_PRINT << "  SDL_GL_ACCUM_RED_SIZE = " << value << endl;
    SDL_GL_GetAttribute(SDL_GL_ACCUM_GREEN_SIZE , &value);
    AC_PRINT << "  SDL_GL_ACCUM_GREEN_SIZE = " << value << endl;
    SDL_GL_GetAttribute(SDL_GL_ACCUM_BLUE_SIZE , &value);
    AC_PRINT << "  SDL_GL_ACCUM_BLUE_SIZE = " << value << endl;
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

    if ( static_cast<int>(image.size()) != mySize[0] * mySize[1]) {
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

#ifdef _WIN32
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
#ifdef OSX
        std::string myError = getScreenSizeOSX(myWidth, myHeight);
        if (!myError.empty() ) {
            myWidth = myHeight = -1;
            throw SDLWindowException(myError, PLUS_FILE_LINE);
        }
#else
        Display * myDisplay = wminfo.info.x11.display;
        myWidth  = DisplayWidth(myDisplay, theScreen);
        myHeight = DisplayHeight(myDisplay, theScreen);
#endif
    }
#endif
    if (myWidth >= 0 && myHeight >= 0) {
        AC_DEBUG << "screen size=" << myWidth << "x" << myHeight;
    }
    return asl::Vector2i(myWidth, myHeight);
}
void SDLWindow::setVisibility(bool theFlag) {
    _myVisiblityFlag = theFlag;
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    if (SDL_GetWMInfo(&wminfo) < 0) {
        AC_ERROR << "SDL_GetWMInfo: " << SDL_GetError() << endl;
        return;
    }
#ifdef _WIN32
    LPCTSTR myRenderGirlWindowName = _myWindowTitle.c_str();
    HWND myRenderGirlWindow = FindWindow(0, myRenderGirlWindowName);
    if (myRenderGirlWindow != wminfo.window) {
        AC_ERROR << "Windows don't match!\n";
        myRenderGirlWindow = wminfo.window;
    }
    if (myRenderGirlWindow) {
        //ShowWindow(myRenderGirlWindow, theFlag ? SW_SHOW:SW_HIDE);
        ShowWindow(myRenderGirlWindow, theFlag ? SW_RESTORE:SW_MINIMIZE);
        if (theFlag) {
            SetForegroundWindow(myRenderGirlWindow);
        }

    }
#else
#ifdef OSX // TODO PORT
    AC_WARNING << "SDLWindow::setVisibility not yet implemented for OSX";
#endif
#endif
    AC_TRACE << "SDLWindow::setVisibility : " << theFlag;
    }

void SDLWindow::setPosition(asl::Vector2i thePos) {
    _myWindowPosX = thePos[0];
    _myWindowPosY = thePos[1];

    if (!_myVideoInitializedFlag) {
        // _myWindowPosX/_myWindowPosY will be used, when window is opened
        return;
    }

    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    if (SDL_GetWMInfo(&wminfo) < 0) {
        AC_ERROR << "SDL_GetWMInfo: " << SDL_GetError() << endl;
        return;
    }
#ifdef _WIN32
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
#endif
#ifdef AC_USE_X11
    wminfo.info.x11.lock_func();
    XMoveWindow( wminfo.info.x11.display, wminfo.info.x11.wmwindow,
                 thePos[0], thePos[1]);
    XSync(wminfo.info.x11.display, false);
    wminfo.info.x11.unlock_func();
#endif

#ifdef OSX 
    std::string myError = setWindowPositionOSX(_myWindowPosX, _myWindowPosY);
    if (!myError.empty() ) {
        throw SDLWindowException(myError, PLUS_FILE_LINE);
    }
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
    if (_myDecorationFlag != theWinDecoFlag) {
        _myDecorationFlag = theWinDecoFlag;
        _myResizableFlag = _myDecorationFlag;
        if (_myVideoInitializedFlag) {
            unbindTextures();
            updateSDLVideoMode();
             //SDL resets gl context when toggling fullscreen mode
            initGL(); //XXX: handle shader and offscreen rendering here
        }
    }
}

bool SDLWindow::getWinDeco() {
    return _myDecorationFlag;
}

bool SDLWindow::getFullScreen() {
    return _myFullscreenFlag;
}

void SDLWindow::ensureVideoInitialized() {
    if(!_myVideoInitializedFlag) {
        AC_DEBUG << "Initializing SDL video";

        if (!SDL_WasInit(SDL_INIT_VIDEO)) {
            if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1) {
                throw SDLWindowException(string("Could not init SDL video subsystem: ") + SDL_GetError(), PLUS_FILE_LINE);
            }
        }

        // enable double-buffering
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // component sizes
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        // depth buffer
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        // multisampling
        unsigned mySamples = AbstractRenderWindow::getMultisamples();
        if (mySamples >= 1) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, mySamples);
        }

        _myVideoInitializedFlag = true;

        // set the video mode for the first time
        updateSDLVideoMode();
        initGL();

        // initialize glew
        unsigned int myGlewError = glewInit();
        if (GLEW_OK != myGlewError) {
            throw RendererException(std::string("Glew Initialization Error: ") +
                    std::string(reinterpret_cast<const char*>(glewGetErrorString(myGlewError))),
                    PLUS_FILE_LINE);
        }

        // initialize glrm
        GLResourceManager::get().initCaps();

        // find out if vsync is supported
#ifdef _WIN32
        _myHasVideoSync = 0 != wglewIsSupported("WGL_EXT_swap_control");
#elif LINUX
        _myHasVideoSync = glxewIsSupported("GLX_SGI_video_sync");
#else
        _myHasVideoSync = false;
#endif
    }
}

void
SDLWindow::setMultisamples(unsigned theSampleSize) {
    if (SDL_WasInit(SDL_INIT_VIDEO)) {
        AC_WARNING << "Sorry, setting the multisampling size will take no effect after the sdl window has been initialized!";
        return;
    }
    AbstractRenderWindow::setMultisamples(theSampleSize);
}

unsigned
SDLWindow::getMultisamples() {
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        return AbstractRenderWindow::getMultisamples();
    } else {
        int mySize;
        SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &mySize);
        return unsigned(mySize);
    }
}

void SDLWindow::setShowMouseCursor(bool theShowMouseCursor) {
    SDL_ShowCursor(theShowMouseCursor);
    _myShowMouseCursorFlag = theShowMouseCursor;
}

void
SDLWindow::setShowTaskbar(bool theFlag) {
#ifdef _WIN32
    HWND myWindowHandle = FindWindow("Shell_TrayWnd", 0);
    ShowWindow(myWindowHandle, (theFlag ? SW_SHOW : SW_HIDE));
#endif // _WIN32

#ifdef OSX
    std::string myError = setShowTaskBarOSX(theFlag);
    if (!myError.empty()) {
        throw SDLWindowException(myError, PLUS_FILE_LINE);
    }
#endif // OSX

}

bool
SDLWindow::getShowTaskbar() const {
#ifdef _WIN32
    HWND myWindowHandle = FindWindow("Shell_TrayWnd", 0);
    return 0!= (GetWindowLong(myWindowHandle, GWL_STYLE) & WS_VISIBLE);
#else
    return true;
#endif
}

void SDLWindow::setMousePosition(int theX, int theY) {
    SDL_WarpMouse( static_cast<Uint16>(theX), static_cast<Uint16>(theY) );
}

void
SDLWindow::setCaptureMouseCursor(bool theCaptureFlag) {
    if (theCaptureFlag == _myCaptureMouseCursorFlag) {
        return;
    }

#ifdef _WIN32
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
#endif
#ifdef AC_USE_X11
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
#elif defined( OSX )
#   warning Mac OS X implementation of setCaptureMouseCursor() is missing
#endif
    _myCaptureMouseCursorFlag = theCaptureFlag;
}

void
SDLWindow::setAutoPause(bool theAutoPauseFlag) {
    _myAutoPauseFlag = theAutoPauseFlag;
}

void
SDLWindow::handle(EventPtr theEvent) {
    MAKE_GL_SCOPE_TIMER(handleEvents);
    switch (theEvent->type) {
        case Event::QUIT:
            _myAppQuitFlag = true;
            break;
        default:
            AbstractRenderWindow::handle(theEvent);
    }
}

void
SDLWindow::onKey(Event & theEvent) {
    KeyEvent & myKeyEvent = dynamic_cast<KeyEvent&>(theEvent);

    // Hardcoded key event hanlder
    if ((myKeyEvent.keyString == "q" && myKeyEvent.modifiers & KEYMOD_CTRL) ||
        (myKeyEvent.keyString == "f4" && myKeyEvent.modifiers & KEYMOD_ALT)) {
        _myAppQuitFlag = true;
    }

    AbstractRenderWindow::onKey(theEvent);
}

namespace jslib {
    //in JSRenderWindow
    jsval as_jsval(JSContext *cx, asl::Ptr<SDLWindow> theOwner);

    jsval as_jsval(JSContext *cx, asl::Ptr<AbstractRenderWindow> theOwner) {
        return as_jsval(cx, dynamic_cast_Ptr<SDLWindow>( theOwner));
    }

}

void
SDLWindow::mainLoop() {
    _myAppQuitFlag = false;
    // do one update before entering the mainloop. This ensures everything
    // is up to date during first onFrame
    if (_myRenderer) {
        _myRenderer->getCurrentScene()->updateAllModified();
    }

    if (_myEventListener && jslib::JSA_hasFunction(_myJSContext, _myEventListener, "onStartMainLoop")) {
        jsval argv[1], rval;
        if (!jslib::JSA_CallFunctionName(_myJSContext, _myEventListener, "onStartMainLoop", 0, argv, &rval)) {
            return;
        }
    }

    while ( ! _myAppQuitFlag ) {
#ifdef _WIN32
        bool isOnTop = false;
        if (_myAutoPauseFlag) {
            LPCTSTR myRenderGirlWindowName = _myWindowTitle.c_str();
            HWND myTopWindow        = GetForegroundWindow();
            HWND myRenderGirlWindow = FindWindow(0, myRenderGirlWindowName);
            isOnTop = (myTopWindow == myRenderGirlWindow);
        }
#endif

        START_TIMER(dispatchEvents);
        EventDispatcher::get().dispatch();
        if (JS_IsExceptionPending(_myJSContext)) {
            return;
        }
        STOP_TIMER(dispatchEvents);

        START_TIMER(handleRequests);
        _myRequestManager.handleRequests();
        if (JS_IsExceptionPending(_myJSContext)) {
            return;
        }
        STOP_TIMER(handleRequests);

        // Call onProtoFrame (a second onframe that can be used to automatically run tutorials)
        if ( jslib::JSA_hasFunction(_myJSContext, _myEventListener, "onProtoFrame")) {
            jsval argv[1], rval;
            argv[0] = jslib::as_jsval(_myJSContext, _myElapsedTime);
            if (!jslib::JSA_CallFunctionName(_myJSContext, _myEventListener, "onProtoFrame", 1, argv, &rval)) {
                return;
            }
        }

        onFrame();

        if (_myRenderer && _myRenderer->getCurrentScene()) {
            renderFrame();
        }

        if (_myJSContext) {
            MAKE_GL_SCOPE_TIMER(gc);
            if (getForceFullGC()) {
                JS_GC(_myJSContext);
            } else {
#ifdef USE_SPIDERMONKEY_INCREMENTAL_GC
                JS_AdaptiveGC(_myJSContext);
#else
                JS_GC(_myJSContext);
#endif
            }
        }

        swapBuffers();

        asl::AGPMemoryFlushSingleton::get().resetGLAGPMemoryFlush();

        if (jslib::JSApp::getQuitFlag() == JS_TRUE) {
            _myAppQuitFlag = true;
        }

#ifdef _WIN32
        if (_myAutoPauseFlag && isOnTop == false) {
            unsigned long mySleepDuration = 40; // in millisec
            asl::msleep(mySleepDuration);
        }
#endif

        STOP_TIMER(frames);
        asl::getDashboard().cycle();
        START_TIMER(frames);
    }

    jsval argv[1], rval;
    if (jslib::JSA_hasFunction(_myJSContext, _myEventListener, "onExit")) {
        jslib::JSA_CallFunctionName(_myJSContext, _myEventListener, "onExit", 0, argv, &rval);
    }
}

void
SDLWindow::stop() {
    AC_DEBUG << "SDLWindow::stop" << endl;
    _myAppQuitFlag = true;
}

bool
SDLWindow::go() {
    bool myResult = false;
    AC_DEBUG << "SDLWindow::go";
    if (!_myJSContext) {
        AC_WARNING << "SDLWindow::go() - No js context found, please assign an eventHandler to window before calling go()";
        return myResult;
    }
    try {
        AbstractRenderWindow::go();
        mainLoop();
    } catch (const asl::Exception & ex) {
        AC_ERROR << "Exception caught in SDLWindow::go(): " << ex;
    } catch (const exception & ex) {
        AC_ERROR << "Exception caught in SDLWindow::go(): " << ex.what();
    } catch (const PLTextException & ex) {
        AC_ERROR << "Exception caught in SDLWindow::go(): " << ex;
    } catch (...) {
        AC_ERROR << "Unknown exception in SDLWindow::go()";
    }
    // release mouse capture
    setCaptureMouseCursor(false);
    return !JS_IsExceptionPending(_myJSContext);
}

void SDLWindow::setEventRecorderMode(EventRecorder::Mode theMode, bool theDiscardFlag) {
    _myEventRecorder.setMode(theMode, theDiscardFlag);
}

EventRecorder::Mode SDLWindow::getEventRecorderMode() const {
    return _myEventRecorder.getMode();
}

void SDLWindow::loadEvents(const std::string & theFilename) {
    _myEventRecorder.load(theFilename);
}

void SDLWindow::saveEvents(const std::string & theFilename) {
    _myEventRecorder.save(theFilename);
}


void
SDLWindow::setSwapInterval(unsigned theInterval)
{
    if (!_myRenderer) {
        AC_WARNING << "Sorry, setting the swap interval will take no effect before the renderer is created!";
        return;
    }

#ifdef _WIN32
    if (_myHasVideoSync) {
        bool myCardIsATI =  getGLVendorString().find("ATI") != string::npos; 
        if (myCardIsATI && theInterval > 1 ) {
            // take a look at http://www.opengl.org/registry/specs/EXT/swap_control.txt
            AC_WARNING << "Sorry, ATI-Graphiccards most likely have a MAX_SWAP_INTERVAL of 1 (at least by now Oct. 2010), \n" <<
                          "so setting a value greater than 1, in this case '#" << theInterval <<
                          "', may result in no swap at all.";
        }
        wglSwapIntervalEXT(theInterval);
        _mySwapInterval = wglGetSwapIntervalEXT();
        if (theInterval != _mySwapInterval) {
            AC_WARNING << "Cannot set swap interval." <<
                          "Vertical sync must must be set to 'application controlled' " <<
                          "in driver settings. Using " << _mySwapInterval;
        }
    } else {
        AC_WARNING << "setSwapInterval(): wglSwapInterval not supported.";
    }
#endif
#ifdef AC_USE_X11
    if (_myHasVideoSync) {
        if (_mySwapInterval == 0 && theInterval != 0) {
            // check if it's working
            unsigned counter0 = 0;
            unsigned counter1 = 0;
            // this is broken on nvidia 8800 GTX with driver 1.0-9746
            glXWaitVideoSyncSGI(1, 0, &counter0);
            glXWaitVideoSyncSGI(1, 0, &counter1);
            glXWaitVideoSyncSGI(1, 0, &counter1);
            if (counter1 <= counter0) {
                AC_WARNING << "setSwapInterval(): glXGetVideoSyncSGI not working properly (counter0=" << counter0 << ", counter1=" << counter1 << "), disabling";
                theInterval = 0;
                // glXGetVideoSyncSGI = 0;
                // glXWaitVideoSyncSGI = 0;
            } else {
                AC_INFO << "setSwapInterval(): glXGetVideoSyncSGI working properly";
            }
        }
        _mySwapInterval = theInterval;
    } else {
        AC_WARNING << "setSwapInterval(): glXGetVideoSyncSGI not supported";
        _mySwapInterval = 0;
    }
#endif
#ifdef AC_USE_OSX_CGL
    const GLint myInterval = theInterval;
    CGLError myError = CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &myInterval);
    if (myError != 0) {
        AC_WARNING << "Cannot set swap interval, error=" << CGLErrorString(myError) << ", " <<
            "Vertical sync must must be set to 'application controlled' " <<
            "in driver settings. Using " << _mySwapInterval;
    } else {
        _mySwapInterval = theInterval;
    }
#endif
}

int
SDLWindow::getSwapInterval() {
    if (!_myRenderer) {
        AC_WARNING << "Sorry, getting the swap interval will no work before the renderer is created!" << endl;
        return 0;
    }

#ifdef _WIN32
    if (_myHasVideoSync) {
        return wglGetSwapIntervalEXT();
    } else {
        AC_WARNING << "getSwapInterval(): wglSwapInterval Extension not supported.";
    }
    return 0;
#else
    return _mySwapInterval;
#endif
}

