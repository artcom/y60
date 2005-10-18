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
//   $RCSfile: JSQuaternion.cpp,v $
//   $Author: christian $
//   $Revision: 1.9 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSVector.h"
#include "JSQuaternion.h"
#include <iostream>

using namespace asl;
using namespace std;

namespace jslib {

typedef QuaternionNumber Number;
typedef asl::Quaternion<Number> NATIVE;

static JSBool
assignFromEuler(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Computes the Quaternion from given Euler angles.");
    DOC_PARAM("theEulerAngles", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::assignFromEuler,cx,obj,argc,argv,rval);
}

static JSBool
getImaginaryPart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the imaginary part of the quaternion.");
    DOC_RVAL("", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::getImaginaryPart,cx,obj,argc,argv,rval);
}

static JSBool
getRealPart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the real part of the quaternion.");
    DOC_RVAL("", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_RVAL("", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::getRealPart,cx,obj,argc,argv,rval);
}

static JSBool
setImaginaryPart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets the imaginary part of the quaternion.");
    DOC_PARAM("theImaginary", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::setImaginaryPart,cx,obj,argc,argv,rval);
}

static JSBool
setRealPart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets the real part of the quaternion.");
    DOC_PARAM("theReal", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theReal", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::setRealPart,cx,obj,argc,argv,rval);
}

static JSBool
normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Normalizes the quaternion to unit length.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::normalize,cx,obj,argc,argv,rval);
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints the Quaternion.");
    DOC_END;
    std::string myStringRep = asl::as_string(JSQuaternion::getJSWrapper(cx,obj).getNative());
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}



static JSBool
multiply(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Multiply the Quaternion with a Scalar or another Quaternion.");
    DOC_PARAM("otherReal", DOC_TYPE_INTEGER);
    DOC_RESET;
    DOC_PARAM("otherReal", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theOther", DOC_TYPE_QUATERNIONF);
    DOC_END;
    try {

        ensureParamCount(argc, 1);
        JSClassTraits<NATIVE>::ScopedNativeRef myObjRef(cx, obj);

        NATIVE myQuaternion;
        float  myScalar;
        if (convertFrom(cx, argv[0], myQuaternion)) {
            myObjRef.getNative().mult(myQuaternion);
            return JS_TRUE;
        } else if (convertFrom(cx, argv[0], myScalar)) {
            myObjRef.getNative().mult(myScalar);
            return JS_TRUE;
        } else {
            JS_ReportError(cx, "JSQuaternion::multiply: argument #1 must be a Quaternionf or a scalar.");
            return JS_FALSE;
        }
    } HANDLE_CPP_EXCEPTION
}


static JSBool
createFromEuler(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Computes and assigns the Quaternion from given Euler angles.");
    DOC_PARAM("theRotation", DOC_TYPE_VECTOR3F);
    DOC_END;
    try {

        asl::Vector3f myXYZRotation;

        if (argc == 1) {
            if (!convertFrom(cx, argv[0], myXYZRotation)) {
                JS_ReportError(cx, "JSQuaternion::createFromEuler: argument #1 must be a Vector3f");
                return JS_FALSE;
            }
        } else {
            JS_ReportError(cx, "JSQuaternion::createFromEuler: need one argument");
            return JS_FALSE;
        }
        asl::Quaternionf myQuaternion = Quaternionf::createFromEuler(myXYZRotation);
        *rval = as_jsval(cx, myQuaternion);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
lerp(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("Linear interpolation.");
    DOC_PARAM("q", DOC_TYPE_QUATERNIONF);
    DOC_PARAM("r", DOC_TYPE_QUATERNIONF);
    DOC_PARAM("theT", DOC_TYPE_FLOAT);
    DOC_END;    
    try {
        ensureParamCount(argc, 3);

        asl::Quaternionf myQ1;
        if ( ! convertFrom(cx, argv[0], myQ1)) {
                JS_ReportError(cx, "JSQuaternion::lerp: argument #1 must be a quaternion");
                return JS_FALSE;
        }
        asl::Quaternionf myQ2;
        if ( ! convertFrom(cx, argv[1], myQ2)) {
                JS_ReportError(cx, "JSQuaternion::lerp: argument #2 must be a quaternion");
                return JS_FALSE;
        }
        float myT;
        if ( ! convertFrom(cx, argv[2], myT)) {
                JS_ReportError(cx, "JSQuaternion::lerp: argument #3 must be a float");
                return JS_FALSE;
        }
        asl::Quaternionf myResult = lerp(myQ1, myQ2, myT);
        * rval = as_jsval(cx, myResult);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
slerp(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("Spherical linear interpolation.");
    DOC_PARAM("q", DOC_TYPE_QUATERNIONF);
    DOC_PARAM("r", DOC_TYPE_QUATERNIONF);
    DOC_PARAM("theT", DOC_TYPE_FLOAT);
    DOC_END;    
    try {
        ensureParamCount(argc, 3);

        asl::Quaternionf myQ1;
        if ( ! convertFrom(cx, argv[0], myQ1)) {
                JS_ReportError(cx, "JSQuaternion::slerp: argument #1 must be a quaternion");
                return JS_FALSE;
        }
        asl::Quaternionf myQ2;
        if ( ! convertFrom(cx, argv[1], myQ2)) {
                JS_ReportError(cx, "JSQuaternion::slerp: argument #2 must be a quaternion");
                return JS_FALSE;
        }
        float myT;
        if ( ! convertFrom(cx, argv[2], myT)) {
                JS_ReportError(cx, "JSQuaternion::slerp: argument #3 must be a float");
                return JS_FALSE;
        }
        asl::Quaternionf myResult = slerp(myQ1, myQ2, myT);
        * rval = as_jsval(cx, myResult);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSQuaternion::StaticFunctions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native          nargs
        {"createFromEuler",      createFromEuler,  1},
        {"lerp",                 lerp,             3},
        {"slerp",                slerp,            3},
        {0}
    };
    return myFunctions;
}

JSFunctionSpec *
JSQuaternion::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {"toString",         toString,         0},
        {"assignFromEuler",  assignFromEuler,  1},
        {"getImaginaryPart", getImaginaryPart, 0},
        {"getRealPart",      getRealPart,      0},
        {"setImaginaryPart", setImaginaryPart, 1},
        {"setRealPart",      setRealPart,      1},
        {"normalize",        normalize,        0},
        {"multiply",         multiply,         1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSQuaternion::Properties() {
    static JSPropertySpec myProperties[] = {
        {"imag", PROP_imag, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // readwrite attribute Vector3f
        {"real", PROP_real, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // readwrite attribute float
        {"length", PROP_length, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute boolean
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSQuaternion::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSQuaternion::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

// getproperty handling
JSBool
JSQuaternion::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_imag:
                *vp = as_jsval(cx, getNative().getImaginaryPart());
                return JS_TRUE;
            case PROP_real:
                *vp = as_jsval(cx, getNative().getRealPart());
                return JS_TRUE;
             case PROP_length:
                *vp = as_jsval(cx, length());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSQuaternion::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
};

// setproperty handling
JSBool
JSQuaternion::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    jsval dummy;
    switch (theID) {
        case PROP_imag:
            return Method<NATIVE>::call(&NATIVE::setImaginaryPart, cx, obj, 1, vp, &dummy);
        case PROP_real:
            return Method<NATIVE>::call(&NATIVE::setRealPart, cx, obj, 1, vp, &dummy);
        default:
            JS_ReportError(cx,"JSQuaternion::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
};


JSBool JSQuaternion::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    *vp = as_jsval(cx, getNative()[theIndex]);
    return JS_TRUE;
};

JSBool
JSQuaternion::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    jsdouble myNumber;
    if (JS_ValueToNumber(cx,*vp, &myNumber) && !JSDOUBLE_IS_NaN(myNumber)) {
        openNative()[theIndex] = float(myNumber);
        closeNative();
        return JS_TRUE;
    } else {
        JS_ReportError(cx, "JSQuaternion::setProperty: right hand side of assignment is not a number");
        return JS_FALSE;
    }
};


JSBool
JSQuaternion::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructor");
// construct from one Quaternion
    DOC_PARAM("theOther", DOC_TYPE_QUATERNIONF);
    DOC_RESET;
// construct from axis and angle
    DOC_PARAM("theAxis", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theAngle", DOC_TYPE_INTEGER);
    DOC_RESET;
    DOC_PARAM("theAxis", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theAngle", DOC_TYPE_FLOAT);
    DOC_RESET;
// Construct from explicit real and imaginary part.
    DOC_PARAM("i", DOC_TYPE_FLOAT);
    DOC_PARAM("j", DOC_TYPE_FLOAT);
    DOC_PARAM("k", DOC_TYPE_FLOAT);
    DOC_PARAM("w", DOC_TYPE_FLOAT);
    DOC_END;    
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSQuaternion * myNewObject = 0;
    JSQuaternion::NativeValuePtr myNewValue = JSQuaternion::NativeValuePtr(new dom::VectorValue<asl::Quaternion<Number> >(0));
    asl::Quaternion<Number> & myNewQuaternion = myNewValue->openWriteableValue(); // we close it only on success, otherwise we trash it anyway
    if (argc == 0) {
        // construct empty
        myNewObject = new JSQuaternion(myNewValue);
    } else {
        if (argc == 2) {
            JSObject * myObject = JSVector<asl::Vector3<Number> >::Construct(cx, argv[0]);
            if (!myObject) {
                JS_ReportError(cx,"JSQuaternion::Constructor: argument #1 must be a normalized Vector3f");
                return JS_FALSE;
            }
            jsdouble myNumber;
            if (JS_ValueToNumber(cx,argv[1],&myNumber) && !JSDOUBLE_IS_NaN(myNumber)) {
                // construct from axis and angle
                myNewQuaternion = asl::Quaternion<Number>(JSVector<asl::Vector3<Number> >::getNativeRef(cx,myObject), Number(myNumber));
            } else {
                JSObject * mySecondObject = JSVector<asl::Vector3<Number> >::Construct(cx, argv[1]);
                if (mySecondObject) {
                    // construct from axis and angle
                    myNewQuaternion = asl::Quaternion<Number>(JSVector<asl::Vector3<Number> >::getNativeRef(cx,myObject), JSVector<asl::Vector3<Number> >::getNativeRef(cx,mySecondObject));
                } else {
                    JS_ReportError(cx,"JSQuaternion::Constructor: argument #2 must be a float or a Vector3f");
                    return JS_FALSE;
                }
            }
            myNewObject = new JSQuaternion(myNewValue);
        } else if (argc == 1) {
            // construct from one Quaternion
            if (JSVAL_IS_VOID(argv[0])) {
                JS_ReportError(cx,"JSQuaternion::Constructor: need one argument");
                return JS_FALSE;
            }
            JSObject * myArgument;
            if (!JS_ValueToObject(cx, argv[0], &myArgument)) {
                JS_ReportError(cx,"JSQuaternion::Constructor: argument #1 must be a Quaternion");
                return JS_FALSE;
            }
            if (JSA_GetClass(cx,myArgument) == Class()) {
                myNewQuaternion = getJSWrapper(cx, myArgument).getNative();
                myNewObject = new JSQuaternion(myNewValue);
            }
        } else if (argc == 4) {
            // construct from four numbers
            for (int i = 0; i < 4 ;++i) {
                jsdouble myNumber;
                if (!JS_ValueToNumber(cx,argv[i],&myNumber) && !JSDOUBLE_IS_NaN(myNumber)) {
                    JS_ReportError(cx,"JSQuaternion::Constructor: argument #%d must be a float", i);
                    return JS_FALSE;
                }
                myNewQuaternion[i] = float(myNumber);
            }
            myNewObject = new JSQuaternion(myNewValue);
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
    JS_ReportError(cx,"JSQuaternion::Constructor: bad parameters");
    return JS_FALSE;
}


JSObject *
JSQuaternion::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
            Properties(), Functions(), 0, 0, StaticFunctions());
    DOC_MODULE_CREATE("math", JSQuaternion);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSQuaternion::NativeValuePtr & theValuePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Quaternionf>::Class()) {
                theValuePtr = JSClassTraits<asl::Quaternionf>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Quaternion<Number>  & theQuaternion) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Quaternion<Number> >::Class()) {
                theQuaternion = JSClassTraits<asl::Quaternion<Number> >::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, const asl::Quaternion<Number> & theValue) {
    JSObject * myReturnObject = JSQuaternion::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSQuaternion::NativeValuePtr theValue) {
    JSObject * myObject = JSQuaternion::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

}
