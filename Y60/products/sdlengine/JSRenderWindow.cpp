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

#include <asl/settings.h>

#include <iostream>

#include <y60/jssettings.h>

#include "SDLApp.h"
#include "SDLFontInfo.h"
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

#include "JSRenderWindow.h"

#include <y60/GLAlloc.h>
#include <y60/RenderStatistic.h>
#include <y60/Renderer.h>

#include <y60/Event.h>

#include <asl/Time.h>
#include <asl/numeric_functions.h>
#include <asl/GeometryUtils.h>
#include <asl/Box.h>
#include <asl/PlugInManager.h>

#include <y60/JSBox.h>
#include <y60/JSLine.h>
#include <y60/JSTriangle.h>
#include <y60/JSSphere.h>
#include <y60/JSVector.h>
#include <y60/JSMatrix.h>
#include <y60/JSNode.h>
#include <y60/JSNodeList.h>
#include <y60/JSAbstractRenderWindow.h>

#define DB(x) // x

using namespace std;
using namespace y60;
using namespace jslib;

// typedef std::map<string, KeyEvent> KeyMap;
typedef SDLWindow NATIVE;
typedef jslib::AbstractRenderWindow BASE;
typedef jslib::JSAbstractRenderWindow<SDLWindow> JSBASE;

namespace jslib {

template <>
struct JSClassTraits<AbstractRenderWindow> : public JSClassTraitsWrapper<SDLWindow, JSRenderWindow> {
    static JSClass * Class() {
        return JSRenderWindow::Base::Class();
    }
};

}

// =============== Wrapper Implementation

// =============== Own Methods
static JSBool
setVideoMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::setVideoMode,cx,obj,argc,argv,rval);
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
go(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::go,cx,obj,argc,argv,rval);
}
static JSBool
loadTTF(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    // Binding is implemented by hand to allow overloading
    try {
        SDLFontInfo::FONTTYPE myFontType = SDLFontInfo::NORMAL;
        std::string myName   = "";
        std::string myPath   = "";
        unsigned    myHeight = 0;

        if (argc != 3 && argc != 4) {
            JS_ReportError(cx, "Renderer::loadTTF(): Wrong number of arguments. Must be three or four");
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
            unsigned short myFontTypeEnum = 0;
            if (!convertFrom(cx, argv[3], myFontTypeEnum)) {
                JS_ReportError(cx, "Renderer::loadTTF(): Argument #4 must be a font face type");
                return JS_FALSE;
            }
            myFontType = SDLFontInfo::FONTTYPE(myFontTypeEnum);
        }

        myObj.getNative().getRenderer()->getTextManager().loadTTF(myName, myPath, myHeight, myFontType);
        return JS_TRUE;
   } HANDLE_CPP_EXCEPTION;
}

static JSBool
setMousePosition(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set the mouse cursor position.");
    DOC_PARAM("theX", DOC_TYPE_INTEGER);
    DOC_PARAM("theY", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::setMousePosition,cx,obj,argc,argv,rval);
}

static JSBool
draw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (argc == 3) {
        if (JSLineSegment::matchesClassOf(cx, argv[0])) {
            typedef void (SDLWindow::*MyMethod)(
                const asl::LineSegment<float> &,
                const asl::Vector4f & theColor,
                const asl::Matrix4f & theTransformation);
            return Method<SDLWindow>::call((MyMethod)&SDLWindow::draw,cx,obj,argc,argv,rval);
        }
        if (JSTriangle::matchesClassOf(cx, argv[0])) {
            typedef void (SDLWindow::*MyMethod)(
                const asl::Triangle<float> &,
                const asl::Vector4f & theColor,
                const asl::Matrix4f & theTransformation);
            return Method<SDLWindow>::call((MyMethod)&SDLWindow::draw,cx,obj,argc,argv,rval);
        }
        if (JSSphere::matchesClassOf(cx, argv[0])) {
            typedef void (SDLWindow::*MyMethod)(
                const asl::Sphere<float> &,
                const asl::Vector4f & theColor,
                const asl::Matrix4f & theTransformation);
            return Method<SDLWindow>::call((MyMethod)&SDLWindow::draw,cx,obj,argc,argv,rval);
        }
        if (JSBox3f::matchesClassOf(cx, argv[0])) {
            typedef void (SDLWindow::*MyMethod)(
                const asl::Box3<float> &,
                const asl::Vector4f & theColor,
                const asl::Matrix4f & theTransformation);
            return Method<SDLWindow>::call((MyMethod)&SDLWindow::draw,cx,obj,argc,argv,rval);
        }
        JS_ReportError(cx,"JSRenderWindow::draw: bad argument type #0");
        return JS_FALSE;
    }
    JS_ReportError(cx,"JSRenderWindow::draw: bad number of arguments, 3 expected");
    return JS_FALSE;
}

static JSBool
getGlyphMetrics(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (argc == 2) {
        std::string myFontName;
        if (!convertFrom(cx,argv[0],myFontName)) {
            JS_ReportError(cx,"JSRenderWindow::Constructor: parameter 0 must be a string");
            return JS_FALSE;
        }
        std::string myCharacter;
        if (!convertFrom(cx,argv[1],myCharacter)) {
            JS_ReportError(cx,"JSRenderWindow::Constructor: parameter 1 must be a string");
            return JS_FALSE;
        }

        asl::Box2f myGlyphBox;
        JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
        double myAdvance;
        myObj.getNative().getGlyphMetrics(myFontName, myCharacter, myGlyphBox, myAdvance);

        JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
        *rval = OBJECT_TO_JSVAL(myReturnObject);
        if (!JS_DefineProperty(cx, myReturnObject, "min",    as_jsval(cx, myGlyphBox.getMin()), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
        if (!JS_DefineProperty(cx, myReturnObject, "max",    as_jsval(cx, myGlyphBox.getMax()), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;
        if (!JS_DefineProperty(cx, myReturnObject, "advance", as_jsval(cx, myAdvance), 0,0, JSPROP_ENUMERATE)) return JS_FALSE;

        return JS_TRUE;
    }
    JS_ReportError(cx,"JSRenderWindow::intersectWorld: bad number of arguments, 2 expected");
    return JS_FALSE;
}

static JSBool
getKerning(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<SDLWindow>::call(&SDLWindow::getKerning,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSRenderWindow::Functions() {
    static JSFunctionSpec myFunctions[] = {
        /* name                DERIVED::native          nargs    */
        {"setVideoMode",       setVideoMode,             3},
        {"resetCursor",        resetCursor,              0},
        {"createCursor",       createCursor,             1},
        {"go",                 go,                       0},
        {"loadTTF",            loadTTF,                  4},
        {"setMousePosition",   setMousePosition,         2},
        {"draw",               draw,                     1},
        {"getGlyphMetrics",    getGlyphMetrics,          7},
        {"getKerning",         getKerning,               3},
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
    PROP_screenSize
};

JSConstIntPropertySpec *
JSRenderWindow::ConstIntProperties() {

    const unsigned short PROP_BOLD       = 1;
    const unsigned short PROP_ITALIC     = 2;
    const unsigned short PROP_BOLDITALIC = 3;

    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

JSPropertySpec *
JSRenderWindow::Properties() {
    static JSPropertySpec myProperties[] = {
        {"decorations",        PROP_windeco,            JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"showMouseCursor",    PROP_showMouseCursor,    JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"showTaskbar",        PROP_showTaskbar,        JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"captureMouseCursor", PROP_captureMouseCursor, JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"autoPause",          PROP_autoPause,          JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"title",              PROP_title,              JSPROP_ENUMERATE|JSPROP_PERMANENT}, // boolean
        {"screenSize",         PROP_screenSize,         JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"position",           PROP_position,           JSPROP_ENUMERATE|JSPROP_PERMANENT}, // Vector2i
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSRenderWindow::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);

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
    DOC_BEGIN("");
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
    JSRenderWindow * myNewObject = new JSRenderWindow(myNewWindow, &(*myNewWindow));
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
    //JSY60BASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, JSBASE::BaseFunctions());
    JSA_AddProperties(cx, theClassProto, JSBASE::BaseProperties());
    //JSA_AddFunctions(cx, theClassProto, Functions());
    createClassDocumentation("AbstractRenderWindow", JSBASE::BaseProperties(),
            JSBASE::BaseFunctions(), 0, 0, 0);
    createClassDocumentation(ClassName(), Properties(), Functions(),
            ConstIntProperties(), 0, 0, "AbstractRenderWindow");
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

namespace jslib {

bool convertFrom(JSContext *cx, jsval theValue, SDLWindow *& theRenderWindow) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRenderWindow::NATIVE>::Class()) {
                theRenderWindow = &(*JSClassTraits<JSRenderWindow::NATIVE>::getNativeOwner(cx,myArgument));
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, asl::Ptr<SDLWindow> theOwner) {
    JSObject * myReturnObject = JSRenderWindow::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

