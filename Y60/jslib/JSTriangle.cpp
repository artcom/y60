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
//   $RCSfile: JSTriangle.cpp,v $
//   $Author: christian $
//   $Revision: 1.5 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSVector.h"
#include "JSTriangle.h"
#include "JSPlane.h"
#include <iostream>

using namespace std;

namespace jslib {

typedef TriangleNumber Number;
typedef asl::Triangle<Number> NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = asl::as_string(JSTriangle::getJSWrapper(cx,obj).getNative());
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
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
    DOC_BEGIN("");
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
    DOC_MODULE_CREATE("math", JSTriangle);
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
