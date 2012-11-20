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
#include "JSPlane.h"

#include "JSVector.h"
#include "JSLine.h"
#include "JSWrapper.impl"
#include <asl/math/intersection.h>
#include <iostream>

using namespace std;

namespace jslib {

template class JSWrapper<asl::Plane<PlaneNumber> >;

typedef PlaneNumber Number;
typedef asl::Plane<Number> NATIVE;

static JSBool
normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Normalizes the plane.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::normalize,cx,obj,argc,argv,rval);
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints a string representation of the plane.");
    DOC_END;
    std::string myStringRep = asl::as_string(JSPlane::getJSWrapper(cx,obj).getNative());
*rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
intersect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Computes the line of intersection between this plane and the plane given. "
        "Returns null if no single line of intersection can be found.");
    DOC_END;
    NATIVE myNative;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
    NATIVE myPlane;
    convertFrom(cx, argv[0], myPlane);
    asl::Line<Number> myResult;
    bool hasIntersection = asl::intersection(myNative, myPlane, myResult);
    if (hasIntersection) {
        *rval = as_jsval(cx, myResult);
    } else {
        *rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

static JSBool
distance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Calculates the distance between a given point and the plane.");
        DOC_PARAM("thePoint", "The point to calcluate the distance for", DOC_TYPE_VECTOR3F);
        DOC_RVAL("The unsigend distance to the plane", DOC_TYPE_FLOAT);
        DOC_END;
        ensureParamCount(argc, 1);
        NATIVE myPlane;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myPlane);

        asl::Point3f myPoint;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myPoint)) {
            JS_ReportError(cx, "JSPlane::distance(): Argument #1 must be a point");
            return JS_FALSE;
        }

        float myDistance = asl::distance(myPoint, myPlane);
        *rval = as_jsval(cx, myDistance);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
signedDistance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Calculates the signed distance between a given point and the plane.");
    DOC_PARAM("thePoint", "The point to calcluate the distance for", DOC_TYPE_VECTOR3F);
    DOC_RVAL("The signed distance to the plane", DOC_TYPE_FLOAT);
    DOC_END
    try {
        ensureParamCount(argc, 1);
        NATIVE myPlane;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myPlane);

        asl::Point3f myPoint;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myPoint)) {
            JS_ReportError(cx, "JSPlane::signedDistance(): Argument #1 must be a point");
            return JS_FALSE;
        }

        float myDistance = asl::signedDistance(myPoint, myPlane);
        *rval = as_jsval(cx, myDistance);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSPlane::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {"normalize",          normalize,               0},
        {"toString",           toString,                0},
        {"intersect",          intersect,               1},
        {"distance",           distance,                1},
        {"signedDistance",     signedDistance,          1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSPlane::Properties() {
    static JSPropertySpec myProperties[] = {
        {"normal", PROP_normal, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // readwrite attribute Vector3f
        {"offset", PROP_offset, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // readwrite attribute float
        {0}
    };
    return myProperties;
}
JSConstIntPropertySpec *
JSPlane::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSPlane::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSPlane::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSPlane::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_normal:
                *vp = as_jsval(cx, getNative().normal);
                return JS_TRUE;
            case PROP_offset:
                *vp = as_jsval(cx, getNative().offset);
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSPlane::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
};

// setproperty handling
JSBool
JSPlane::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    jsval dummy;
    switch (theID) {
        case PROP_normal:
            return Method<NATIVE>::call(&NATIVE::setNormal, cx, obj, 1, vp, &dummy);
        case PROP_offset:
            return Method<NATIVE>::call(&NATIVE::setOffset, cx, obj, 1, vp, &dummy);
        default:
            JS_ReportError(cx,"JSPlane::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
};


JSBool
JSPlane::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Construct a plane from normal/point, with normal/distance, from another plane or construct an empty one.");
    DOC_PARAM("theVector3", "The normal of the plane", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theVector3", "A point on the plane", DOC_TYPE_VECTOR3F);
    DOC_RESET;
    DOC_PARAM("thePlane", "The other plane", DOC_TYPE_PLANE);
    DOC_RESET;
    DOC_PARAM("theDistance", "The distance of the plane to the origin.", DOC_TYPE_FLOAT);
    DOC_PARAM("theNormal", "The normal of the plane", DOC_TYPE_VECTOR3F);
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSPlane * myNewObject = 0;
    JSPlane::NativeValuePtr myNewValue = JSPlane::NativeValuePtr(new dom::SimpleValue<asl::Plane<Number> >(0));
    asl::Plane<Number> & myNewPlane = myNewValue->openWriteableValue(); // we close it only on success, otherwise we trash it anyway
    if (argc == 0) {
        // construct empty
        myNewObject=new JSPlane(myNewValue);
    } else {
        if (argc == 2) {
            JSObject * myObject = JSVector<asl::Vector3<Number> >::Construct(cx, argv[0]);
            if (!myObject) {
                JS_ReportError(cx,"JSPlane::Constructor: first argument must be a normal vector of size 3",ClassName());
                return JS_FALSE;
            }
            jsdouble myNumber;
            if (JS_ValueToNumber(cx,argv[1],&myNumber) && !JSDOUBLE_IS_NaN(myNumber)) {
                // use normal/distance construction
                myNewPlane = asl::Plane<Number>(JSVector<asl::Vector3<Number> >::getNativeRef(cx,myObject), Number(myNumber));
                myNewPlane.normalize();
            } else {
                // use normal/point-on-plane construction
                JSObject * myObject2 = JSVector<asl::Vector3<Number> >::Construct(cx, argv[1]);
                if (!myObject2) {
                    JS_ReportError(cx,"JSPlane::Constructor: second argument must be a distance number or point on the plane of size 3");
                    return JS_FALSE;
                }
                myNewPlane = asl::Plane<Number>(JSVector<asl::Vector3<Number> >::getNativeRef(cx,myObject),
                                                JSVector<asl::Vector3<Number> >::getNativeRef(cx,myObject2));
                myNewPlane.normalize();
           }
            myNewObject=new JSPlane(myNewValue);
        } else if (argc == 1) {
            // construct from one Plane
            JSObject * myArgument;
            if (JSVAL_IS_VOID(argv[0])) {
                JS_ReportError(cx,"JSPlane::Constructor: bad argument #1 (undefined)",ClassName());
                return JS_FALSE;
            }
            if (!JS_ValueToObject(cx, argv[0], &myArgument)) {
                JS_ReportError(cx,"JSPlane::Constructor: bad argument type, Plane expected");
                return JS_FALSE;
            }
            if (JSA_GetClass(cx,myArgument) == Class()) {
                myNewPlane = getJSWrapper(cx, myArgument).getNative();
                myNewObject=new JSPlane(myNewValue);
            }
        } else {
            JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 0,1 or 2, got %d",ClassName(), argc);
            return JS_FALSE;
        }
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        myNewValue->closeWriteableValue();
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSPlane::Constructor: bad parameters");
    return JS_FALSE;
}


JSObject *
JSPlane::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
    DOC_MODULE_CREATE("Math",JSPlane);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Plane<Number>  & thePlane) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Plane<Number> >::Class()) {
                thePlane = JSClassTraits<asl::Plane<Number> >::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, const asl::Plane<Number>  & theValue) {
    JSObject * myReturnObject = JSPlane::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSPlane::NativeValuePtr theValue) {
    JSObject * myObject = JSPlane::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

}
