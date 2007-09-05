//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSCairo.h"

#include <asl/os_functions.h>
#include <y60/JSBlock.h>
#include <dom/Nodes.h>
#include <y60/JSNode.h>
#include <y60/JSScene.h>
#include <y60/JSWrapper.impl>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns information about the Cairo."); DOC_END;
    const Cairo & myCairo = JSCairo::getJSWrapper(cx,obj).getNative();

    std::string myStatusString = "";
    myStatusString = std::string("\nR rocks hard on Y60");
    
    *rval = as_jsval(cx, myStatusString);
    return JS_TRUE;
}

static JSBool
createContextFromImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("create cairo context and surface from given image.");
    DOC_PARAM("theImageNode", "an image node to render into", DOC_TYPE_NODE);
    DOC_RVAL("true", DOC_TYPE_STRING);
    DOC_END;

    // generate default values
    dom::NodePtr myImageNode;
    
    try {
        if (argc != 1) {
            JS_ReportError(cx, "JSCairo::create: needs 2 arguments!");
            return JS_FALSE;
        }

        if (!convertFrom(cx, argv[0], myImageNode)) {
            JS_ReportError(cx, "JSCairo::create: argument #1 must be an imagenode");
            return JS_FALSE;
        }

        return Method<JSCairo::NATIVE>::call(&JSCairo::NATIVE::createContextFromImage, cx, obj, argc, argv, rval);
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

static JSBool
destroy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("destroy cairo context");
    DOC_PARAM("theImageNode", "an image node to render into", DOC_TYPE_NODE);
    DOC_RVAL("true", DOC_TYPE_STRING);
    DOC_END;

    try {
        if (argc != 0) {
            JS_ReportError(cx, "JSCairo::create: needs 0 argument!");
            return JS_FALSE;
        }

        return Method<JSCairo::NATIVE>::call(&JSCairo::NATIVE::destroy, cx, obj, argc, argv, rval);
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

static JSBool
moveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("moveTo");
    DOC_PARAM("theX", "x coordinate", DOC_TYPE_NODE);
    DOC_PARAM("theY", "y coordinate", DOC_TYPE_NODE);
    DOC_RVAL("true", DOC_TYPE_STRING);
    DOC_END;

    try {
        if (argc != 2) {
            JS_ReportError(cx, "JSCairo::create: needs 2 arguments!");
            return JS_FALSE;
        }

        double myX, myY;

        if (!convertFrom(cx, argv[0], myX)) {
            JS_ReportError(cx, "JSCairo::create: argument #1 must be a number");
            return JS_FALSE;
        }

        if (!convertFrom(cx, argv[1], myY)) {
            JS_ReportError(cx, "JSCairo::create: argument #1 must be a number");
            return JS_FALSE;
        }

        return Method<JSCairo::NATIVE>::call(&JSCairo::NATIVE::moveTo, cx, obj, argc, argv, rval);
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

static JSBool
lineTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("lineTo");
    DOC_PARAM("theX", "x coordinate", DOC_TYPE_NODE);
    DOC_PARAM("theY", "y coordinate", DOC_TYPE_NODE);
    DOC_RVAL("true", DOC_TYPE_STRING);
    DOC_END;

    try {
        if (argc != 2) {
            JS_ReportError(cx, "JSCairo::create: needs 2 arguments!");
            return JS_FALSE;
        }

        double myX, myY;

        if (!convertFrom(cx, argv[0], myX)) {
            JS_ReportError(cx, "JSCairo::create: argument #1 must be a number");
            return JS_FALSE;
        }

        if (!convertFrom(cx, argv[1], myY)) {
            JS_ReportError(cx, "JSCairo::create: argument #1 must be a number");
            return JS_FALSE;
        }

        return Method<JSCairo::NATIVE>::call(&JSCairo::NATIVE::lineTo, cx, obj, argc, argv, rval);
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

static JSBool
relLineTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("relative line to");
    DOC_PARAM("theX", "x coordinate", DOC_TYPE_NODE);
    DOC_PARAM("theY", "y coordinate", DOC_TYPE_NODE);
    DOC_RVAL("true", DOC_TYPE_STRING);
    DOC_END;

    try {
        if (argc != 2) {
            JS_ReportError(cx, "JSCairo::create: needs 2 arguments!");
            return JS_FALSE;
        }

        double myX, myY;

        if (!convertFrom(cx, argv[0], myX)) {
            JS_ReportError(cx, "JSCairo::create: argument #1 must be a number");
            return JS_FALSE;
        }

        if (!convertFrom(cx, argv[1], myY)) {
            JS_ReportError(cx, "JSCairo::create: argument #1 must be a number");
            return JS_FALSE;
        }

        return Method<JSCairo::NATIVE>::call(&JSCairo::NATIVE::relLineTo, cx, obj, argc, argv, rval);
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

static JSBool
stroke(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("stroke");
    DOC_RVAL("true", DOC_TYPE_STRING);
    DOC_END;

    try {
        if (argc != 2) {
            JS_ReportError(cx, "JSCairo::create: needs 0 arguments!");
            return JS_FALSE;
        }

        return Method<JSCairo::NATIVE>::call(&JSCairo::NATIVE::stroke, cx, obj, argc, argv, rval);
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

JSCairo::~JSCairo() {
}

JSFunctionSpec *
JSCairo::Functions() {
    IF_REG(cerr << "Registering class '" << ClassName() << "'" << endl);
    static JSFunctionSpec myFunctions[] = {
        // name                     native                  nargs
        {"toString",                toString,               0},
        {"createContextFromImage",  createContextFromImage, 1},
        {"destroy",                 destroy,                2},
        {"moveTo",                  moveTo,                 2},
        {"lineTo",                  lineTo,                 2},
        {"relLineTo",               relLineTo,              2},
        {"stroke",                  stroke,                 0},
      //  {"setSourceRGBA",           setSourceRGBA,          4},
      //  {"setLineWidth",            setLineWidth,           1},
      //  {"fillPreserve",            fillPreserve,           0},
      //  {"rectangle",               rectangle,              4},
      //  {"arc",                     arc,                    6},
      //  {"fill",                    fill,                   0},
      //  {"closePath",               closePath,              0},
      //  {"save",                    save,                   0},
      //  {"restore",                 restore,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSCairo::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSCairo::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSCairo::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        // name             native           nargs
        {0}
    };
    return myFunctions;
}

// getproperty handling
JSBool
JSCairo::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        default:
            JS_ReportError(cx,"JSCairo::getProperty: index %d out of range. Cairo has no properties.", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSCairo::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    
    switch (theID) {
        default:
            JS_ReportError(cx,"JSCairo::setPropertySwitch: index %d out of range. Cairo has no properties.", theID);
            return JS_FALSE;
    }
}

JSBool
JSCairo::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a Cairo Instance.");
    DOC_END;
    
    try{
        ensureParamCount(argc, 0);
        if (JSA_GetClass(cx,obj) != Class()) {
            JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
            return JS_FALSE;
        }
        
        OWNERPTR myCairo = OWNERPTR(new Cairo());
        JSCairo * myNewObject = new JSCairo(myCairo, &(*myCairo));
    
        if (myNewObject) {
            JS_SetPrivate(cx, obj, myNewObject);
            return JS_TRUE;
        } 
    
        JS_ReportError(cx,"JSCairo::Constructor: bad parameters");

        return JS_FALSE;
    } HANDLE_CPP_EXCEPTION;
}

JSConstIntPropertySpec *
JSCairo::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

JSObject *
JSCairo::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), 
                                        Functions(), ConstIntProperties(), 0, StaticFunctions());
    DOC_MODULE_CREATE("Components", JSCairo);
    return myClass;
}

jsval as_jsval(JSContext *cx, JSCairo::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSCairo::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval(JSContext *cx, JSCairo::OWNERPTR theOwner, JSCairo::NATIVE * theSerial) {
    JSObject * myObject = JSCairo::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}
