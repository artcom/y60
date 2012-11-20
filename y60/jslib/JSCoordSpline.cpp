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
#include "JSCoordSpline.h"

#include <y60/jsbase/JSVector.h>
#include "JSKeyframe.h"
#include <y60/jsbase/JSWrapper.impl>

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<asl::CoordSpline, asl::Ptr<asl::CoordSpline>, StaticAccessProtocol>;

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
    return Method<JSCoordSpline::NATIVE>::call(&JSCoordSpline::NATIVE::getQuaternion,cx,obj,argc,argv,rval);
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
        myNewObject = new JSCoordSpline(myNewCoordSpline, myNewCoordSpline.get());
    } else if (argc == 1) {
        std::vector<asl::QuaternionKeyframe> myKeyframes;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myKeyframes)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #1 must be a vector of keyframes");
            return JS_FALSE;
        }
        float myLength = 0;

        OWNERPTR myNewCoordSpline = OWNERPTR(new CoordSpline());
        myNewCoordSpline->init(myKeyframes, myLength, false);
        myNewObject = new JSCoordSpline(myNewCoordSpline, myNewCoordSpline.get());
    } else if (argc == 2) {
        std::vector<asl::QuaternionKeyframe> myKeyframes;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myKeyframes)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #1 must be a vector of keyframes");
            return JS_FALSE;
        }
        HermiteInitMode myInitMode = catmull_rom;
        string myInitModeString;
        if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myInitModeString)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #2 must be a string");
            return JS_FALSE;
        }
        float myLength = 0;
        try {
            myInitMode = (HermiteInitMode)getEnumFromString(myInitModeString, asl::HermiteInitModeString);
        } catch (asl::ParseException) {
            AC_WARNING << myInitModeString << " cannot be parsed probably is not a valid string";
        }
        OWNERPTR myNewCoordSpline = OWNERPTR(new CoordSpline(myInitMode));
        myNewCoordSpline->init(myKeyframes, myLength, false);
        myNewObject = new JSCoordSpline(myNewCoordSpline, myNewCoordSpline.get());
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
    DOC_MODULE_CREATE("Math", JSCoordSpline);
    return myClass;
}

}

