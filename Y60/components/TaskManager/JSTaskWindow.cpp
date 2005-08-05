//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSTaskWindow.cpp,v $
//   $Author: christian $
//   $Revision: 1.12 $
//   $Date: 2005/04/28 17:12:56 $
//
//
//=============================================================================

#include "JSTaskWindow.h"
#include <y60/JScppUtils.h>
#include <y60/JSVector.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) // x

namespace jslib {

    static JSBool
    setAlwaysOnTop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::setAlwaysOnTop,cx,obj,argc,argv,rval);
    }
    static JSBool
    validateRect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::validateRect,cx,obj,argc,argv,rval);
    }
    static JSBool
    enable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::enable,cx,obj,argc,argv,rval);
    }
    static JSBool
    disable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::disable,cx,obj,argc,argv,rval);
    }
    static JSBool
    close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::close,cx,obj,argc,argv,rval);
    }
    static JSBool
    destroy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::destroy,cx,obj,argc,argv,rval);
    }
    static JSBool
    activate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::activate,cx,obj,argc,argv,rval);
    }
    static JSBool
    capture(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::capture,cx,obj,argc,argv,rval);
    }
    static JSBool
    resize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::resize,cx,obj,argc,argv,rval);
    }
    static JSBool
    maximize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::maximize,cx,obj,argc,argv,rval);
    }
    static JSBool
    mimimize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::minimize,cx,obj,argc,argv,rval);
    }
    static JSBool
    restore(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::restore,cx,obj,argc,argv,rval);
    }
    static JSBool
    setAlpha(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::setAlpha,cx,obj,argc,argv,rval);
    }
    static JSBool
    fade(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::fade,cx,obj,argc,argv,rval);
    }
    static JSBool
    hideDecoration(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        return Method<JSTaskWindow::NATIVE>::call(&JSTaskWindow::NATIVE::hideDecoration,cx,obj,argc,argv,rval);
    }

    JSFunctionSpec *
    JSTaskWindow::Functions() {
        static JSFunctionSpec myFunctions[] = {
            // name                  native            nargs
            {"close",                close,             0},
            {"enable",               enable,            0},
            {"disable",              disable,           0},
            {"setAlwaysOnTop",       setAlwaysOnTop,    0},
            {"validateRect",         validateRect,      0},
            {"destroy",              destroy,           0},
            {"activate",             activate,          0},
            {"maximize",             maximize,          0},
            {"minimize",             mimimize,          0},
            {"restore",              restore,           0},
            {"capture",              capture,           1},
            {"resize",               resize,            2},
            {"setAlpha",             setAlpha,          1},
            {"fade",                 fade,              1},
            {"hideDecoration",       hideDecoration,    0},
            {0}
        };
        return myFunctions;
    }

    JSPropertySpec *
    JSTaskWindow::Properties() {
        static JSPropertySpec myProperties[] = {
            {"windowname", PROP_windowName, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"visible", PROP_visible, JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"position", PROP_position, JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {0}
        };
        return myProperties;
    }

    // getproperty handling
    JSBool
    JSTaskWindow::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        switch (theID) {
            case PROP_windowName:
                *vp = as_jsval(cx, getNative().getName());
                return JS_TRUE;
            case PROP_visible:
                *vp = as_jsval(cx, getNative().isVisible());
                return JS_TRUE;
            case PROP_position:
                *vp = as_jsval(cx, getNative().getPosition());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSTaskWindow::getProperty: index %d out of range", theID);
                return JS_FALSE;
        }
    }

    // setproperty handling
    JSBool
    JSTaskWindow::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        jsval dummy;
        switch (theID) {
            case PROP_visible:
                return Method<NATIVE>::call(&NATIVE::setVisible, cx, obj, 1, vp, &dummy);
            case PROP_position:
                return Method<NATIVE>::call(&NATIVE::setPosition, cx, obj, 1, vp, &dummy);
            default:
                JS_ReportError(cx,"JSTaskWindow::setPropertySwitch: index %d out of range", theID);
                return JS_FALSE;
        }
    }

    JSBool
    JSTaskWindow::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        if (JSA_GetClass(cx,obj) != Class()) {
            JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
            return JS_FALSE;
        }
        
        OWNERPTR myNewNative;

        if (argc == 0) {
            // Construct empty TaskWindow that will be filled by copy Construct()
            myNewNative = OWNERPTR(new TaskWindow());
        } else if (argc == 1) {
            string myWindowName;
            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myWindowName)) {
                JS_ReportError(cx, "JSTaskWindow::Constructor takes window name as argument");
                return JS_FALSE;
            }
            myNewNative = OWNERPTR(new TaskWindow(myWindowName));
        } else {
            JS_ReportError(cx, "JSTaskWindow::Constructor takes only one argument (window name)");
            return JS_FALSE;
        }

        JSTaskWindow * myNewObject = new JSTaskWindow(myNewNative, &*myNewNative);

        if (!myNewObject) {
            JS_ReportError(cx, "JSTaskWindow::Constructor: bad parameters");
            return JS_FALSE;
        }

        JS_SetPrivate(cx, obj, myNewObject);
        return JS_TRUE;
    }

    JSConstIntPropertySpec *
    JSTaskWindow::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSObject *
    JSTaskWindow::initClass(JSContext *cx, JSObject *theGlobalObject) {
        return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    }

    jsval as_jsval(JSContext *cx, JSTaskWindow::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSTaskWindow::Construct(cx, theOwner, &(*theOwner));
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSTaskWindow::OWNERPTR theOwner, JSTaskWindow::NATIVE * theNative) {
        JSObject * myObject = JSTaskWindow::Construct(cx, theOwner, theNative);
        return OBJECT_TO_JSVAL(myObject);
    }
}