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
#include "AbstractRenderWindow.h"

#include "JSApp.h"
#include <y60/jsbase/JSNode.h>

#include <y60/input/WindowEvent.h>
#include <y60/input/KeyEvent.h>
#include <y60/input/MouseEvent.h>
#include <y60/input/AxisEvent.h>
#include <y60/input/ButtonEvent.h>
#include <y60/input/TouchEvent.h>
#include <y60/input/GenericEvent.h>
#include <y60/glutil/ScopedGLContext.h>
#include <y60/image/Image.h>
#include <y60/video/Movie.h>
#include <y60/video/Capture.h>
#include <y60/glutil/GLBufferAdapter.h>

#include <asl/base/file_functions.h>
#include <asl/base/os_functions.h>
#include <asl/base/PlugInManager.h>
#include <asl/base/Ptr.h>
#include <asl/base/StdOutputRedirector.h>
#include <asl/dom/Nodes.h>
#include "GLContextRegistry.h"

using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) // x

namespace jslib {

    AbstractRenderWindow::AbstractRenderWindow(const JSErrorReporter & theErrorReporter) :
// _myScene(0),
// _myCanvas(0),
// _myRenderer(0),
_myEventListener(0),
_myJSContext(0),
_myErrorReporter(theErrorReporter),
_myRenderingCaps(static_cast<unsigned int>(~0)), // enable all (~)
_myMultisamples(0),
_myElapsedTime(0.0),
_myVisiblityFlag(true),
_myFixedDeltaT(0.0),
_myStartTime(-1.0),
_myPauseTime(0.0),
_myPauseFlag(false),
_myForceFullGC(false)
{}

void
AbstractRenderWindow::setSelf(asl::Ptr<AbstractRenderWindow> theSelfPtr) {
    _mySelf = theSelfPtr;
    GLContextRegistry::get().registerContext(_mySelf);
}

AbstractRenderWindow::~AbstractRenderWindow() {
    GLContextRegistry::get().unregisterContext(_mySelf);
}

void
AbstractRenderWindow::ensureScene() {
    if (!_myScene) {
        // Add an empty default scene.
        // The defaultscene can be overwritten with setScene.
        // It is created without an OpenGL context and therefore the scene data
        // is not added to the graphics-memory.
        // To startup the renderer and copy the scene data into the gl-memory
        // you must call go()
        PackageManagerPtr myPackageManager = JSApp::getPackageManager();
        _myScene = y60::Scene::createStubs(myPackageManager);

        //_myScene = y60::ScenePtr(new y60::Scene());
        //_myScene->createStubs(JSApp::getPackageManager());
        setCanvas(_myScene->getCanvasRoot()->childNode("canvas"));
    }
}
void AbstractRenderWindow::setVisibility(bool theFlag) {
    AC_WARNING << "AbstractRenderWindow::setVisibility() not implemented.";
}

bool AbstractRenderWindow::getVisibility() const {
    return _myVisiblityFlag;
}

y60::ScenePtr &
AbstractRenderWindow::getCurrentScene() {
    ensureScene();
    return _myScene;
}

const y60::ScenePtr &
AbstractRenderWindow::getCurrentScene() const {
    if (!_myScene) {
        AC_WARNING << "AbstractRenderWindow::getCurrentScene(): Scene not set.";
    }
    return _myScene;
}

bool
AbstractRenderWindow::setScene(const y60::ScenePtr & theScene) {
    return setSceneAndCanvas(theScene, dom::NodePtr());
}

// TODO: do something with the extensions
bool
AbstractRenderWindow::setSceneAndCanvas(const y60::ScenePtr & theScene, const dom::NodePtr & theCanvas) {
    if (_mySelf.expired()) {
        throw asl::Exception("ERROR: AbstractRenderWindow::_mySelf is 0! Did you use the proper factory method?", PLUS_FILE_LINE);
    }
    // Do I already have a scene, then cleanly unbind it from me
    if (_myScene) {
        unsetCanvas();
        AC_INFO << "setScene removed Framebuffer from previously rendered scene";
        _myScene = y60::ScenePtr();
    }
    //if (!theScene) {
    //    throw asl::Exception("ERROR: AbstractRenderWindow::theScene is 0!", PLUS_FILE_LINE);
    //}
    try {
        y60::ScopedGLContext myContextLock(this);
        if (!_myRenderer) {
            initDisplay();
            _myRenderer = RendererPtr(new Renderer(_myGLContext, _myRenderingCaps));
            //AC_INFO << "created Renderer" << _myRenderer;
            _myRenderer->getTextManager().setTTFRenderer(createTTFRenderer());
        }

        _myScene = theScene;
        _myRenderer->setCurrentScene(_myScene);

        if ( theCanvas ) {
            setCanvas(theCanvas);
        } else if (_myScene->getCanvasRoot()->childNodesLength() == 1) {
            //set canvas automatically only if the scene has a single canvas (dk)
            dom::NodePtr myCanvas = _myScene->getCanvasRoot()->childNode("canvas");
            setCanvas(_myScene->getCanvasRoot()->childNode("canvas"));
        } else {
            //AC_WARNING << "Scene has multiple canvases. No canvas set automatically.";
        }

        for (ExtensionList::iterator it = _myExtensions.begin(); it != _myExtensions.end(); ++it) {
            std::string myName = (*it)->getName() + "::onSceneLoaded";

            try {
                MAKE_NAMED_SCOPE_TIMER(myTimer, myName);
                (*it)->onSceneLoaded(this);
            } catch (const asl::Exception & ex) {
                AC_ERROR << "Exception while calling " << myName << ": " << ex;
            } catch (...) {
                AC_ERROR << "Unknown exception while calling " << myName;
            }
        }
    } catch (const asl::Exception & ex) {
        AC_ERROR << "Exception caught in AbstractRenderWindow::setScene(): " << ex;
        return false;
    } catch (const exception & ex) {
        AC_ERROR << "Exception caught in AbstractRenderWindow::setScene(): " << ex.what();
        return false;
    } catch (...) {
        AC_ERROR << "Unknown exception in AbstractRenderWindow::setScene()";
        return false;
    }
    return true;
}

bool
AbstractRenderWindow::go() {
    // If the renderer has not been created by setScene(), yet, because everything
    // was added using the initial default scene, do so now.
    if (!_myRenderer) {
        y60::ScopedGLContext myContextLock(this);
        initDisplay();

        _myRenderer = RendererPtr(new Renderer(_myGLContext, _myRenderingCaps));
        _myRenderer->getTextManager().setTTFRenderer(createTTFRenderer());
        _myRenderer->setCurrentScene(getCurrentScene());

        setCanvas(_myScene->getCanvasRoot()->childNode("canvas"));
    }
    return true;
}

ViewportPtr
AbstractRenderWindow::getSingleViewport() const {
    if (_myCanvas->childNodesLength(VIEWPORT_NODE_NAME) != 1) {
        throw RenderWindowException("single-viewport convenience call used when multiple viewports exist!",
            PLUS_FILE_LINE);
    }
    return _myCanvas->childNode(VIEWPORT_NODE_NAME)->getFacade<Viewport>();
}

void
AbstractRenderWindow::unsetCanvas() {
    if (_myCanvas) {
        CanvasPtr myCanvas = _myCanvas->getFacade<Canvas>();
        if (!myCanvas->setFrameBuffer(asl::Ptr<IFrameBuffer>())) {
            // Huh?! This point should never be reached
            throw asl::Exception("Canvas::setFrameBuffer(0) did not return true. This should never happen.", PLUS_FILE_LINE);
        }
        _myCanvas = dom::NodePtr();
    }
}

dom::NodePtr
AbstractRenderWindow::getCanvas() {
    //ensureScene();
    if (!_myCanvas) {
        AC_WARNING << "No canvas found in scene";
    }
    return _myCanvas;
}

dom::NodePtr
AbstractRenderWindow::getCanvas() const {
    if (!_myCanvas) {
        AC_WARNING << "No canvas found in scene";
    }
    return _myCanvas;
}

bool
AbstractRenderWindow::setCanvas(const dom::NodePtr & theCanvas) {
    if (theCanvas) {
        unsetCanvas();
        CanvasPtr myCanvas = theCanvas->getFacade<Canvas>();
        AC_DEBUG << "AbstractRenderWindow::setCanvas setting canvas to " << myCanvas->get<IdTag>();
        if (myCanvas->setFrameBuffer(_mySelf.lock())) {
            _myCanvas = theCanvas;
            return true;
        } else {
            AC_WARNING << "Framebuffer is already set. Ignoring setCanvas";
            return false;
        }
    } else {
        throw asl::Exception("AbstractRenderWindow::setCanvas(): Canvas node is invalid.", PLUS_FILE_LINE);
    }
}

void
AbstractRenderWindow::saveBuffer(const std::string & theFilename) {

    // see GLBufferAdapter.h for possible format consts
    unsigned myFormat = PL_FT_PNG;
    string myExt = toUpperCase(theFilename.substr(theFilename.rfind('.')+1));
    if (myExt == "JPG") {
        myFormat = PL_FT_JPEG;
    } else if (myExt == "BMP") {
        myFormat = PL_FT_WINBMP;
    } else if (myExt == "TIFF" || myExt == "TIF") {
        myFormat = PL_FT_TIFF;
    } else {
        if (myExt != "PNG") {
            AC_WARNING << "AbstractRenderWindow::saveBuffer: Unknown extension " << myExt << ". Saving as png.";
        }
        myFormat = PL_FT_PNG;
    }

    y60::ScopedGLContext myContextLock(this);
    BufferToFile myBufferWriter(theFilename, myFormat,
        _myCanvas->getFacade<Canvas>()->getWidth(),
        _myCanvas->getFacade<Canvas>()->getHeight(), 4);
    myBufferWriter.performAction(FRAME_BUFFER);
}

void
AbstractRenderWindow::copyBufferToTexture(dom::NodePtr & theTextureNode,
                                          const asl::Vector2i & theOffset,
                                          bool theCopyToRasterFlag) {

                                              TexturePtr myTexture = theTextureNode->getFacade<y60::Texture>();
                                              BufferToTexture myBufferWriter(myTexture, theOffset, theCopyToRasterFlag);
                                              myBufferWriter.performAction(FRAME_BUFFER);
}

void
AbstractRenderWindow::setJSContext(JSContext * cx) {
    _myJSContext = cx;
    JS_SetErrorReporter(cx, _myErrorReporter);
}

JSObject *
AbstractRenderWindow::getEventListener() const {
    return _myEventListener;
}

void
AbstractRenderWindow::setEventListener(JSObject * theListener) {
    _myEventListener = theListener;
    //        if (_myJSContext && _myEventListener) {
    //            dumpJSObj(_myJSContext, _myEventListener);
    //        }
}

void
AbstractRenderWindow::setPause(bool thePauseFlag) {
    asl::NanoTime myTime = asl::NanoTime();
    static double _myPauseStart;
    if (thePauseFlag) {
        _myPauseStart = myTime.seconds();
    } else {
        _myPauseTime += (myTime.seconds() - _myPauseStart);
    }
    _myPauseFlag = thePauseFlag;
}

bool
AbstractRenderWindow::getPause() const {
    return _myPauseFlag;
}

void
AbstractRenderWindow::setForceFullGC(bool theForceFullGC) {
    _myForceFullGC = theForceFullGC;
}

bool
AbstractRenderWindow::getForceFullGC() const {
    return _myForceFullGC;
}

long
AbstractRenderWindow::setTimeout(const std::string & myCommand, float myMilliseconds, JSObject * theObjectToCall) {
    return _myTimeoutQueue.addTimeout(myCommand, _myElapsedTime, myMilliseconds, false, theObjectToCall);
}
void
AbstractRenderWindow::clearTimeout(long myTimeoutId) {
    _myTimeoutQueue.clearTimeout(myTimeoutId);
}
long
AbstractRenderWindow::setInterval(const std::string & myCommand, float myMilliseconds, JSObject * theObjectToCall) {
    return _myTimeoutQueue.addTimeout(myCommand, _myElapsedTime, myMilliseconds, true, theObjectToCall);
}
void
AbstractRenderWindow::clearInterval(long myIntervalId) {
    _myTimeoutQueue.clearTimeout(myIntervalId);
}

bool
AbstractRenderWindow::hasCap(unsigned int theCap) {
    y60::ScopedGLContext myContextLock(this);
    return y60::hasCap(theCap);
}

bool
AbstractRenderWindow::hasCapAsString(const std::string & theCapStr) {
    y60::ScopedGLContext myContextLock(this);
    return y60::hasCap(theCapStr);
}

std::string
AbstractRenderWindow::getGLVersionString() {
    y60::ScopedGLContext myContextLock(this);
    return y60::getGLVersionString();
}

std::string
AbstractRenderWindow::getGLVendorString() {
    y60::ScopedGLContext myContextLock(this);
    return y60::getGLVendorString();
}
std::string
AbstractRenderWindow::getGLRendererString() {
    y60::ScopedGLContext myContextLock(this);
    return y60::getGLRendererString();
}

unsigned int
AbstractRenderWindow::getGLExtensionStrings(std::vector<std::string> & theTokens) {
    y60::ScopedGLContext myContextLock(this);
    return y60::getGLExtensionStrings(theTokens);
}

void
AbstractRenderWindow::setRenderingCaps(unsigned int theRenderingCaps) {
    if (_myRenderer) {
        AC_WARNING << "Sorry, setting the rendering caps will only take effect before renderer is created!";
    } else {
        _myRenderingCaps = theRenderingCaps;
    }
}
unsigned int
AbstractRenderWindow::getRenderingCaps() {
    return _myRenderingCaps;
}

void
AbstractRenderWindow::onFrame() {
    asl::NanoTime myTime = asl::NanoTime();
    if (_myStartTime < 0.0) {
        _myStartTime = myTime.seconds();
        //AC_DEBUG << "startTime=" << _myStartTime;
    }

    MAKE_SCOPE_TIMER(onFrame);
    asl::StdOutputRedirector::get().checkForFileWrapAround();

    if (_myPauseFlag) {
        return;
    }

    // delta-T
    double myDeltaT;
    if (_myFixedDeltaT != 0.0) {
        myDeltaT = _myFixedDeltaT;
    } else {
        double myCurrentTime = myTime.seconds() - _myStartTime - _myPauseTime;
        myDeltaT = myCurrentTime - _myElapsedTime;
        if (myDeltaT < 0.0) {
            myDeltaT = 0.0;
        }
    }

    //AC_DEBUG << "pauseT=" << _myPauseTime << " elapsedT=" << _myElapsedTime << " deltaT=" << myDeltaT;
    // Check for timeouts
    {
        MAKE_SCOPE_TIMER(onFrame_updateTimeouts);
        while (_myTimeoutQueue.isShowTime(_myElapsedTime)) {
            TimeoutPtr myTimeout = _myTimeoutQueue.popTimeout();

            const std::string & myTimeoutCommand = myTimeout->getCommand();
            try {
                MAKE_SCOPE_TIMER(onPostViewport);
                JSObject * myListener = _myEventListener;
                if (myTimeout->getObjectToCall()) {
                    myListener = myTimeout->getObjectToCall();
                }
                if (JSA_hasFunction(_myJSContext, myListener, myTimeoutCommand.c_str()))
                {
                    jsval rval;
                    jslib::JSA_CallFunctionName(_myJSContext, myListener,
                        myTimeoutCommand.c_str(), 0, 0, &rval);
                } else {
                    AC_ERROR << "Timeout: Function '" << myTimeoutCommand << "' does not exist.";
                }
            } catch (const asl::Exception & ex) {
                AC_ERROR << "asl exception caught in AbstractRenderWindow::onFrame(),"
                    << " (timeout " << myTimeoutCommand << "): " << ex;
            } catch (const std::exception & ex) {
                AC_ERROR << "std::exception caught in AbstractRenderWindow::onFrame(),"
                    << " (timeout " << myTimeoutCommand << "): " << ex.what();
            } catch (...) {
                AC_ERROR << "Unknown exception in AbstractRenderWindow::onFrame(),"
                    << " (timeout " << myTimeoutCommand << ") ";
            }
        }
    }

    // update movies and capture
    if (_myScene) {
        MAKE_SCOPE_TIMER(onFrame_updateMovieAndCapture);
        std::vector<MoviePtr> myMovies = _myScene->getImagesRoot()->getAllFacades<Movie>(MOVIE_NODE_NAME);        
        for (unsigned i = 0; i < myMovies.size(); ++i) {
            _myScene->getTextureManager()->loadMovieFrame(myMovies[i], _myElapsedTime);
        }
        std::vector<CapturePtr> myCaptures = _myScene->getImagesRoot()->getAllFacades<Capture>(CAPTURE_NODE_NAME);        
        for (unsigned i = 0; i < myCaptures.size(); ++i) {
            _myScene->getTextureManager()->loadCaptureFrame(myCaptures[i]);
        }
    }

    // JavaScript onFrame(theTime, theDeltaT)
    if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onFrame")) {
        MAKE_SCOPE_TIMER(onFrame_JSCallback);
        jsval argv[2], rval;
        argv[0] = as_jsval(_myJSContext, _myElapsedTime);
        argv[1] = as_jsval(_myJSContext, myDeltaT);
        JSA_CallFunctionName(_myJSContext, _myEventListener, "onFrame", 2, argv, &rval);
    }

    // extensions onFrame
    for (ExtensionList::iterator i = _myExtensions.begin(); i != _myExtensions.end(); ++i) {
        const std::string myName = (*i)->getName() + "::onFrame";
        try {
            MAKE_NAMED_SCOPE_TIMER(myTimer, myName);
            (*i)->onFrame(this, _myElapsedTime);
        } catch (const asl::Exception & ex) {
            AC_ERROR << "EXCEPTION while calling " << myName << ": " << ex;
        } catch (const std::exception & ex) {
            AC_ERROR << "std::exception while calling" << myName << ": " << ex.what();
        } catch (...) {
            AC_ERROR << "UNKNOWN EXCEPTION while calling " << myName;
        }
    }

    // advance time
    if (_myFixedDeltaT != 0.0) {
        _myElapsedTime += _myFixedDeltaT;
    } else {
        _myElapsedTime = myTime.seconds() - _myStartTime - _myPauseTime;
        // dk + uz: sometimes myTime.seconds() - _myStartTime *is* negative
        // on the first frame.
        if (_myElapsedTime < 0.0) {
            _myElapsedTime = 0.0;
        }
    }
}

void
AbstractRenderWindow::clearBuffers(unsigned int theBuffersMask) {
    _myRenderer->clearBuffers(_myCanvas->getFacade<Canvas>(), theBuffersMask);
}

void
AbstractRenderWindow::preRender() {
    try {
        MAKE_SCOPE_TIMER(preRender);
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onPreRender")) {
            jsval argv[1], rval;
            MAKE_SCOPE_TIMER(onPreRender_JSCallback);
            jslib::JSA_CallFunctionName(_myJSContext, _myEventListener, "onPreRender", 0, argv, &rval);
        }

        for (ExtensionList::iterator i = _myExtensions.begin(); i != _myExtensions.end(); ++i) {
            const std::string myName = (*i)->getName() + "::onPreRender";
            try {
                MAKE_NAMED_SCOPE_TIMER(myTimer, myName);
                (*i)->onPreRender(this);
            } catch (const asl::Exception & ex) {
                AC_ERROR << "Exception while calling " << myName << ": " << ex;
            } catch (...) {
                AC_ERROR << "Unknown exception while calling " << myName;
            }
        }
    } catch (const asl::Exception & ex) {
        AC_ERROR << "ASL exception caught in AbstractRenderWindow::preRender(): " << ex;
    } catch (const std::exception & ex) {
        AC_ERROR << "std::exception caught in AbstractRenderWindow::preRender(): " << ex.what();
    } catch (...) {
        AC_ERROR << "Unknown exception in AbstractRenderWindow::preRender()";
    }
}

void
AbstractRenderWindow::preViewport(const dom::NodePtr & theViewport) {
    try {
        MAKE_SCOPE_TIMER(preViewport);
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onPreViewport")) {
            jsval argv[1], rval;
            argv[0] = as_jsval(_myJSContext, theViewport);
            jslib::JSA_CallFunctionName(_myJSContext, _myEventListener, "onPreViewport", 1, argv, &rval);
        }
    } catch (const asl::Exception & ex) {
        AC_ERROR << "ASL exception caught in AbstractRenderWindow::preViewport(): " << ex;
    } catch (const std::exception & ex) {
        AC_ERROR << "std::exception caught in AbstractRenderWindow::preViewport(): " << ex.what();
    } catch (...) {
        AC_ERROR << "Unknown exception in AbstractRenderWindow::preViewport()";
    }
}

void
AbstractRenderWindow::render() {
    MAKE_SCOPE_TIMER(render);
    for (unsigned i = 0; i < _myCanvas->childNodesLength(VIEWPORT_NODE_NAME); ++i) {
        const dom::NodePtr & myViewport = _myCanvas->childNode(VIEWPORT_NODE_NAME, i);
        preViewport(myViewport);
        _myRenderer->render(myViewport->getFacade<Viewport>());
        postViewport(myViewport);
    }
}

void
AbstractRenderWindow::postViewport(const dom::NodePtr & theViewport) {
    try {
        MAKE_SCOPE_TIMER(postViewport);
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onPostViewport")) {
            jsval argv[1], rval;
            argv[0] = as_jsval(_myJSContext, theViewport);
            jslib::JSA_CallFunctionName(_myJSContext, _myEventListener, "onPostViewport", 1, argv, &rval);
        }
    } catch (const asl::Exception & ex) {
        AC_ERROR << "ASL exception caught in AbstractRenderWindow::postViewport(): " << ex;
    } catch (const std::exception & ex) {
        AC_ERROR << "std::exception caught in AbstractRenderWindow::postViewport(): " << ex.what();
    } catch (...) {
        AC_ERROR << "Unknown exception in AbstractRenderWindow::postViewport()";
    }
}

void
AbstractRenderWindow::postRender() {
    try {
        MAKE_SCOPE_TIMER(postRender);
        // UH: changed calling order so that objects drawn from JS are
        // available for post-render components
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onPostRender")) {
            MAKE_SCOPE_TIMER(onPostRender);
            jsval argv[1], rval;
            JSA_CallFunctionName(_myJSContext, _myEventListener, "onPostRender", 0, argv, &rval);
        }

        for (ExtensionList::iterator i = _myExtensions.begin(); i != _myExtensions.end(); ++i) {
            const std::string myName = (*i)->getName() + "::onPostRender";
            try {
                MAKE_NAMED_SCOPE_TIMER(myTimer, myName);
                (*i)->onPostRender(this);
            } catch (const asl::Exception & ex) {
                AC_ERROR << "Exception while calling " << myName << ": " << ex;
            } catch (...) {
                AC_ERROR << "Unknown exception while calling " << myName;
            }
        }
    } catch (const asl::Exception & ex) {
        AC_ERROR << "ASL exception caught in AbstractRenderWindow::postRender(): " << ex;
    } catch (const exception & ex) {
        AC_ERROR << "std::exception caught in AbstractRenderWindow::postRender(): " << ex.what();
    } catch (...) {
        AC_ERROR << "Unknown exception in AbstractRenderWindow::postRender()";
    }
}

void
AbstractRenderWindow::renderFrame() {

    _myRenderer->getCurrentScene()->updateAllModified();

    if (jslib::JSA_hasFunction(_myJSContext, _myEventListener, "onRender")) {
        jsval argv[1], rval;
        argv[0] = jslib::as_jsval(_myJSContext, _myElapsedTime);
        jslib::JSA_CallFunctionName(_myJSContext, _myEventListener, "onRender", 1, argv, &rval);
    } else {
        clearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        preRender();
        render();
        postRender();
    }
}

void
AbstractRenderWindow::handle(y60::EventPtr theEvent) {
    MAKE_SCOPE_TIMER(handleEvents);
    theEvent->simulation_time = _myElapsedTime;
    for (ExtensionList::iterator i = _myExtensions.begin(); i != _myExtensions.end(); ++i) {
        string myName = (*i)->getName() + "::handle";
        try {
            MAKE_NAMED_SCOPE_TIMER(myTimer, myName);
            (*i)->handle(this, theEvent);
        } catch (const asl::Exception & ex) {
            AC_ERROR << "EXCEPTION while calling " << myName << ": " << ex;
        } catch (...) {
            AC_ERROR << "UNKNOWN EXCEPTION while calling " << myName;
        }
    }

    switch (theEvent->type) {
            case y60::Event::MOUSE_MOTION:
                onMouseMotion(*theEvent);
                break;
            case y60::Event::MOUSE_BUTTON_DOWN:
            case y60::Event::MOUSE_BUTTON_UP:
                onMouseButton(*theEvent);
                break;
            case y60::Event::MOUSE_WHEEL:
                onMouseWheel(*theEvent);
                break;
            case y60::Event::KEY_UP:
            case y60::Event::KEY_DOWN:
                onKey(*theEvent);
                break;
            case y60::Event::BUTTON_UP:
            case y60::Event::BUTTON_DOWN:
                onButton(*theEvent);
                break;
            case y60::Event::AXIS:
                onAxis(*theEvent);
                break;
            case y60::Event::TOUCH:
                onTouch(*theEvent);
                break;
            case y60::Event::RESIZE:
                onResize(*theEvent);
                break;
            case y60::Event::GENERIC:
                onGenericEvent(*theEvent);
                break;
            default:
                AC_WARNING << "Unknown event type: " << asl::as_string(theEvent->type) << endl;
    }
}

void
AbstractRenderWindow::onKey(y60::Event & theEvent) {
    if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onKey")) {
        y60::KeyEvent & myKeyEvent = dynamic_cast<y60::KeyEvent&>(theEvent);

        try {
            jsval argv[9], rval;
            argv[0] = as_jsval(_myJSContext, myKeyEvent.keyString);
            argv[1] = as_jsval(_myJSContext, myKeyEvent.type == y60::Event::KEY_DOWN);
            argv[2] = as_jsval(_myJSContext, 0); // TODO: mouse X POS
            argv[3] = as_jsval(_myJSContext, 0); // TODO: mouse Y POS
            argv[4] = as_jsval(_myJSContext, ((myKeyEvent.modifiers & y60::KEYMOD_SHIFT) !=0));
            argv[5] = as_jsval(_myJSContext, ((myKeyEvent.modifiers & y60::KEYMOD_CTRL) !=0));
            argv[6] = as_jsval(_myJSContext, ((myKeyEvent.modifiers & y60::KEYMOD_ALT) !=0));

            argv[7] = as_jsval(_myJSContext, theEvent.simulation_time);
            argv[8] = as_jsval(_myJSContext, myKeyEvent.scanCode);

            JSA_CallFunctionName(_myJSContext, _myEventListener, "onKey", 9, argv, &rval);
        } catch (const asl::Exception & ex) {
            AC_ERROR << "ASL exception caught in AbstractRenderWindow::onKey(): " << ex;
        } catch (const exception & ex) {
            AC_ERROR << "std::exception caught in AbstractRenderWindow::onKey(): " << ex.what();
        } catch (...) {
            AC_ERROR << "Unknown exception in AbstractRenderWindow::onKey()";
        }
    }
}

void
AbstractRenderWindow::onMouseButton(y60::Event & theEvent) {
    if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onMouseButton")) {
        y60::MouseEvent & myMouseEvent = dynamic_cast<y60::MouseEvent&>(theEvent);
        jsval argv[5], rval;
        argv[0] = as_jsval(_myJSContext, myMouseEvent.button);
        argv[1] = as_jsval(_myJSContext, myMouseEvent.type == y60::Event::MOUSE_BUTTON_DOWN);
        if (getOrientation() == PORTRAIT_ORIENTATION) {
            argv[2] = as_jsval(_myJSContext, myMouseEvent.position[1]);
            argv[3] = as_jsval(_myJSContext, getWidth() - myMouseEvent.position[0]);
        } else {
            argv[2] = as_jsval(_myJSContext, myMouseEvent.position[0]);
            argv[3] = as_jsval(_myJSContext, myMouseEvent.position[1]);
        }

        argv[4] = as_jsval(_myJSContext, theEvent.simulation_time);

        JSA_CallFunctionName(_myJSContext, _myEventListener, "onMouseButton", 5, argv, &rval);
    }
}

void
AbstractRenderWindow::onMouseMotion(y60::Event & theEvent) {
    if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onMouseMotion")) {
        y60::MouseEvent & myMouseEvent = dynamic_cast<y60::MouseEvent&>(theEvent);
        jsval argv[3], rval;
        if (getOrientation() == PORTRAIT_ORIENTATION) {
            argv[0] = as_jsval(_myJSContext, myMouseEvent.position[1]);
            argv[1] = as_jsval(_myJSContext, getWidth() - myMouseEvent.position[0]);
        } else {
            argv[0] = as_jsval(_myJSContext, myMouseEvent.position[0]);
            argv[1] = as_jsval(_myJSContext, myMouseEvent.position[1]);
        }

        argv[2] = as_jsval(_myJSContext, theEvent.simulation_time);

        JSA_CallFunctionName(_myJSContext, _myEventListener, "onMouseMotion", 3, argv, &rval);
    }
}

void
AbstractRenderWindow::onMouseWheel(y60::Event & theEvent) {
    if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onMouseWheel")) {
        y60::MouseEvent & myMouseEvent = dynamic_cast<y60::MouseEvent&>(theEvent);
        jsval argv[3], rval;

        argv[0] = as_jsval(_myJSContext, myMouseEvent.delta[0]);
        argv[1] = as_jsval(_myJSContext, myMouseEvent.delta[1]);

        argv[2] = as_jsval(_myJSContext, theEvent.simulation_time);

        JSA_CallFunctionName(_myJSContext, _myEventListener, "onMouseWheel", 3, argv, &rval);
    }
}

void
AbstractRenderWindow::onAxis(y60::Event & theEvent) {
    if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onAxis")) {
        y60::AxisEvent & myAxisEvent = dynamic_cast<y60::AxisEvent&>(theEvent);
        jsval argv[4], rval;
        argv[0] = as_jsval(_myJSContext, myAxisEvent.device);
        argv[1] = as_jsval(_myJSContext, myAxisEvent.axis);
        argv[2] = as_jsval(_myJSContext, myAxisEvent.value);

        argv[3] = as_jsval(_myJSContext, theEvent.simulation_time);

        JSA_CallFunctionName(_myJSContext, _myEventListener, "onAxis", 4, argv, &rval);
    }
}

void
AbstractRenderWindow::onTouch(y60::Event & theEvent) {
    if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onTouch")) {
        y60::TouchEvent & myEvent = dynamic_cast<y60::TouchEvent&>(theEvent);

        jsval argv[7], rval;
        int nargs = 0;
        argv[nargs++] = as_jsval(_myJSContext, myEvent.device);
        argv[nargs++] = as_jsval(_myJSContext, myEvent.position[0]);
        argv[nargs++] = as_jsval(_myJSContext, myEvent.position[1]);
        argv[nargs++] = as_jsval(_myJSContext, myEvent.size[0]);
        argv[nargs++] = as_jsval(_myJSContext, myEvent.size[1]);
        argv[nargs++] = as_jsval(_myJSContext, myEvent.intensity);

        argv[nargs++] = as_jsval(_myJSContext, theEvent.simulation_time);

        JSA_CallFunctionName(_myJSContext, _myEventListener, "onTouch", nargs, argv, &rval);
    }
}

void
AbstractRenderWindow::onResize(y60::Event & theEvent) {
    if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onResize")) {
        y60::WindowEvent & myWindowEvent = dynamic_cast<y60::WindowEvent&>(theEvent);
        jsval argv[3], rval;
        argv[0] = as_jsval(_myJSContext, myWindowEvent.width);
        argv[1] = as_jsval(_myJSContext, myWindowEvent.height);

        argv[2] = as_jsval(_myJSContext, theEvent.simulation_time);

        JSA_CallFunctionName(_myJSContext, _myEventListener, "onResize", 3, argv, &rval);
    }
}

void
AbstractRenderWindow::onButton(y60::Event & theEvent) {
    if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onButton")) {
        y60::ButtonEvent & myButtonEvent = dynamic_cast<y60::ButtonEvent&>(theEvent);
        jsval argv[4], rval;
        argv[0] = as_jsval(_myJSContext, myButtonEvent.device);
        argv[1] = as_jsval(_myJSContext, myButtonEvent.button);
        if (myButtonEvent.type == y60::Event::BUTTON_UP) {
            argv[2] = as_jsval(_myJSContext, 0);
        } else {
            argv[2] = as_jsval(_myJSContext, 1);
        }

        argv[3] = as_jsval(_myJSContext, theEvent.simulation_time);

        JSA_CallFunctionName(_myJSContext, _myEventListener, "onButton", 4, argv, &rval);
    }
}

void
AbstractRenderWindow::onGenericEvent(y60::Event & theEvent) {
    y60::GenericEvent & myEvent = dynamic_cast<y60::GenericEvent&>( theEvent );
    dom::NodePtr myEventNode = myEvent.asNode();
    std::string myCallback( myEventNode->getAttribute("callback") ?
        myEventNode->getAttributeValue<std::string>("callback") : "onEvent");
    if (_myEventListener ) {
        if (JSA_hasFunction(_myJSContext, _myEventListener, myCallback.c_str())) {
            jsval arg, rval;
            arg = as_jsval( _myJSContext, myEvent.asNode());
            JSA_CallFunctionName(_myJSContext, _myEventListener, myCallback.c_str(), 1, & arg, & rval);
        } else {
            AC_WARNING << "Generic event callback '" << myCallback << "' is missing.";/* << " in class "
                                                                                      << JSA_GetClass(_myJSContext, _myEventListener)->name;*/
        }
    }
}

const RendererPtr
AbstractRenderWindow::getRenderer() const {
    return _myRenderer;
}

RendererPtr
AbstractRenderWindow::getRenderer() {
    return _myRenderer;
}

void
AbstractRenderWindow::addExtension(IRendererExtensionPtr theExtension) {
    const std::string & myName = theExtension->getName();
    for (ExtensionList::iterator i = _myExtensions.begin(); i != _myExtensions.end(); ++i) {
        if ( myName == (*i)->getName()) {
            AC_WARNING << "Extension name '" << myName
                << "' is ambiguous. Statistical timers will be misleading." << endl;
        }
    }
    _myExtensions.push_back(theExtension);

    AC_INFO << "Added extension '" << theExtension->getName() << "'";
    theExtension->onStartup(this);
    if (getCurrentScene()) {
        theExtension->onSceneLoaded(this);
    }
}

void
AbstractRenderWindow::printStatistics() {
    asl::getDashboard().print(std::cerr);
    asl::getDashboard().reset();
}

double
AbstractRenderWindow::getFrameRate() const {
    return asl::getDashboard().getFrameRate();
}

// =======================================================================
//  Scene methods
// =======================================================================
ImagePtr
AbstractRenderWindow::getImage(const std::string & theFileName) {
    if (_myScene) {
        /* XXX
        ImagePtr myImage = _myScene->getTextureManager()->getImage(theFileName);
        if (myImage) {
        return myImage;
        }
        */
    }
    return ImagePtr();
}

// TODO: adapt for other 1 and 3 byte pixel formats
asl::Vector4i
AbstractRenderWindow::getImagePixel(dom::NodePtr theImageNode, unsigned long theX, unsigned long theY) {
    try {
        ImagePtr myImage = theImageNode->getFacade<y60::Image>();
        if (myImage) {
            const unsigned char * myData = myImage->getRasterPtr()->pixels().begin();
            if (myData) {
                const unsigned char * myPixelPtr = myData +
                    getBytesRequired(theY * myImage->get<ImageWidthTag>() + theX, myImage->getRasterEncoding());
                asl::Vector4i myColor(*myPixelPtr, *(myPixelPtr + 1), *(myPixelPtr + 2), *(myPixelPtr + 3));
                return myColor;
            }
        }
    } catch(const exception & ex) {
        AC_ERROR << "AbstractRenderWindow::getImagePixel: Exception caught: " << ex.what();
    } catch(const asl::Exception & ex) {
        AC_ERROR << "AbstractRenderWindow::getImagePixel: Exception caught: " << ex;
    }
    return asl::Vector4i(0,0,0,0);
}

bool
AbstractRenderWindow::setImagePixel(dom::NodePtr theImageNode, unsigned long theX, unsigned long theY, const asl::Vector4i & theColor) {
    try {
        ImagePtr myImage = theImageNode->getFacade<y60::Image>();
        if (myImage) {
            unsigned char * myData = myImage->getRasterPtr()->pixels().begin();
            if (myData) {
                unsigned char * myPixelPtr = myData +
                    getBytesRequired(theY * myImage->get<ImageWidthTag>() + theX, myImage->getRasterEncoding());

                unsigned myUsedBytesPerPixel = getBytesRequired(1, myImage->getRasterEncoding());
                for (unsigned i = 0; i < myUsedBytesPerPixel; ++i) {
                    *(myPixelPtr + i) = static_cast<unsigned char>(theColor[i]);
                }
                return true;
            }
        }
    } catch(const exception & ex) {
        AC_ERROR << "AbstractRenderWindow::setImagePixel: Exception caught: " << ex.what();
    } catch(const asl::Exception & ex) {
        AC_ERROR << "AbstractRenderWindow::setImagePixel: Exception caught: " << ex;
    }
    return false;
}

void
AbstractRenderWindow::performRequest(const jslib::JSRequestPtr & theRequest) {
    _myRequestManager.performRequest(theRequest);
}

// =======================================================================
//  Text Manager
// =======================================================================
void AbstractRenderWindow::renderText(const asl::Vector2f & thePixelPosition, const std::string & theString,
                                      const std::string & theFont, const y60::ViewportPtr & theViewport)
{
    asl::Vector2f myRelativePosition(thePixelPosition[0] / (getWidth() * theViewport->get<Size2DTag>()[0]),
        thePixelPosition[1] / (getHeight() * theViewport->get<Size2DTag>()[1]));
    _myRenderer->getTextManager().addText(myRelativePosition, theString, theFont, theViewport);
}

void AbstractRenderWindow::setTextColor(const asl::Vector4f & theTextColor)
{
    _myRenderer->getTextManager().setColor(theTextColor);
}
const asl::Vector4f &
AbstractRenderWindow::getTextColor() {
    return _myRenderer->getTextManager().getColor();
}

const asl::Vector2i & AbstractRenderWindow::getTextCursorPosition() const
{
    return _myRenderer->getTextManager().getTextCursorPosition();
}

const std::vector<asl::Vector2i> & AbstractRenderWindow::getTextGlyphPositions() const
{
    return _myRenderer->getTextManager().getGlyphPositions();
}

const std::vector<unsigned int> & AbstractRenderWindow::getLineWidths() const
{
    return _myRenderer->getTextManager().getLineWidths();
}

const unsigned int & AbstractRenderWindow::getMaxWidth() const
{
    return _myRenderer->getTextManager().getMaxWidth();
}

asl::Vector2i AbstractRenderWindow::renderTextAsImage(dom::NodePtr theImageNode,
                                                      const std::string & theString, const std::string & theFont,
                                                      const unsigned int & theTargetWidth, const unsigned int & theTargetHeight,
                                                      const asl::Vector2i & theCursorPos)
{
    return _myRenderer->getTextManager().renderTextAsImage(*(_myScene->getTextureManager()),
        theImageNode, theString, theFont, theTargetWidth, theTargetHeight, theCursorPos);
}

void AbstractRenderWindow::setTextPadding(int topPadding, int bottomPadding, int leftPadding, int rightPadding) {
    _myRenderer->getTextManager().setPadding(topPadding, bottomPadding, leftPadding, rightPadding);
}

void AbstractRenderWindow::setTextIndentation(int theIndentaion) {
    _myRenderer->getTextManager().setIndentation(theIndentaion);
}
void AbstractRenderWindow::setHTextAlignment(unsigned int theHAlignment) {
    _myRenderer->getTextManager().setHTextAlignment(TextRenderer::TextAligment(theHAlignment));
}

void AbstractRenderWindow::setVTextAlignment(unsigned int theVAlignment) {
    _myRenderer->getTextManager().setVTextAlignment(TextRenderer::TextAligment(theVAlignment));
}
void AbstractRenderWindow::setLineHeight(unsigned int theLineHeight) {
    _myRenderer->getTextManager().setLineHeight(theLineHeight);
}
void AbstractRenderWindow::setParagraph(unsigned int theTopMargin, unsigned int theBottomMargin) {
    _myRenderer->getTextManager().setParagraph(theTopMargin, theBottomMargin);
}

bool AbstractRenderWindow::getFontMetrics(const std::string & theFontName,
                                          int & theFontHeight,
                                          int & theFontAscent, int & theFontDescent,
                                          int & theFontLineSkip) {
                                              return _myRenderer->getTextManager().getFontMetrics(theFontName, theFontHeight, theFontAscent, theFontDescent, theFontLineSkip);
}

bool AbstractRenderWindow::getGlyphMetrics(const std::string & theFontName,
                                           const std::string & theCharacter,
                                           asl::Box2f & theGlyphBox, double & theAdvance) {
                                               return _myRenderer->getTextManager().getGlyphMetrics(theFontName, theCharacter, theGlyphBox, theAdvance);
}

double AbstractRenderWindow::getKerning(const std::string & theFontName,
                                        const std::string & theFirstCharacter,
                                        const std::string & theSecondCharacter) {
                                            return _myRenderer->getTextManager().getKerning(theFontName, theFirstCharacter, theSecondCharacter);
}
bool AbstractRenderWindow::hasGlyph(const std::string & theFontName,
                                    const std::string & theCharacter) {
                                        return _myRenderer->getTextManager().hasGlyph(theFontName, theCharacter);
}
void AbstractRenderWindow::setTracking(float theTracking) {
    _myRenderer->getTextManager().setTracking(theTracking);
}

// =======================================================================
//  Animation Manager
// =======================================================================
void AbstractRenderWindow::runAnimations(float theTime) {
    MAKE_SCOPE_TIMER(updateAnimations);
    if (_myScene) {
        _myScene->getAnimationManager().run(theTime);
    }
}
void AbstractRenderWindow::playClip(float theTime,
                                    const std::string & theCharacterName,
                                    const std::string & theClipName)
{
    if (_myScene) {
        _myScene->getAnimationManager().playClip(theTime, theCharacterName, theClipName);
    }
}
void AbstractRenderWindow::setClipLoops(const std::string & theCharacterName,
                                        const std::string & theClipName,
                                        unsigned int theLoops)
{
    if (_myScene) {
        _myScene->getAnimationManager().setClipLoops(theCharacterName, theClipName, theLoops);
    }
}
void AbstractRenderWindow::setClipForwardDirection(const std::string & theCharacterName,
                                                   const std::string & theClipName,
                                                   bool theDirection)
{
    if (_myScene) {
        _myScene->getAnimationManager().setClipForwardDirection(theCharacterName, theClipName, theDirection);
    }
}

unsigned int AbstractRenderWindow::getLoops(const std::string & theCharacterName, const std::string & theClipName) {
    if (_myScene) {
        return _myScene->getAnimationManager().getLoops(theCharacterName, theClipName);
    } else {
        return 0;
    }
}

bool AbstractRenderWindow::isClipActive(const std::string & theCharacterName, const std::string & theClipName) {
    if (_myScene) {
        return _myScene->getAnimationManager().isClipActive(theCharacterName, theClipName);
    }
    return false;
}
bool AbstractRenderWindow::isCharacterActive(const std::string & theCharacterName) {
    if (_myScene) {
        return _myScene->getAnimationManager().isCharacterActive(theCharacterName);
    }
    return false;
}

void AbstractRenderWindow::stopCharacter(const std::string & theCharacterName) {
    if (_myScene) {
        _myScene->getAnimationManager().stop(theCharacterName);
    }
}

void
AbstractRenderWindow::setFixedDeltaT(const double & theDeltaT) {
    _myFixedDeltaT = theDeltaT;
}
const double &
AbstractRenderWindow::getFixedDeltaT() const {
    return _myFixedDeltaT;
}
void
AbstractRenderWindow::getPixel(unsigned int theXPos, unsigned int theYPos, GLenum theFormat, float & theValue) {
    y60::ScopedGLContext myContextLock(this);
    glReadPixels(theXPos, theYPos, 1,1, theFormat, GL_FLOAT, &theValue);
}
void
AbstractRenderWindow::getPixel(unsigned int theXPos, unsigned int theYPos, GLenum theFormat, asl::Unsigned8 & theValue) {
    y60::ScopedGLContext myContextLock(this);
    glReadPixels(theXPos, theYPos, 1,1, theFormat, GL_UNSIGNED_BYTE, &theValue);
}
}

