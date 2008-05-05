//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSVector.h"
#include "JSMatrix.h"
#include "JSQuaternion.h"
#include "JSWrapper.impl"
#include <iostream>

using namespace std;

#define DB(x) // x

namespace jslib {

typedef float Number;
typedef asl::Matrix4<Number> NATIVE;

template class JSWrapper<asl::Matrix4f>;

static JSBool
setRow(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set a row of the matrix.");
    DOC_PARAM("theIndex", "index of the row [0-3] to set", DOC_TYPE_INTEGER);
    DOC_PARAM("theVector", "Row vector to set.", DOC_TYPE_VECTOR4F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::assignRow,cx,obj,argc,argv,rval);
}
static JSBool
getRow(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get a row from the matrix.");
    DOC_PARAM("theIndex", "index of the row to get", DOC_TYPE_INTEGER);
    DOC_RVAL("Row vector", DOC_TYPE_VECTOR4F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::getRow,cx,obj,argc,argv,rval);
}

static JSBool
setColumn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set a column of the matrix.");
    DOC_PARAM("theIndex", "index of the column to set", DOC_TYPE_INTEGER);
    DOC_PARAM("theVector", "Column vector to set.", DOC_TYPE_VECTOR4F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::assignColumn,cx,obj,argc,argv,rval);
}
static JSBool
getColumn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get a column from the matrix.");
    DOC_PARAM("theIndex", "index of the column to get", DOC_TYPE_INTEGER);
    DOC_RVAL("Column vector", DOC_TYPE_VECTOR4F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::getColumn,cx,obj,argc,argv,rval);
}

static JSBool
makeIdentity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Make this matrix an identity matrix.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeIdentity,cx,obj,argc,argv,rval);
}
static JSBool
makeXRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Make this matrix a X-axis rotation matrix.");
    DOC_PARAM("theAngle", "Angle around the X-axis", DOC_TYPE_FLOAT);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeXRotating,cx,obj,argc,argv,rval);
}
static JSBool
makeYRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Make this matrix a Y-axis rotation matrix.");
    DOC_PARAM("theAngle", "Angle around the Y-axis", DOC_TYPE_FLOAT);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeYRotating,cx,obj,argc,argv,rval);
}
static JSBool
makeZRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Make this matrix a Z-axis rotation matrix.");
    DOC_PARAM("theAngle", "Angle around the Z-axis", DOC_TYPE_FLOAT);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeZRotating,cx,obj,argc,argv,rval);
}
static JSBool
makeXYZRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Make this matrix a XYZ rotation matrix.");
    DOC_PARAM("theAngles", "Angles of rotation around XYZ axis", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeXYZRotating,cx,obj,argc,argv,rval);
}
static JSBool
makeRotating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Make this matrix a general rotation matrix.");
    DOC_PARAM("theAxis", "Axis to rotate around", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theAngle", "Angle around the axis", DOC_TYPE_FLOAT);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeRotating,cx,obj,argc,argv,rval);
}

static JSBool
makeScaling(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Make this matrix a scaling matrix.");
    DOC_PARAM("theScale", "Scaling factors for the XYZ axis", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeScaling,cx,obj,argc,argv,rval);
}
static JSBool
makeTranslating(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Make this matrix a translation matrix.");
    DOC_PARAM("theTranslation", "Translation values for the XYZ axis.", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeTranslating,cx,obj,argc,argv,rval);
}

static JSBool
makeLookAt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Make this matrix a transform which looks from eye to center.");
    DOC_PARAM("theEye", "Eye Position", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theCenter", "Center Position", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theUp", "Up Vector", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makeLookAt,cx,obj,argc,argv,rval);
}

static JSBool
makePerspective(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Make this matrix a projection matrix.");
    DOC_PARAM("theFovy", "Field of View", DOC_TYPE_FLOAT);
    DOC_PARAM("theAspect", "Aspect ratio", DOC_TYPE_FLOAT);
    DOC_PARAM("theZNear", "Near Plane Distance", DOC_TYPE_FLOAT);
    DOC_PARAM("theZFar", "Far Plane Distance", DOC_TYPE_FLOAT);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::makePerspective,cx,obj,argc,argv,rval);
}

static JSBool
rotateX(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Rotate this matrix around the X axis.");
    DOC_PARAM("theAngle", "Angle around the X-axis", DOC_TYPE_FLOAT);
    DOC_END;
    typedef void (NATIVE::*MyMethod)(Number);
    return Method<NATIVE>::call((MyMethod)&NATIVE::rotateX,cx,obj,argc,argv,rval);
}
static JSBool
rotateY(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Rotate this matrix around the Y axis.");
    DOC_PARAM("theAngle", "Angle around the Y-axis", DOC_TYPE_FLOAT);
    DOC_END;
    typedef void (NATIVE::*MyMethod)(Number);
    return Method<NATIVE>::call((MyMethod)&NATIVE::rotateY,cx,obj,argc,argv,rval);
}
static JSBool
rotateZ(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Rotate this matrix around the Z axis.");
    DOC_PARAM("theAngle", "Angle around the Z-axis", DOC_TYPE_FLOAT);
    DOC_END;
    typedef void (NATIVE::*MyMethod)(Number);
    return Method<NATIVE>::call((MyMethod)&NATIVE::rotateZ,cx,obj,argc,argv,rval);
}
static JSBool
rotateXYZ(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Rotate this matrix around the XYZ axis.");
    DOC_PARAM("theAngles", "Angles of rotation around XYZ axis", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::rotateXYZ,cx,obj,argc,argv,rval);
}
static JSBool
rotate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Rotate this matrix around an arbitrary axis.");
    DOC_PARAM("theAxis", "Axis to rotate around", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theAngle", "Angle around the axis", DOC_TYPE_FLOAT);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::rotate,cx,obj,argc,argv,rval);
}
static JSBool
scale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Scale this matrix.");
    DOC_PARAM("theScale", "Scaling factors for the XYZ axis", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::scale,cx,obj,argc,argv,rval);
}
static JSBool
translate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Translate this matrix.");
    DOC_PARAM("theTranslation", "Translation values for the XYZ axis.", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::translate,cx,obj,argc,argv,rval);
}

static JSBool
postMultiply(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Multiply this matrix by another matrix.");
    DOC_PARAM("theMatrix", "", DOC_TYPE_MATRIX4F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::postMultiply,cx,obj,argc,argv,rval);
}
static JSBool
invert(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Invert this matrix.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::invert,cx,obj,argc,argv,rval);
}

static JSBool
getTranslation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get translation part of this matrix.");
    DOC_RVAL("Translation vector", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::getTranslation,cx,obj,argc,argv,rval);
}
static JSBool
getScale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get scaling part of this matrix.");
    DOC_RVAL("Scale vector", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::getScale,cx,obj,argc,argv,rval);
}
static JSBool
getRotation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get orientation part of this matrix.");
    DOC_RVAL("orientation vector", DOC_TYPE_VECTOR3F);
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
decompose(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Decompose matrix into it's components.");
    DOC_RVAL("{scale,shear,orientation,position}", DOC_TYPE_OBJECT);
    DOC_END;
    if (argc != 0) {
        JS_ReportError(cx,"Matrix.decompose: wrong number of parameters: %d, 0 expected", argc);
        return JS_FALSE;
    }
    typedef JSMatrix::NativeValuePtr TYPED_PTR;
    TYPED_PTR myObjValPtr;
    if (convertFrom(cx, OBJECT_TO_JSVAL(obj),myObjValPtr)) {
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
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns string representation for this matrix.");
    DOC_END;
    std::string myStringRep = asl::as_string(JSMatrix::getJSWrapper(cx,obj).getNative());
*rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSMatrix::~JSMatrix() {
    DB(AC_TRACE << "JSMatrix DTOR " << this); 
}
 
JSFunctionSpec *
JSMatrix::Functions() {
    AC_DEBUG << "Registering class '" << ClassName() << "'";
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {"setRow",             setRow,                  2},
        {"getRow",             getRow,                  1},
        {"setColumn",          setColumn,               2},
        {"getColumn",          getColumn,               1},
        {"makeIdentity",       makeIdentity,            0},
        {"makeXRotating",      makeXRotating,           1},
        {"makeYRotating",      makeYRotating,           1},
        {"makeZRotating",      makeZRotating,           1},
        {"makeXYZRotating",    makeXYZRotating,         1},
        {"makeRotating",       makeRotating,            2},
        //{"makeQuaternionRotating", makeQuaternionRotating, 1},
        {"makeScaling",        makeScaling,             3},
        {"makeTranslating",    makeTranslating,         3},
        {"makeLookAt",         makeLookAt,              3},
        {"makePerspective",    makePerspective,         4},
        {"rotateX",            rotateX,                 1},
        {"rotateY",            rotateY,                 1},
        {"rotateZ",            rotateZ,                 1},
        {"rotateXYZ",          rotateXYZ,               3},
        {"rotate",             rotate,                  2},
        //{"getRotationAxisAngle",      getRotationAxisAngle,           1},
        {"scale",              scale,                   3},
        {"translate",          translate,               3},
        {"postMultiply",       postMultiply,            1},
        {"invert",             invert,                  0},
        {"getTranslation",     getTranslation,          0},
        {"getRotation",        getRotation,             0},
        {"getScale",           getScale,                0},
        {"decompose",          decompose,               0},
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

#define DEFINE_MATRIXTYPE(NAME) { #NAME, PROP_ ## NAME , asl::NAME }

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
    DOC_BEGIN("Construct a 4x4 matrix. Creates an identity matrix if no argument is given");
    DOC_PARAM("theMatrix", "Copy from given matrix.", DOC_TYPE_MATRIX4F);
    DOC_RESET;
    DOC_PARAM("theQuaternion", "Make rotation matrix from Quaternion.", DOC_TYPE_QUATERNIONF);
    DOC_RESET;
    DOC_PARAM("theNumbers", "16 numbers defining the matrix (row major format)", DOC_TYPE_FLOAT);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSMatrix * myNewObject = 0;
    JSMatrix::NativeValuePtr myNewValue = JSMatrix::NativeValuePtr(new dom::SimpleValue<asl::Matrix4f>(0));
    asl::Matrix4f & myNewMatrix = myNewValue->openWriteableValue();
    if (argc == 0) {
        myNewObject = new JSMatrix(myNewValue);
        myNewMatrix.makeIdentity();
        DB(AC_TRACE << "JSMatrix CTOR " << myNewObject);
    } else if (argc == 16) {
        asl::FixedVector<16,jsdouble> myArgs;
        for (unsigned i = 0; i < 16 ;++i) {
            if (!JS_ValueToNumber(cx, argv[i], &myArgs[i])) {
                JS_ReportError(cx,"JSMatrix::Constructor: parameter %d must be a number",i);
                return JS_FALSE;
            }
#if 0
            myNewMatrix[i/4][i%4] = float(myArgs[i]);
#endif
        }
        myNewMatrix.assign(float(myArgs[0]), float(myArgs[1]), 
                           float(myArgs[2]), float(myArgs[3]),
                           float(myArgs[4]), float(myArgs[5]), 
                           float(myArgs[6]), float(myArgs[7]),
                           float(myArgs[8]), float(myArgs[9]), 
                           float(myArgs[10]),float(myArgs[11]),
                           float(myArgs[12]),float(myArgs[13]),
                           float(myArgs[14]),float(myArgs[15])  );

        myNewObject = new JSMatrix(myNewValue);
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
        } else {
            std::string myText = asl::as_string(cx, argv[0]);
            try {
                myNewMatrix = asl::as<asl::Matrix4f>(myText);
            } catch (asl::Exception & ex) {
                JS_ReportError(cx,"Constructor for %s: malformed argument, not a Matrix4f: %s",ClassName(), myText.c_str());
                return JS_FALSE;
            }
            myNewObject = new JSMatrix(myNewValue);
        }
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 0,1 or 16, got %d",ClassName(), argc);
        return JS_FALSE;
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
    DOC_MODULE_CREATE("Math", JSMatrix);
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
