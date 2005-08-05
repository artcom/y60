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
//   $RCSfile: JSMatrix.cpp,v $
//   $Author: christian $
//   $Revision: 1.7 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSVector.h"
#include "JSMatrix.h"
#include "JSQuaternion.h"
#include <iostream>

using namespace std;

namespace jslib {

typedef float Number;
typedef asl::Matrix4<Number> NATIVE;

static JSBool
setRow(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::assignRow,cx,obj,argc,argv,rval);
}
static JSBool
getRow(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::getRow,cx,obj,argc,argv,rval);
}
static JSBool
getColumn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::getColumn,cx,obj,argc,argv,rval);
}
static JSBool
setColumn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::assignColumn,cx,obj,argc,argv,rval);
}
static JSBool
makeIdentity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeIdentity,cx,obj,argc,argv,rval);
}
static JSBool
makeXRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeXRotating,cx,obj,argc,argv,rval);
}
static JSBool
makeYRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeYRotating,cx,obj,argc,argv,rval);
}
static JSBool
makeZRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeZRotating,cx,obj,argc,argv,rval);
}
static JSBool
makeRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeRotating,cx,obj,argc,argv,rval);
}
static JSBool
makeXYZRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeXYZRotating,cx,obj,argc,argv,rval);
}

/*
static JSBool
makeQuaternionRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
//typedef dom::NodePtr (NATIVE::*MyMethod)(int);
return Method<NATIVE>::call(&NATIVE::makeRotating,cx,obj,argc,argv,rval);
}
*/
static JSBool
makeScaling(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    //typedef dom::NodePtr (NATIVE::*MyMethod)(int);
    return Method<NATIVE>::call(&NATIVE::makeScaling,cx,obj,argc,argv,rval);
}
static JSBool
makeTranslating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeTranslating,cx,obj,argc,argv,rval);
}
static JSBool
rotateX(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    typedef void (NATIVE::*MyMethod)(Number);
    return Method<NATIVE>::call((MyMethod)&NATIVE::rotateX,cx,obj,argc,argv,rval);
}
static JSBool
rotateY(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    typedef void (NATIVE::*MyMethod)(Number);
    return Method<NATIVE>::call((MyMethod)&NATIVE::rotateY,cx,obj,argc,argv,rval);
}
static JSBool
rotateZ(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    typedef void (NATIVE::*MyMethod)(Number);
    return Method<NATIVE>::call((MyMethod)&NATIVE::rotateZ,cx,obj,argc,argv,rval);
}
static JSBool
rotate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::rotate,cx,obj,argc,argv,rval);
}
/*
static JSBool
getRotationAxisAngle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
return Method<NATIVE>::call(&NATIVE::getRotation,cx,obj,argc,argv,rval);
}
*/
static JSBool
rotateXYZ(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::rotateXYZ,cx,obj,argc,argv,rval);
}

static JSBool
decompose(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (argc != 0) {
        JS_ReportError(cx,"Matrix.decompose: wrong number of parameters: %d, 0 expected", argc);
        return JS_FALSE;
    }
    typedef JSMatrix::NativeValuePtr TYPED_PTR;
    TYPED_PTR myObjValPtr;
    if (convertFrom(cx, OBJECT_TO_JSVAL(obj),myObjValPtr))
    {

        asl::Vector3<float> myScale;
        asl::Vector3<float> myShear;
        asl::Quaternionf myOrientation;
        asl::Vector3<float> myPosition;
        if  (myObjValPtr->getValue().decompose(myScale, myShear, myOrientation, myPosition)) {
            JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
            if (!JS_DefineProperty(cx, myReturnObject, "scale",       as_jsval(cx, myScale),        0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
            if (!JS_DefineProperty(cx, myReturnObject, "shear",       as_jsval(cx, myShear),        0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
            if (!JS_DefineProperty(cx, myReturnObject, "orientation", as_jsval(cx, myOrientation),  0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
            if (!JS_DefineProperty(cx, myReturnObject, "position",    as_jsval(cx, myPosition),     0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
            *rval = OBJECT_TO_JSVAL(myReturnObject);
            return JS_TRUE;
        }
        *rval = JSVAL_VOID;
        return JS_TRUE;
    }
    return JS_FALSE;
}

static JSBool
scale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::scale,cx,obj,argc,argv,rval);
}
static JSBool
translate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::translate,cx,obj,argc,argv,rval);
}
static JSBool
postMultiply(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::postMultiply,cx,obj,argc,argv,rval);
}
static JSBool
invert(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::invert,cx,obj,argc,argv,rval);
}
static JSBool
getTranslation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::getTranslation,cx,obj,argc,argv,rval);
}
static JSBool
getRotation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    asl::Matrix4f myObj;
    if (convertFrom(cx, OBJECT_TO_JSVAL(obj), myObj)) {
        asl::Vector3<float> myRotation;
        myObj.getRotation(myRotation);
        *rval = as_jsval(cx, myRotation);
        return JS_TRUE;
    }
    return JS_FALSE;
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = asl::as_string(JSMatrix::getJSWrapper(cx,obj).getNative());
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

JSMatrix::~JSMatrix() {
    AC_TRACE << "JSMatrix DTOR " << this << endl; 
}
 
JSFunctionSpec *
JSMatrix::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {"getRow",             getRow,                  1},
        {"getColumn",          getColumn,               1},
        {"setRow",             setRow,                  2},
        {"setColumn",          setColumn,               2},
        {"makeIdentity",       makeIdentity,            0},
        {"makeXRotating",      makeXRotating,           1},
        {"makeYRotating",      makeYRotating,           1},
        {"makeZRotating",      makeZRotating,           1},
        {"makeRotating",       makeRotating,            2},
        {"makeXYZRotating",    makeXYZRotating,         1},
        //{"makeQuaternionRotating", makeQuaternionRotating, 1},
        {"makeScaling",        makeScaling,             3},
        {"makeTranslating",    makeTranslating,         3},
        {"rotateX",            rotateX,                 1},
        {"rotateY",            rotateY,                 1},
        {"rotateZ",            rotateZ,                 1},
        {"rotate",             rotate,                  2},
        //{"getRotationAxisAngle",      getRotationAxisAngle,           1},
        {"rotateXYZ",          rotateXYZ,               3},
        {"decompose",          decompose,               0},
        {"scale",              scale,                   3},
        {"translate",          translate,               3},
        {"postMultiply",       postMultiply,            1},
        {"invert",             invert,                  0},
        {"getTranslation",     getTranslation,          0},
        {"getRotation",        getRotation,             0},
        {"toString",           toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSMatrix::Properties() {
    static JSPropertySpec myProperties[] = {
        {"type", PROP_type, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {"typename", PROP_typename, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {0}
    };
    return myProperties;
}

#define DEFINE_MATRIXTYPE(NAME) { #NAME, PROP_ ## NAME , asl::Matrix4f::NAME }

JSConstIntPropertySpec *
JSMatrix::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_MATRIXTYPE(IDENTITY),
            DEFINE_MATRIXTYPE(X_ROTATING),
            DEFINE_MATRIXTYPE(Y_ROTATING),
            DEFINE_MATRIXTYPE(Z_ROTATING),
            DEFINE_MATRIXTYPE(ROTATING),
            DEFINE_MATRIXTYPE(SCALING),
            DEFINE_MATRIXTYPE(LINEAR),
            DEFINE_MATRIXTYPE(TRANSLATING),
            DEFINE_MATRIXTYPE(AFFINE),
            DEFINE_MATRIXTYPE(UNKNOWN),
        {0}
    };
    return myProperties;
};

JSPropertySpec *
JSMatrix::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSMatrix::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}
 
// getproperty handling
JSBool
JSMatrix::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_type:
                *vp = as_jsval(cx, getNative().getType());
                return JS_TRUE;
            case PROP_typename:
                *vp = as_jsval(cx, getNative().getTypeString());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSMatrix::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
};
JSBool JSMatrix::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    *vp = as_jsval(cx, getNative()[theIndex]);
    return JS_TRUE;
};

// setproperty handling
JSBool
JSMatrix::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JS_ReportError(cx,"JSMatrix::setPropertySwitch: index %d out of range", theID);
    return JS_FALSE;
};
JSBool
JSMatrix::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    if (JSVAL_IS_OBJECT(*vp)) {
        JSObject * myValObj = JSVAL_TO_OBJECT(*vp);
        if (JSVector<asl::Vector4f>::Class() != JSA_GetClass(cx,myValObj)) {
            myValObj = JSVector<asl::Vector4d>::Construct(cx, *vp);
        }
        if (myValObj) {
            const asl::Vector4f & myNativeArg = JSClassTraits<asl::Vector4f>::getNativeRef(cx, JSVAL_TO_OBJECT(*vp));
            WritableNative(this).get()[theIndex] = myNativeArg;
            return JS_TRUE;
        } else {
            JS_ReportError(cx,"JSMatrix::setPropertyIndex: bad argument type, Vector4 expected");
            return JS_FALSE;
        }
    }
    return JS_TRUE;
};

JSBool
JSMatrix::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    AC_DEBUG << "Constructor argc =" << argc << endl;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSMatrix * myNewObject = 0;
    JSMatrix::NativeValuePtr myNewValue = JSMatrix::NativeValuePtr(new dom::SimpleValue<asl::Matrix4f>(0));
    asl::Matrix4f & myNewMatrix = myNewValue->openWriteableValue(); // we close it only on success, otherwise we trash it anyway
    if (argc == 0) {
        myNewObject=new JSMatrix(myNewValue);
        myNewMatrix.makeIdentity();
        AC_TRACE << "JSMatrix CTOR " << myNewObject << endl;
    } else {
        if (argc == 16) {
            asl::FixedVector<16,jsdouble> myArgs;
            for (int i = 0; i < 16 ;++i) {
                if (!JS_ValueToNumber(cx, argv[i], &myArgs[i])) {
                    JS_ReportError(cx,"JSMatrix::Constructor: parameter %d must be a number",i);
                    return JS_FALSE;
                }
                myNewMatrix[i/4][i%4] = float(myArgs[i]);
            }
            myNewObject=new JSMatrix(myNewValue);
        } else if (argc == 1) {
            JSObject * myArgument;

            if (!JS_ValueToObject(cx, argv[0], &myArgument)) {
                JS_ReportError(cx,"JSMatrix<%s>::Constructor: bad argument type",ClassName());
                return JS_FALSE;
            }

            if (JSA_GetClass(cx,myArgument) == Class()) {
                // Copy constructor
                myNewObject = new JSMatrix(myNewValue);
                myNewMatrix = JSClassTraits<asl::Matrix4f>::getNativeRef(cx,myArgument);
            } else if (JSA_GetClass(cx,myArgument) == JSQuaternion::Class()) {
                // constructor from quaternion
                const asl::Quaternionf & myQuat =
                    JSClassTraits<asl::Quaternionf>::getNativeRef(cx,myArgument);
                myNewObject = new JSMatrix(myNewValue);
                myNewMatrix = asl::Matrix4f(myQuat);
            }
        } else {
            JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 0,1 or 16, got %d",ClassName(), argc);
            return JS_FALSE;
        }
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        myNewValue->closeWriteableValue();
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSMatrix::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSMatrix::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_MODULE_CREATE("math", JSMatrix);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSMatrix::NativeValuePtr & theMatrixValuePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Matrix4f>::Class()) {
                theMatrixValuePtr = JSClassTraits<asl::Matrix4f>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Matrix4f & theMatrix) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Matrix4f>::Class()) {
                theMatrix = JSClassTraits<asl::Matrix4f>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}
jsval as_jsval(JSContext *cx, const asl::Matrix4f & theValue) {
    JSObject * myReturnObject = JSMatrix::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSMatrix::NativeValuePtr theValue) {
    JSObject * myObject = JSMatrix::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

}
