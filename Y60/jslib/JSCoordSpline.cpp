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
//   $RCSfile: JSCoordSpline.cpp,v $
//   $Author: christian $
//   $Revision: 1.5 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSCoordSpline.h"
//#include "JScppUtils.h"
//#include "JSNode.h"
#include "JSVector.h"
#include "JSKeyframe.h"

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

typedef asl::CoordSpline NATIVE;

static JSBool
position(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a position for a given timestamp");
    DOC_PARAM("theTimestamp", "", DOC_TYPE_FLOAT);
    DOC_RVAL("Position", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<JSCoordSpline::NATIVE>::call(&JSCoordSpline::NATIVE::xyz,cx,obj,argc,argv,rval);
}
static JSBool
orientation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a orientation for a given timestamp");
    DOC_PARAM("theTimestamp", "", DOC_TYPE_FLOAT);
    DOC_RVAL("Orientation", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<JSCoordSpline::NATIVE>::call(&JSCoordSpline::NATIVE::getHPR,cx,obj,argc,argv,rval);
}
static JSBool
print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Print Spline");
    DOC_END;
    return Method<JSCoordSpline::NATIVE>::call(&JSCoordSpline::NATIVE::print,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSCoordSpline::Functions() {
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"position",             position,               1},
        {"orientation",          orientation,            1},
        {"print",                print,                  0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSCoordSpline::Properties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSConstIntPropertySpec *
JSCoordSpline::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSCoordSpline::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSCoordSpline::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSCoordSpline::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    /*switch (theID) {
        default:*/
            JS_ReportError(cx,"JSCoordSpline::getProperty: index %d out of range", theID);
            return JS_FALSE;
    //}

}


// setproperty handling
JSBool
JSCoordSpline::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    /*switch (theID) {
        default:*/
            JS_ReportError(cx,"JSCoordSpline::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    //}
}



JSBool
JSCoordSpline::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs a CoordSpline");
    DOC_PARAM("theKeyframes", "Array of Keyframes", DOC_TYPE_ARRAY);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSCoordSpline * myNewObject = 0;

    if (argc == 0) {
        OWNERPTR myNewCoordSpline = OWNERPTR(new CoordSpline());
        myNewObject = new JSCoordSpline(myNewCoordSpline, &(*myNewCoordSpline));
    } else if (argc == 1) {
        std::vector<asl::QuaternionKeyframe> myKeyframes;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myKeyframes)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #1 must be a vector of keyframes");
            return JS_FALSE;
        }
        float myLength = 0;

        OWNERPTR myNewCoordSpline = OWNERPTR(new CoordSpline());
        myNewCoordSpline->init(myKeyframes, myLength, false);
        myNewObject = new JSCoordSpline(myNewCoordSpline, &(*myNewCoordSpline));
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 4 (position, orienation, timestamp, speed) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSCoordSpline::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSCoordSpline::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
    DOC_CREATE(JSCoordSpline);

    return myClass;
}

}

