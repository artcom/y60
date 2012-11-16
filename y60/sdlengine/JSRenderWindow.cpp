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
*/

// own header
#include "JSRenderWindow.h"

#include <asl/base/settings.h>

#include <iostream>

#include <y60/jsbase/jssettings.h>
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>

#include "SDLApp.h"
#include "SDLFontInfo.h"
#include <SDL.h>
#include <SDL_syswm.h>

#include <y60/glutil/GLAlloc.h>
#include <y60/glrender/Renderer.h>

#include <y60/input/Event.h>

#include <asl/base/Time.h>
#include <asl/math/numeric_functions.h>
#include <asl/math/GeometryUtils.h>
#include <asl/math/Box.h>
#include <asl/base/PlugInManager.h>

#include <y60/jsbase/JSBox.h>
#include <y60/jsbase/JSLine.h>
#include <y60/jsbase/JSTriangle.h>
#include <y60/jsbase/JSSphere.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSMatrix.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jslib/JSAbstractRenderWindow.h>
#include <y60/jslib/JSAbstractRenderWindow.impl>

#define DB(x) // x

using namespace std;
using namespace y60;
using namespace jslib;

typedef SDLWindow NATIVE;
typedef jslib::AbstractRenderWindow BASE;
typedef jslib::JSAbstractRenderWindow<SDLWindow> JSBASE;

namespace jslib {
    template class JSWrapper<SDLWindow, asl::Ptr<SDLWindow>,
         StaticAccessProtocol>;

    template class JSAbstractRenderWindow<NATIVE>;
}

// =============== Wrapper Implementation

// =============== Own Methods
static JSBool
resize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Resizes the window. Optionally the video mode is set to fullscreen and the screen resolution is set so it fits the current window size best.");
    DOC_PARAM("theWidth", "Target window width in pixel", DOC_TYPE_INTEGER);
    DOC_PARAM("theHeight", "Target window height in pixel", DOC_TYPE_INTEGER);
    DOC_PARAM_OPT("theFullscreenFlag", "If this flag is set to true, the video mode is set to fullscreen.", DOC_TYPE_INTEGER, false);
    DOC_END;
    ensureParamCount(argc, 2, 3);
    unsigned myWidth;
    unsigned myHeight;
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    if ( ! convertFrom(cx, argv[0], myWidth) ||
         ! convertFrom(cx, argv[1], myHeight))
    {
        JS_ReportError(cx, "Renderer::resize(): Argument one and two must be integers");
        return JS_FALSE;
    }
    if (argc == 2) {
        myObj.getNative().setVideoMode(myWidth, myHeight);
        return JS_TRUE;
    } else if (argc == 3) { // argc == 3
        bool myFullscreenFlag;
        if ( ! convertFrom(cx, argv[2], myFullscreenFlag)) {
            JS_ReportError(cx, "Renderer::resize(): Argument three must be bool");
            return JS_FALSE;
        }
        myObj.getNative().setVideoMode(myWidth, myHeight, myFullscreenFlag);
        return JS_TRUE;
    } else {
        // KAPUTT
        return JS_FALSE;
    }
}
static JSBool
createCursor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::createCursor,cx,obj,argc,argv,rval);
}
static JSBool
resetCursor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::resetCursor,cx,obj,argc,argv,rval);
}
static JSBool
stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Closes the window and stops the main renderloop");
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::stop,cx,obj,argc,argv,rval);
}
static JSBool
go(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Opens the window, if it is not alread open and starts the main renderloop.");
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::go,cx,obj,argc,argv,rval);
}
static JSBool
loadTTF(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Load a TTF font");
    DOC_PARAM("theName", "Symbolic font name", DOC_TYPE_STRING);
    DOC_PARAM("theFilename", "Filename to load the font from", DOC_TYPE_STRING);
    DOC_PARAM("theHeight", "Font height", DOC_TYPE_INTEGER);
    DOC_PARAM("theHinting", "Font hinting", DOC_TYPE_ENUMERATION);
    DOC_PARAM_OPT("theFontFace", "Font face, see static properties of Renderer", DOC_TYPE_ENUMERATION, SDLFontInfo::NORMAL);
    DOC_PARAM_OPT("theAscendOffset", "Some fonts need a ascend offset to be rendered correctly by SDL-ttf", DOC_TYPE_INTEGER, 0);
    DOC_END;
    // Binding is implemented by hand to allow overloading
    try {
        SDLFontInfo::FONTTYPE myFontType = SDLFontInfo::NORMAL;
        SDLFontInfo::FONTHINTING myFontHint = SDLFontInfo::AUTOHINTING;

        std::string myName   = "";
        std::string myPath   = "";
        unsigned    myHeight = 0;
        int         myAscendOffset = 0;

        if (argc <3 || argc > 6) {
            JS_ReportError(cx, "Renderer::loadTTF(): Wrong number of arguments. Must be three, four, five or six");
            return JS_FALSE;
        }

        JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);

        if (!convertFrom(cx, argv[0], myName)) {
            JS_ReportError(cx, "Renderer::loadTTF(): Argument #1 must be a font name");
            return JS_FALSE;
        }
        if (!convertFrom(cx, argv[1], myPath)) {
            JS_ReportError(cx, "Renderer::loadTTF(): Argument #2 must be a font path");
            return JS_FALSE;
        }
        if (!convertFrom(cx, argv[2], myHeight)) {
            JS_ReportError(cx, "Renderer::loadTTF(): Argument #3 must be a font height");
            return JS_FALSE;
        }

        if (argc > 3) {
        unsigned short myFontHintEnum = 0;
        if (!convertFrom(cx, argv[3], myFontHintEnum)) {
            JS_ReportError(cx, "Renderer::loadTTF(): Argument #4 must be a font hint type");
            return JS_FALSE;
        }
        myFontHint = SDLFontInfo::FONTHINTING(myFontHintEnum);
        }

        if (argc > 4) {
            unsigned short myFontTypeEnum = 0;
            if (!convertFrom(cx, argv[4], myFontTypeEnum)) {
                JS_ReportError(cx, "Renderer::loadTTF(): Argument #5 must be a font face type");
                return JS_FALSE;
            }
            myFontType = SDLFontInfo::FONTTYPE(myFontTypeEnum);
        }

        if (argc > 5) {
            if (!convertFrom(cx, argv[5], myAscendOffset)) {
                JS_ReportError(cx, "Renderer::loadTTF(): Argument #6 must be a ascend offset");
                return JS_FALSE;
            }
        }

        myObj.getNative().getRenderer()->getTextManager().loadTTF(myName, myPath, myHeight, myFontHint, myFontType, myAscendOffset);
        return JS_TRUE;
   } HANDLE_CPP_EXCEPTION;
}

static JSBool
setMousePosition(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set the mouse cursor position.");
    DOC_PARAM("theX", "X-Position in pixels", DOC_TYPE_INTEGER);
    DOC_PARAM("theY", "Y-Position in pixels", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::setMousePosition,cx,obj,argc,argv,rval);
}

static JSBool
setEventRecorderMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set the EventRecorder mode");
    DOC_PARAM("theMode", "STOP, PLAY, or RECORD", DOC_TYPE_INTEGER);
    DOC_PARAM_OPT("theDiscardFlag", "When theMODE==RECORD, discard previously recorded events", DOC_TYPE_INTEGER, true);
    DOC_END;
    if (argc >= 1) {
        unsigned myMode;
        bool myDiscardFlag = true;
        if (!convertFrom(cx,argv[0],myMode)) {
            JS_ReportError(cx,"JSRenderWindow::setEventRecorderMode: parameter 0 must be an int");
            return JS_FALSE;
        }

        if (argc > 1 && !convertFrom(cx,argv[1],myDiscardFlag)) {
            JS_ReportError(cx,"jsrenderwindow::setEventRecorderMode: parameter 1 must be an bool");
            return JS_FALSE;
        }

        JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
        myObj.getNative().setEventRecorderMode((EventRecorder::Mode) myMode, myDiscardFlag);

        return JS_TRUE;
    }
    JS_ReportError(cx,"JSRenderWindow::setEventRecorderMode: bad number of arguments, 1 expected");
    return JS_FALSE;
}

static JSBool
getEventRecorderMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns current mode of the EventRecorder");
    DOC_RVAL("theMode", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::getEventRecorderMode,cx,obj,argc,argv,rval);
}

static JSBool
loadEvents(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Load events from file");
    DOC_PARAM("theFilename", "Filename to load from", DOC_TYPE_STRING);
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::loadEvents,cx,obj,argc,argv,rval);
}

static JSBool
saveEvents(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Save previously recorded events to file");
    DOC_PARAM("theFilename", "Filename to save to", DOC_TYPE_STRING);
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::saveEvents,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSRenderWindow::Functions() {
    static JSFunctionSpec myFunctions[] = {
        /* name                  DERIVED::native          nargs    */
        {"resize",               resize,                   3},
        {"resetCursor",          resetCursor,              0},
        {"createCursor",         createCursor,             1},
        {"go",                   go,                       0},
        {"stop",                 stop,                     0},
        {"loadTTF",              loadTTF,                  6},
        {"setMousePosition",     setMousePosition,         2},
        {"setEventRecorderMode", setEventRecorderMode,     2},
        {"getEventRecorderMode", getEventRecorderMode,     0},
        {"loadEvents",           loadEvents,               1},
        {"saveEvents",           saveEvents,               1},
        {0}
    };
    return myFunctions;
}

enum PropertyNumbers {
    PROP_windeco = JSBASE::PROP_END,
    PROP_showMouseCursor,
    PROP_showTaskbar,
    PROP_captureMouseCursor,
    PROP_autoPause,
    PROP_title,
    PROP_position,
    PROP_screenSize,
    PROP_swapInterval,
    PROP_multisamplingFactor,
    // ConstInt
    PROP_STOP,
    PROP_PLAY,
    PROP_RECORD
};

JSConstIntPropertySpec *
JSRenderWindow::ConstIntProperties() {

#if 0
    const unsigned short PROP_BOLD       = 1;
    const unsigned short PROP_ITALIC     = 2;
    const unsigned short PROP_BOLDITALIC = 3;
#endif

    static JSConstIntPropertySpec myProperties[] = {
        { "STOP", PROP_STOP, y60::EventRecorder::STOP },
        { "PLAY", PROP_PLAY, y60::EventRecorder::PLAY },
        { "RECORD", PROP_RECORD, y60::EventRecorder::RECORD },
        {0}
    };
    return myProperties;
};

JSPropertySpec *
JSRenderWindow::Properties() {
    static JSPropertySpec myProperties[] = {
        {"decorations",         PROP_windeco,            JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"showMouseCursor",     PROP_showMouseCursor,    JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"showTaskbar",         PROP_showTaskbar,        JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"captureMouseCursor",  PROP_captureMouseCursor, JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"autoPause",           PROP_autoPause,          JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"title",               PROP_title,              JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"screenSize",          PROP_screenSize,         JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"position",            PROP_position,           JSPROP_ENUMERATE|JSPROP_PERMANENT}, // Vector2i
        {"swapInterval",        PROP_swapInterval,       JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSRenderWindow::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    //cerr << "JSRenderWindow::getPropertySwitch: id ="<<int(theID)<<endl;
    switch (theID) {
        case PROP_windeco:
            *vp = as_jsval(cx, myObj.getNative().getWinDeco());
            return JS_TRUE;
        case PROP_showMouseCursor:
            *vp = as_jsval(cx, myObj.getNative().getShowMouseCursor());
            return JS_TRUE;
        case PROP_showTaskbar:
            *vp = as_jsval(cx, myObj.getNative().getShowTaskbar());
            return JS_TRUE;
        case PROP_captureMouseCursor:
            *vp = as_jsval(cx, myObj.getNative().getCaptureMouseCursor());
            return JS_TRUE;
        case PROP_autoPause:
            *vp = as_jsval(cx, myObj.getNative().getAutoPause());
            return JS_TRUE;
        case PROP_title:
            *vp = as_jsval(cx, myObj.getNative().getWindowTitle());
            return JS_TRUE;
        case PROP_position:
            *vp = as_jsval(cx, myObj.getNative().getPosition());
            return JS_TRUE;
        case PROP_screenSize:
            *vp = as_jsval(cx, myObj.getNative().getScreenSize());
            return JS_TRUE;
        case PROP_swapInterval:
            *vp = as_jsval(cx, myObj.getNative().getSwapInterval());
            return JS_TRUE;
        default:
            return JSBASE::getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
    }
}

JSBool JSRenderWindow::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
}

// setproperty handling
JSBool
JSRenderWindow::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    jsval dummy;
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);

    switch (theID) {
        case PROP_windeco:
            return Method<SDLWindow>::call(&SDLWindow::setWinDeco, cx, obj, 1, vp, &dummy);
        case PROP_showMouseCursor:
            return Method<SDLWindow>::call(&SDLWindow::setShowMouseCursor, cx, obj, 1, vp, &dummy);
        case PROP_showTaskbar:
            return Method<SDLWindow>::call(&SDLWindow::setShowTaskbar, cx, obj, 1, vp, &dummy);
        case PROP_captureMouseCursor:
            return Method<SDLWindow>::call(&SDLWindow::setCaptureMouseCursor, cx, obj, 1, vp, &dummy);
        case PROP_autoPause:
            return Method<SDLWindow>::call(&SDLWindow::setAutoPause, cx, obj, 1, vp, &dummy);
        case PROP_title:
            return Method<SDLWindow>::call(&SDLWindow::setWindowTitle, cx, obj, 1, vp, &dummy);
        case PROP_position:
            return Method<SDLWindow>::call(&SDLWindow::setPosition, cx, obj, 1, vp, &dummy);
        case PROP_swapInterval: {
                JSBool myResult = Method<SDLWindow>::call(&SDLWindow::setSwapInterval, cx, obj, 1, vp, &dummy);
                AC_TRACE << "You set swapinterval, you may want to adjust window.fixedFrameTime to 1/(MonitorFrequency/" << myObj.getNative().getSwapInterval() << ") for good video performance?";
                return myResult;
            }
        default:
            return JSBASE::setPropertySwitch(myObj.getNative(),theID, cx, obj, id, vp);
    }
}

JSBool
JSRenderWindow::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
}

JSBool
JSRenderWindow::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new SDL window. Call go() to open it.");
    DOC_RVAL("theNewWindow", DOC_TYPE_OBJECT);
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Base::Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    if (argc != 0) {
        JS_ReportError(cx,"JSRenderWindow::Constructor: bad number of arguments, must be 0");
        return JS_FALSE;
    }

    OWNERPTR myNewWindow = NATIVE::create();
    JSRenderWindow * myNewObject = new JSRenderWindow(myNewWindow, myNewWindow.get());
    if (myNewObject) {
        JS_SetPrivate(cx, obj, myNewObject);

        // Set the JavaScript RenderWindow object as default event listener for the new window
        myNewWindow->setEventListener(obj);
        myNewWindow->setJSContext(cx);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSRenderWindow::Constructor: new JSRenderWindow failed");
    return JS_FALSE;
}

void
JSRenderWindow::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, JSBASE::BaseFunctions());
    JSA_AddProperties(cx, theClassProto, JSBASE::BaseProperties());
    createClassDocumentation(ClassName(), JSBASE::BaseProperties(),
            JSBASE::BaseFunctions(), 0, 0, 0);
    DOC_CREATE(JSRenderWindow);

}

JSObject *
JSRenderWindow::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        // add static props & methods
        JSA_DefineConstInts(cx, myConstructorFuncObj, JSBASE::ConstIntProperties());
        JSA_AddFunctions(cx, myConstructorFuncObj, JSBASE::BaseStaticFunctions());
    } else {
        AC_ERROR << "JSRenderWindow::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

JSPropertySpec *
JSRenderWindow::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSRenderWindow::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

namespace jslib {

bool convertFrom(JSContext *cx, jsval theValue, SDLWindow *& theRenderWindow) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRenderWindow::NATIVE>::Class()) {
                theRenderWindow = JSClassTraits<JSRenderWindow::NATIVE>::getNativeOwner(cx,myArgument).get();
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, asl::Ptr<SDLWindow> theOwner) {
    JSObject * myReturnObject = JSRenderWindow::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

}
