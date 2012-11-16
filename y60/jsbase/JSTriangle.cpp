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
#include "JSTriangle.h"

#include "JSVector.h"
#include "JSPlane.h"
#include "JSWrapper.impl"
#include <iostream>

using namespace std;

namespace jslib {

template class JSWrapper<asl::Triangle<TriangleNumber> >;

typedef TriangleNumber Number;
typedef asl::Triangle<Number> NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a string representation of the triangle.");
    DOC_END;
    std::string myStringRep = asl::as_string(JSTriangle::getJSWrapper(cx,obj).getNative());
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSTriangle::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {"toString",           toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSTriangle::Properties() {
    static JSPropertySpec myProperties[] = {
        {"length", PROP_length, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {"normal", PROP_normal, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute Vector3f
        {"plane", PROP_plane, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},     // readonly attribute Plane
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSTriangle::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSTriangle::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSTriangle::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSTriangle::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_normal:
            *vp = as_jsval(cx, getNative().normal());
            return JS_TRUE;
        case PROP_plane:
            *vp = as_jsval(cx, getNative().plane());
            return JS_TRUE;
        case PROP_length:
            *vp = as_jsval(cx, 3);
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSTriangle::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
};
JSBool JSTriangle::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    *vp = as_jsval(cx, getNative()[theIndex]);
    return JS_TRUE;
};

// setproperty handling
JSBool
JSTriangle::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JS_ReportError(cx,"JSTriangle::setPropertySwitch: index %d out of range", theID);
    return JS_FALSE;
};
JSBool
JSTriangle::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    if (JSVAL_IS_OBJECT(*vp)) {
        JSObject * myValObj = JSVAL_TO_OBJECT(*vp);
        if (JSVector<asl::Vector3<Number> >::Class() != JSA_GetClass(cx,myValObj)) {
            myValObj = JSVector<asl::Vector3<Number> >::Construct(cx, *vp);
        }
        if (myValObj) {
            const asl::Vector3<Number> & myNativeArg = JSVector<asl::Vector3<Number> >::getNativeRef(cx, JSVAL_TO_OBJECT(*vp));
            openNative()[theIndex] = myNativeArg;
            closeNative();
            return JS_TRUE;
        } else {
            JS_ReportError(cx,"JSTriangle::setPropertyIndex: bad argument type, Vector3 expected");
            return JS_FALSE;
        }
    }
    return JS_TRUE;
};

JSBool
JSTriangle::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs a triangle from three points, from a triangle or an empty one.");
    DOC_PARAM("thePoint", "Point 1 as Vector3", DOC_TYPE_VECTOR3F);
    DOC_PARAM("thePoint", "Point 2 as Vector3", DOC_TYPE_VECTOR3F);
    DOC_PARAM("thePoint", "Point 3 as Vector3", DOC_TYPE_VECTOR3F);
    DOC_RESET;
    DOC_PARAM("thePoints", "Array[3] of Vector3", DOC_TYPE_ARRAY);
    DOC_RESET;
    DOC_PARAM("theTriangle", "Triangle", DOC_TYPE_TRIANGLE);
    DOC_RESET;
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSTriangle * myNewObject = 0;
    JSTriangle::NativeValuePtr myNewValue = JSTriangle::NativeValuePtr(new dom::SimpleValue<asl::Triangle<Number> >(0));
    asl::Triangle<Number> & myNewTriangle = myNewValue->openWriteableValue(); // we close it only on success, otherwise we trash it anyway

    if (argc == 0) {
        // construct empty
        myNewObject=new JSTriangle(myNewValue);
    } else {
        if (argc == 3) {
            // construct from three points
            for (int i = 0; i < 3 ;++i) {
                JSObject * myObject = JSVector<asl::Vector3<Number> >::Construct(cx, argv[i]);
                if (!myObject) {
                    JS_ReportError(cx,"JSTriangle::Constructor: argument must be 3 vectors or arrays of size 3",ClassName());
                    return JS_FALSE;
                }
                myNewTriangle[i] = JSVector<asl::Vector3<Number> >::getNativeRef(cx,myObject);
            }
            myNewObject=new JSTriangle(myNewValue);
        } else if (argc == 1) {
            // construct from one Triangle
            if (JSVAL_IS_VOID(argv[0])) {
                JS_ReportError(cx,"JSTriangle::Constructor: bad argument #1 (undefined)");
                return JS_FALSE;
            }
            JSObject * myArgument;
            if (!JS_ValueToObject(cx, argv[0], &myArgument)) {
                JS_ReportError(cx,"JSTriangle::Constructor: bad argument type, Triangle expected");
                return JS_FALSE;
            }
            if (JSA_GetClass(cx,myArgument) == Class()) {
                myNewTriangle = getJSWrapper(cx, myArgument).getNative();
                myNewObject=new JSTriangle(myNewValue);
            }
        } else {
            JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 0,1 or 3, got %d",ClassName(), argc);
            return JS_FALSE;
        }
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        myNewValue->closeWriteableValue();
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSTriangle::Constructor: bad parameters");
    return JS_FALSE;
}



JSObject *
JSTriangle::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
    DOC_MODULE_CREATE("Math", JSTriangle);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Triangle<Number>  & theTriangle) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Triangle<Number> >::Class()) {
                theTriangle = JSClassTraits<asl::Triangle<Number> >::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, const asl::Triangle<Number>  & theValue) {
    JSObject * myReturnObject = JSTriangle::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSTriangle::NativeValuePtr theValue) {
    JSObject * myObject = JSTriangle::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

}
