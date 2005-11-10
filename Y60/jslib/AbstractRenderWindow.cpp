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
//    $RCSfile: AbstractRenderWindow.cpp,v $
//     $Author: jens $
//   $Revision: 1.34 $
//       $Date: 2005/04/26 19:55:59 $
//
//=============================================================================

#include "AbstractRenderWindow.h"
#include "JSApp.h"
#include "JSNode.h"

#include <y60/WindowEvent.h>
#include <y60/KeyEvent.h>
#include <y60/MouseEvent.h>
#include <y60/AxisEvent.h>
#include <y60/ButtonEvent.h>
#include <y60/TouchEvent.h>
#include <y60/ScopedGLContext.h>
#include <y60/RenderStatistic.h>
#include <y60/Image.h>
#include <y60/Movie.h>
#include <y60/Capture.h>
#include <y60/GLBufferAdapter.h>

#include <asl/file_functions.h>
#include <asl/os_functions.h>
#include <asl/PlugInManager.h>
#include <asl/Ptr.h>
#include <asl/StdOutputRedirector.h>
#include <dom/Nodes.h>
#include "GLContextRegistry.h"

using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) // x

namespace jslib {

    AbstractRenderWindow::AbstractRenderWindow(const JSErrorReporter & theErrorReporter) :
        _myScene(0), _myRenderer(0),
        _myContext(0),
        _myEventListener(0),
        _myJSContext(0),
        _myRenderingCaps(~0), // enable all (~)
        _myPauseTime(0),
        _myPauseFlag(false),
        _myErrorReporter(theErrorReporter),
        _myFixedDeltaT(0.0),
        _myElapsedTime(0.0)
        {
        }

    void
    AbstractRenderWindow::setSelf(asl::Ptr<AbstractRenderWindow> theSelfPtr) {
        _mySelf = theSelfPtr;
        GLContextRegistry::get().registerContext(_mySelf);
    }

    AbstractRenderWindow :: ~AbstractRenderWindow() {
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
            _myScene = y60::ScenePtr(new y60::Scene());
            _myScene->createStubs(JSApp::getPackageManager());
            setCanvas(_myScene->getCanvasRoot()->childNode("canvas"));
        }
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

    // TODO: do something with the extensions
    bool
    AbstractRenderWindow::setScene(const y60::ScenePtr & theScene) {

        if (!_mySelf) {
            throw asl::Exception("ERROR: AbstractRenderWindow::_mySelf is 0! Did you use the proper factory method?", PLUS_FILE_LINE);
        }
        // Do I already have a scene, then cleanly unbind it from me
        if (_myScene) {
            unsetCanvas();
            AC_INFO << "setScene removed Framebuffer from previously rendered scene";
            _myScene = y60::ScenePtr(0);
        }
        //if (!theScene) {
        //    throw asl::Exception("ERROR: AbstractRenderWindow::theScene is 0!", PLUS_FILE_LINE);
        //}
        try {
            y60::ScopedGLContext myGLContext(this);
            initDisplay();

            _myRenderer = RendererPtr(new Renderer(_myRenderingCaps));
            _myRenderer->getTextManager().setTTFRenderer(createTTFRenderer());

            _myScene = theScene;
            _myRenderer->setCurrentScene(_myScene);

            setCanvas(_myScene->getCanvasRoot()->childNode("canvas"));

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

    void
    AbstractRenderWindow::go() {
        // If the renderer has not been created by setScene(), yet, because everything
        // was added using the initial default scene, do so now.
        if (!_myRenderer) {
            y60::ScopedGLContext myGLContext(this);
            initDisplay();

            _myRenderer = RendererPtr(new Renderer(_myRenderingCaps));
            _myRenderer->getTextManager().setTTFRenderer(createTTFRenderer());
            _myRenderer->setCurrentScene(getCurrentScene());

            setCanvas(_myScene->getCanvasRoot()->childNode("canvas"));
        }
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
            if (!myCanvas->setFrameBuffer(asl::Ptr<IFrameBuffer>(0))) {
                // Huh?! This point should never be reached
                throw asl::Exception("Canvas::setFrameBuffer(0) did not return true. This should never happen.", PLUS_FILE_LINE);
            }
            _myCanvas = dom::NodePtr(0);
        }
    }

    dom::NodePtr
    AbstractRenderWindow::getCanvas() {
        // ensureScene();
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
        BufferToFile myBufferWriter( _myCanvas->getFacade<Canvas>()->getWidth(),
                _myCanvas->getFacade<Canvas>()->getHeight(), 4);
        myBufferWriter.performAction(FRAME_BUFFER);
        // see GLBufferAdapter.h for possible format consts
        myBufferWriter.saveBufferAsImage(PL_FT_PNG, theFilename);
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
    AbstractRenderWindow::setPause(bool aPause) {
        asl::NanoTime myTime = asl::NanoTime();
        static double _myPauseStart;
        if (aPause) {
            _myPauseStart = myTime.seconds();
        } else {
            _myPauseTime += (myTime.seconds() - _myPauseStart);
        }
        _myPauseFlag = aPause;
    }

    bool
    AbstractRenderWindow::getPause() const {
        return _myPauseFlag;
    }

    long
    AbstractRenderWindow::setTimeout(const std::string & myCommand, float myMilliseconds) {
        return _myTimeoutQueue.addTimeout(myCommand, _myElapsedTime,
                myMilliseconds);
    }
    void
    AbstractRenderWindow::clearTimeout(long myTimeoutId) {
        _myTimeoutQueue.clearTimeout(myTimeoutId);
    }
    long
    AbstractRenderWindow::setInterval(const std::string & myCommand, float myMilliseconds) {
        return _myTimeoutQueue.addTimeout(myCommand, _myElapsedTime,
                myMilliseconds, true /* isInterval */);
    }
    void
    AbstractRenderWindow::clearInterval(long myIntervalId) {
        _myTimeoutQueue.clearTimeout(myIntervalId);
    }

    bool
    AbstractRenderWindow::hasCap(unsigned int theCap) {
        y60::ScopedGLContext myGLContext(this);
        return y60::hasCap(theCap);
    }

    bool
    AbstractRenderWindow::hasCapAsString(const std::string & theCapStr) {
        y60::ScopedGLContext myGLContext(this);
        return y60::hasCap(theCapStr);
    }

    void
    AbstractRenderWindow::setRenderingCaps(unsigned int theRenderingCaps) {
        _myRenderingCaps = theRenderingCaps;
        if (_myRenderer) {
            AC_WARNING << "Sorry, setting the rendering caps will only take effect before renderer is created!" << endl;
        }
    }
    unsigned int
    AbstractRenderWindow::getRenderingCaps() {
        return _myRenderingCaps;
    }

    void
    AbstractRenderWindow::onFrame() {
        asl::NanoTime myTime = asl::NanoTime();
        static double _myStartTime = myTime.seconds();

        MAKE_SCOPE_TIMER(onFrame);
        asl::StdOutputRedirector::get().checkForFileWrapAround();

        if (!_myPauseFlag) {
            if (_myFixedDeltaT == 0.0) {
                _myElapsedTime = myTime.seconds() - _myStartTime - _myPauseTime;
                // dk + uz: sometimes myTime.seconds() - _myStartTime *is* negative
                // on the first frame.
                if (_myElapsedTime < 0) {
                    _myElapsedTime = 0;
                }
            }
            DB(cerr << "st: " << _myStartTime << " pt " << _myPauseTime << " et " << _myElapsedTime << endl;)
            // Check for timeouts
            {
                MAKE_SCOPE_TIMER(Timeouts);
                while (_myTimeoutQueue.isShowTime(_myElapsedTime)) {
                    TimeoutPtr myTimeout = _myTimeoutQueue.popTimeout();

                    const std::string & myTimeoutCommand = myTimeout->getCommand();
                    try {
                        MAKE_SCOPE_TIMER(onPostViewport);
                        if (JSA_hasFunction(_myJSContext, _myEventListener,
//                        if (JSA_hasFunction(_myJSContext, myTimeout->getJSObject(),
                                    myTimeoutCommand.c_str()))
                        {
                            jsval rval;
                            jslib::JSA_CallFunctionName(_myJSContext, _myEventListener,
//                            jslib::JSA_CallFunctionName(_myJSContext, myTimeout->getJSObject(),
                                    myTimeoutCommand.c_str(), 0, 0, &rval);
                        } else {
                            AC_ERROR << "Timeout: Function " << myTimeoutCommand
                                    << " does not exist." << endl;
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

            if (_myScene) {
                dom::NodePtr myImages = _myScene->getImagesRoot();
                for (unsigned i = 0; i < myImages->childNodesLength(); ++i) {
                    dom::NodePtr myImage = myImages->childNode(i);
                    if (myImage->nodeType() != dom::Node::ELEMENT_NODE) {
                        continue;
                    }
                    if (myImage->nodeName() == MOVIE_NODE_NAME) {
                        _myScene->getTextureManager()->loadMovieFrame(myImage->getFacade<Movie>(), _myElapsedTime);
                    }
                    else if (myImage->nodeName() == CAPTURE_NODE_NAME) {
                        _myScene->getTextureManager()->loadCaptureFrame(myImage->getFacade<Capture>());
                    }
                }
            }

            if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onFrame")) {
                MAKE_SCOPE_TIMER(IdleJS);
                jsval argv[1], rval;
                argv[0] = as_jsval(_myJSContext, _myElapsedTime);
                JSA_CallFunctionName(_myJSContext, _myEventListener, "onFrame", 1, argv, &rval);
            }


            for (ExtensionList::iterator i = _myExtensions.begin(); i != _myExtensions.end(); ++i) {
                string myName = (*i)->getName() + "::onFrame";
                try {
                    MAKE_NAMED_SCOPE_TIMER(myTimer, myName);
                    (*i)->onFrame(this, _myElapsedTime);
                } catch (const asl::Exception & ex) {
                    AC_ERROR << "EXCEPTION while calling " << myName << ": " << ex;
                } catch (...) {
                    AC_ERROR << "UNKNOWN EXCEPTION while calling " << myName;
                }
            }
            if (_myFixedDeltaT != 0.0) {
                _myElapsedTime += _myFixedDeltaT;
            }
        }
    }

    void
    AbstractRenderWindow::preRender() {
        try {
            MAKE_SCOPE_TIMER(onPreRender);

            if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onPreRender")) {
                jsval argv[1], rval;
                MAKE_SCOPE_TIMER(onPreRender_JSCallback);
                jslib::JSA_CallFunctionName(_myJSContext, _myEventListener, "onPreRender", 0, argv, &rval);
            }

            for (ExtensionList::iterator i = _myExtensions.begin(); i != _myExtensions.end(); ++i) {
                string myName = (*i)->getName() + "::onPreRender";
                try {
                    MAKE_NAMED_SCOPE_TIMER(myTimer, myName);
                    (*i)->onPreRender(this);
                } catch (const asl::Exception & ex) {
                    AC_ERROR << "Exception while calling " << myName << ": " << ex;
                } catch (...) {
                    AC_ERROR << "Unknown exception while calling " << myName;
                }
            }
            _myRenderer->preRender(_myCanvas->getFacade<Canvas>());

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
            MAKE_SCOPE_TIMER(onPreViewport);
            if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onPreViewport")) {
                jsval argv[1], rval;
                argv[0] = as_jsval(_myJSContext, theViewport);
                jslib::JSA_CallFunctionName(_myJSContext, _myEventListener, "onPreViewport", 1, argv, &rval);
            }
        } catch (const asl::Exception & ex) {
            AC_ERROR << "ASL exception caught in Y60Render::preViewport(): " << ex;
        } catch (const std::exception & ex) {
            AC_ERROR << "std::exception caught in Y60Render::preViewport(): " << ex.what();
        } catch (...) {
            AC_ERROR << "Unknown exception in Y60Render::preViewport()";
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
            MAKE_SCOPE_TIMER(onPostViewport);

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
            _myRenderer->postRender();
            for (ExtensionList::iterator i = _myExtensions.begin(); i != _myExtensions.end(); ++i) {
                string myName = (*i)->getName() + "::onPostRender";
                try {
                    MAKE_NAMED_SCOPE_TIMER(myTimer, myName);
                    (*i)->onPostRender(this);
                } catch (const asl::Exception & ex) {
                    AC_ERROR << "Exception while calling " << myName << ": " << ex;
                } catch (...) {
                    AC_ERROR << "Unknown exception while calling " << myName;
                }
            }

            if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onPostRender")) {
                MAKE_SCOPE_TIMER(onPostRender);
                jsval argv[1], rval;
                JSA_CallFunctionName(_myJSContext, _myEventListener, "onPostRender", 0, argv, &rval);
            }

            if (_myJSContext) {
                MAKE_SCOPE_TIMER(gc);
                JS_GC(_myJSContext);
            }
        } catch (const asl::Exception & ex) {
            AC_ERROR << "ASL exception caught in Y60Render::postRender(): " << ex;
        } catch (const exception & ex) {
            AC_ERROR << "std::exception caught in Y60Render::postRender(): " << ex.what();
        } catch (...) {
            AC_ERROR << "Unknown exception in Y60Render::postRender()";
        }
    }

    void
    AbstractRenderWindow::handle(y60::EventPtr theEvent) {
        MAKE_SCOPE_TIMER(handleEvents);
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
            default:
                AC_WARNING << "Unknown event type: " << asl::as_string(theEvent->type) << endl;
        }
    }

    void
    AbstractRenderWindow::onKey(y60::Event & theEvent) {
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onKey")) {
            y60::KeyEvent & myKeyEvent = dynamic_cast<y60::KeyEvent&>(theEvent);

            try {
                jsval argv[7], rval;
                argv[0] = as_jsval(_myJSContext, myKeyEvent.keyString);
                argv[1] = as_jsval(_myJSContext, myKeyEvent.type == y60::Event::KEY_DOWN);
                argv[2] = as_jsval(_myJSContext, 0); // TODO: mouse X POS
                argv[3] = as_jsval(_myJSContext, 0); // TODO: mouse Y POS
                argv[4] = as_jsval(_myJSContext, ((myKeyEvent.modifiers & y60::KEYMOD_SHIFT) !=0));
                argv[5] = as_jsval(_myJSContext, ((myKeyEvent.modifiers & y60::KEYMOD_CTRL) !=0));
                argv[6] = as_jsval(_myJSContext, ((myKeyEvent.modifiers & y60::KEYMOD_ALT) !=0));
                JSA_CallFunctionName(_myJSContext, _myEventListener, "onKey", 7, argv, &rval);
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
            jsval argv[4], rval;
            argv[0] = as_jsval(_myJSContext, myMouseEvent.button);
            argv[1] = as_jsval(_myJSContext, myMouseEvent.type == y60::Event::MOUSE_BUTTON_DOWN);
            if (getOrientation() == PORTRAIT_ORIENTATION) {
                argv[2] = as_jsval(_myJSContext, myMouseEvent.position[1]);
                argv[3] = as_jsval(_myJSContext, getWidth() - myMouseEvent.position[0]);
            } else {
                argv[2] = as_jsval(_myJSContext, myMouseEvent.position[0]);
                argv[3] = as_jsval(_myJSContext, myMouseEvent.position[1]);
            }

            JSA_CallFunctionName(_myJSContext, _myEventListener, "onMouseButton", 4, argv, &rval);
        }
    }

    void
    AbstractRenderWindow::onMouseMotion(y60::Event & theEvent) {
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onMouseMotion")) {
            y60::MouseEvent & myMouseEvent = dynamic_cast<y60::MouseEvent&>(theEvent);
            jsval argv[2], rval;
            if (getOrientation() == PORTRAIT_ORIENTATION) {
                argv[0] = as_jsval(_myJSContext, myMouseEvent.position[1]);
                argv[1] = as_jsval(_myJSContext, getWidth() - myMouseEvent.position[0]);
            } else {
                argv[0] = as_jsval(_myJSContext, myMouseEvent.position[0]);
                argv[1] = as_jsval(_myJSContext, myMouseEvent.position[1]);
            }

            JSA_CallFunctionName(_myJSContext, _myEventListener, "onMouseMotion", 2, argv, &rval);
        }
    }

    void
    AbstractRenderWindow::onMouseWheel(y60::Event & theEvent) {
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onMouseWheel")) {
            y60::MouseEvent & myMouseEvent = dynamic_cast<y60::MouseEvent&>(theEvent);
            jsval argv[2], rval;

            argv[0] = as_jsval(_myJSContext, myMouseEvent.delta[0]);
            argv[1] = as_jsval(_myJSContext, myMouseEvent.delta[1]);

            JSA_CallFunctionName(_myJSContext, _myEventListener, "onMouseWheel", 2, argv, &rval);
        }
    }

    void
    AbstractRenderWindow::onAxis(y60::Event & theEvent) {
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onAxis")) {
            y60::AxisEvent & myAxisEvent = dynamic_cast<y60::AxisEvent&>(theEvent);
            jsval argv[3], rval;
            argv[0] = as_jsval(_myJSContext, myAxisEvent.device);
            argv[1] = as_jsval(_myJSContext, myAxisEvent.axis);
            argv[2] = as_jsval(_myJSContext, myAxisEvent.value);
            JSA_CallFunctionName(_myJSContext, _myEventListener, "onAxis", 3, argv, &rval);
        }
    }

    void
    AbstractRenderWindow::onTouch(y60::Event & theEvent) {
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onTouch")) {
            y60::TouchEvent & myEvent = dynamic_cast<y60::TouchEvent&>(theEvent);

            jsval argv[5], rval;
            int nargs = 0;
            argv[nargs++] = as_jsval(_myJSContext, myEvent.device);
            argv[nargs++] = as_jsval(_myJSContext, myEvent.position[0]);
            argv[nargs++] = as_jsval(_myJSContext, myEvent.position[1]);
            argv[nargs++] = as_jsval(_myJSContext, myEvent.size[0]);
            argv[nargs++] = as_jsval(_myJSContext, myEvent.size[1]);
            argv[nargs++] = as_jsval(_myJSContext, myEvent.intensity);
            JSA_CallFunctionName(_myJSContext, _myEventListener, "onTouch", nargs, argv, &rval);
        }
    }

    void
    AbstractRenderWindow::onResize(y60::Event & theEvent) {
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onResize")) {
            y60::WindowEvent & myWindowEvent = dynamic_cast<y60::WindowEvent&>(theEvent);
            jsval argv[2], rval;
            argv[0] = as_jsval(_myJSContext, myWindowEvent.width);
            argv[1] = as_jsval(_myJSContext, myWindowEvent.height);
            JSA_CallFunctionName(_myJSContext, _myEventListener, "onResize", 2, argv, &rval);
        }
    }

    void
    AbstractRenderWindow::onButton(y60::Event & theEvent) {
        if (_myEventListener && JSA_hasFunction(_myJSContext, _myEventListener, "onButton")) {
            y60::ButtonEvent & myButtonEvent = dynamic_cast<y60::ButtonEvent&>(theEvent);
            jsval argv[3], rval;
            argv[0] = as_jsval(_myJSContext, myButtonEvent.device);
            argv[1] = as_jsval(_myJSContext, myButtonEvent.button);
            if (myButtonEvent.type == y60::Event::BUTTON_UP) {
                argv[2] = as_jsval(_myJSContext, 0);
            } else {
                argv[2] = as_jsval(_myJSContext, 1);
            }
            JSA_CallFunctionName(_myJSContext, _myEventListener, "onButton", 3, argv, &rval);
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
                    << "' is ambiguous. Statistical timers will be missleading." << endl;
            }
        }
        _myExtensions.push_back(theExtension);

        AC_INFO << " added extension " << theExtension->getName();
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
            ImagePtr myImage = _myScene->getTextureManager()->getImage(theFileName);
            if (myImage) {
                return myImage;
            }
        }
        return ImagePtr(0);
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
                        getBytesRequired(theY * myImage->get<ImageWidthTag>() + theX, myImage->getEncoding());
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
                        getBytesRequired(theY * myImage->get<ImageWidthTag>() + theX, myImage->getEncoding());

                    unsigned myUsedBytesPerPixel = getBytesRequired(1, myImage->getEncoding());
                    for (unsigned i = 0; i < myUsedBytesPerPixel; ++i) {
                        *(myPixelPtr + i) = theColor[i];
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
                    const std::string & theFont)
    {
        asl::Vector2f myRelativePosition(thePixelPosition[0] / getWidth(), thePixelPosition[1] / getHeight());
        _myRenderer->getTextManager().addText(myRelativePosition, theString, theFont);
    }

    void AbstractRenderWindow::setTextColor(const asl::Vector4f & theTextColor, const asl::Vector4f & theBackColor)
    {
        _myRenderer->getTextManager().setColor(theTextColor, theBackColor);
    }

    asl::Vector2i AbstractRenderWindow::renderTextAsImage(dom::NodePtr theImageNode,
                                 const std::string & theString, const std::string & theFont,
                                 const unsigned int & theTargetWidth, const unsigned int & theTargetHeight)
    {
        return _myRenderer->getTextManager().renderTextAsImage(*(_myScene->getTextureManager()),
                        theImageNode, theString, theFont, theTargetWidth, theTargetHeight);
    }


    void AbstractRenderWindow::setTextStyle(unsigned int theStyle) {
        _myRenderer->getTextManager().setTextStyle(y60::Text::RENDERSTYLE(theStyle));
    }

    void AbstractRenderWindow::setTextPadding(int topPadding, int bottomPadding, int leftPadding, int rightPadding) {
        _myRenderer->getTextManager().setPadding(topPadding, bottomPadding, leftPadding, rightPadding);
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
    AbstractRenderWindow::setFixedDeltaT(const float & theDeltaT) {
        _myFixedDeltaT = theDeltaT;
    }
    const float &
    AbstractRenderWindow::getFixedDeltaT() const {
        return _myFixedDeltaT;
    }

}

