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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "JSMatrix.h"

#include "JSVector.h"
#include "JSQuaternion.h"
#include "JSWrapper.impl"
#include <iostream>

using namespace std;

#define DB(x) // x

namespace jslib {

typedef float Number;
typedef asl::Matrix4<Number> NATIVE;

template class JSWrapper<asl::Matrix4f>;

#define WRAP_METHOD_BEGIN2(NAME,CPP_NAME,DOCSTR) \
static JSBool \
Call_##CPP_NAME(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { \
    DOC_BEGIN(DOCSTR)

#define WRAP_METHOD_BEGIN(NAME,DOCSTR) WRAP_METHOD_BEGIN2(NAME,NAME,DOCSTR)

#define WRAP_METHOD_SINGLE_CALL(CPP_NAME) return Method<NATIVE>::call(&NATIVE::CPP_NAME,cx,obj,argc,argv,rval);

#define WRAP_METHOD_END }

#define WRAP_METHOD_REGISTER2(NAME,CPP_NAME) \
static bool Register_##CPP_NAME = JSClassTraits<NATIVE>::registerMethod(# NAME,&Call_##CPP_NAME,0);

#define WRAP_METHOD_REGISTER(NAME) WRAP_METHOD_REGISTER2(NAME,NAME)

// wrap and register a single, non-overlaoded method
#define WRAP_AND_REGISTER_METHOD2(NAME,CPP_NAME) \
DOC_END \
WRAP_METHOD_SINGLE_CALL(CPP_NAME) \
WRAP_METHOD_END \
WRAP_METHOD_REGISTER2(NAME,CPP_NAME)

#define WRAP_AND_REGISTER_METHOD(NAME) WRAP_AND_REGISTER_METHOD2(NAME,NAME)


#define WRAP_METHOD_OVERLOADED_CALL(RETURN_TYPE,CPP_NAME,ARG_TYPES) \
{ \
typedef RETURN_TYPE (NATIVE::*MyMethod)ARG_TYPES; \
return Method<NATIVE>::call((MyMethod)&NATIVE::CPP_NAME,cx,obj,argc,argv,rval); \
}

// wrap and register a single overlaoded method
#define WRAP_AND_REGISTER_OVERLOADED_METHOD2(NAME,RETURN_TYPE,CPP_NAME,ARG_TYPES) \
DOC_END \
WRAP_METHOD_OVERLOADED_CALL(RETURN_TYPE,CPP_NAME,ARG_TYPES) \
WRAP_METHOD_END \
WRAP_METHOD_REGISTER2(NAME,CPP_NAME)

#define WRAP_AND_REGISTER_OVERLOADED_METHOD(RETURN_TYPE,NAME,ARG_TYPES) \
WRAP_AND_REGISTER_OVERLOADED_METHOD2(NAME,RETURN_TYPE,NAME,ARG_TYPES)


//---------------------- property wrapping

#define WRAP_PROPERTY_REGISTER(NAME,GETTER,SETTER) \
static bool RegisterProperty_##NAME = JSClassTraits<NATIVE>::registerProperty(# NAME,GETTER,SETTER);


#define WRAP_PROPERTY_WITH_GETTER(NAME,GETTER) \
static JSBool getProperty_##NAME(JSContext *cx, JSObject *obj, jsval id, jsval *vp) { \
    const NATIVE & myNative = JSClassTraits<NATIVE>::getNativeRef(cx, obj); \
    *vp = as_jsval(cx, myNative.GETTER()); \
    return JS_TRUE; \
} \
WRAP_PROPERTY_REGISTER(NAME, getProperty_##NAME, 0)

WRAP_PROPERTY_WITH_GETTER(type,getType);
WRAP_PROPERTY_WITH_GETTER(typename,getTypeString);

#if 1
WRAP_METHOD_BEGIN2(setRow, assignRow, "Set a row of the matrix.");
    DOC_PARAM("theIndex", "index of the row [0-3] to set", DOC_TYPE_INTEGER);
    DOC_PARAM("theVector", "Row vector to set.", DOC_TYPE_VECTOR4F);
WRAP_AND_REGISTER_METHOD2(setRow, assignRow);

WRAP_METHOD_BEGIN(getRow, "Get a row from the matrix.");
    DOC_PARAM("theIndex", "index of the row to get", DOC_TYPE_INTEGER);
    DOC_RVAL("Row vector", DOC_TYPE_VECTOR4F);
WRAP_AND_REGISTER_METHOD(getRow);

WRAP_METHOD_BEGIN2(setColumn, assignColumn, "Set a column of the matrix.");
    DOC_PARAM("theIndex", "index of the column to set", DOC_TYPE_INTEGER);
    DOC_PARAM("theVector", "Column vector to set.", DOC_TYPE_VECTOR4F);
WRAP_AND_REGISTER_METHOD2(setColumn, assignColumn);

WRAP_METHOD_BEGIN(getColumn, "Get a column from the matrix.");
    DOC_PARAM("theIndex", "index of the column to get", DOC_TYPE_INTEGER);
    DOC_RVAL("Column vector", DOC_TYPE_VECTOR4F);
WRAP_AND_REGISTER_METHOD(getColumn);

WRAP_METHOD_BEGIN(makeIdentity, "Make this matrix an identity matrix.");
WRAP_AND_REGISTER_METHOD(makeIdentity);

WRAP_METHOD_BEGIN(makeXRotating, "Make this matrix a X-axis rotation matrix.");
    DOC_PARAM("theAngle", "Angle around the X-axis", DOC_TYPE_FLOAT);
WRAP_AND_REGISTER_METHOD(makeXRotating);

WRAP_METHOD_BEGIN(makeYRotating, "Make this matrix a Y-axis rotation matrix.");
    DOC_PARAM("theAngle", "Angle around the Y-axis", DOC_TYPE_FLOAT);
WRAP_AND_REGISTER_METHOD(makeYRotating);

WRAP_METHOD_BEGIN(makeZRotating, "Make this matrix a Z-axis rotation matrix.");
    DOC_PARAM("theAngle", "Angle around the Z-axis", DOC_TYPE_FLOAT);
WRAP_AND_REGISTER_METHOD(makeZRotating);

WRAP_METHOD_BEGIN(makeXYZRotating, "Make this matrix a XYZ rotation matrix.");
    DOC_PARAM("theAngles", "Angles of rotation around XYZ axis", DOC_TYPE_VECTOR3F);
WRAP_AND_REGISTER_METHOD(makeXYZRotating);

WRAP_METHOD_BEGIN(makeRotating, "Make this matrix a general rotation matrix.");
    DOC_PARAM("theAxis", "Axis to rotate around", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theAngle", "Angle around the axis", DOC_TYPE_FLOAT);
WRAP_AND_REGISTER_METHOD(makeRotating);


WRAP_METHOD_BEGIN(makeScaling, "Make this matrix a scaling matrix.");
    DOC_PARAM("theScale", "Scaling factors for the XYZ axis", DOC_TYPE_VECTOR3F);
WRAP_AND_REGISTER_METHOD(makeScaling);

WRAP_METHOD_BEGIN(makeTranslating, "Make this matrix a translation matrix.");
    DOC_PARAM("theTranslation", "Translation values for the XYZ axis.", DOC_TYPE_VECTOR3F);
WRAP_AND_REGISTER_METHOD(makeTranslating);


WRAP_METHOD_BEGIN(makeLookAt, "Make this matrix a transform which looks from eye to center.");
    DOC_PARAM("theEye", "Eye Position", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theCenter", "Center Position", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theUp", "Up Vector", DOC_TYPE_VECTOR3F);
WRAP_AND_REGISTER_METHOD(makeLookAt);

WRAP_METHOD_BEGIN(makePerspective, "Make this matrix a projection matrix.");
    DOC_PARAM("theFovy", "Field of View", DOC_TYPE_FLOAT);
    DOC_PARAM("theAspect", "Aspect ratio", DOC_TYPE_FLOAT);
    DOC_PARAM("theZNear", "Near Plane Distance", DOC_TYPE_FLOAT);
    DOC_PARAM("theZFar", "Far Plane Distance", DOC_TYPE_FLOAT);
WRAP_AND_REGISTER_METHOD(makePerspective);

WRAP_METHOD_BEGIN(rotateX, "Rotate this matrix around the X axis.");
    DOC_PARAM("theAngle", "Angle around the X-axis", DOC_TYPE_FLOAT);
WRAP_AND_REGISTER_OVERLOADED_METHOD(void, rotateX, (Number) );

WRAP_METHOD_BEGIN(rotateY, "Rotate this matrix around the Y axis.");
    DOC_PARAM("theAngle", "Angle around the Y-axis", DOC_TYPE_FLOAT);
WRAP_AND_REGISTER_OVERLOADED_METHOD(void, rotateY, (Number) );

WRAP_METHOD_BEGIN(rotateZ, "Rotate this matrix around the Z axis.");
    DOC_PARAM("theAngle", "Angle around the Z-axis", DOC_TYPE_FLOAT);
WRAP_AND_REGISTER_OVERLOADED_METHOD(void, rotateZ, (Number) );

WRAP_METHOD_BEGIN(rotateXYZ, "Rotate this matrix around the XYZ axis.");
    DOC_PARAM("theAngles", "Angles of rotation around XYZ axis", DOC_TYPE_VECTOR3F);
WRAP_AND_REGISTER_METHOD(rotateXYZ);

WRAP_METHOD_BEGIN(rotate, "Rotate this matrix.");
    DOC_PARAM("theAxis", "Axis to rotate around", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theAngle", "Angle around the axis", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theRotation", "Quaternion representation of the Euler angle", DOC_TYPE_QUATERNIONF);
    DOC_END;
    try {
        ensureParamCount(argc, 1, 2);
        JSClassTraits<NATIVE>::ScopedNativeRef myObjRef(cx, obj);

        if (argc == 2) {
            asl::Vector3<Number> myAxis;
            if (!convertFrom(cx, argv[0], myAxis)) {
                JS_ReportError(cx, "rotate(): argument must be a Vector3f (rotation axis)");
                return JS_FALSE;
            }
            Number myAngle;
            if (!convertFrom(cx, argv[1], myAngle)) {
                JS_ReportError(cx, "rotate(): argument must be a float (angle)");
                return JS_FALSE;
            }
            myObjRef.getNative().rotate(myAxis, myAngle);
        } else if (argc ==1) {
            asl::Quaternion<Number> myRotation;
            if (!convertFrom(cx, argv[0], myRotation)) {
                JS_ReportError(cx, "rotate(): argument must be a quaternion (rotation)");
                return JS_FALSE;
            }
            myObjRef.getNative().rotate(myRotation);
        } else {
            throw asl::Exception(string("Not enough arguments, must be 2 (rotation axis, angle) or 1(rotation)."));
        }
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}
WRAP_METHOD_REGISTER(rotate);

WRAP_METHOD_BEGIN(scale, "Scale this matrix.");
    DOC_PARAM("theScale", "Scaling factors for the XYZ axis", DOC_TYPE_VECTOR3F);
 WRAP_AND_REGISTER_METHOD(scale);

WRAP_METHOD_BEGIN(translate, "Translate this matrix.");
    DOC_PARAM("theTranslation", "Translation values for the XYZ axis.", DOC_TYPE_VECTOR3F);
WRAP_AND_REGISTER_METHOD(translate);

WRAP_METHOD_BEGIN(postMultiply, "Multiply this matrix by another matrix.");
    DOC_PARAM("theMatrix", "", DOC_TYPE_MATRIX4F);
WRAP_AND_REGISTER_METHOD(postMultiply);

WRAP_METHOD_BEGIN(invert, "Invert this matrix.");
WRAP_AND_REGISTER_METHOD(invert);

WRAP_METHOD_BEGIN(getTranslation, "Get translation part of this matrix.");
    DOC_RVAL("Translation vector", DOC_TYPE_VECTOR3F);
WRAP_AND_REGISTER_METHOD(getTranslation);

WRAP_METHOD_BEGIN(getScale, "Get scaling part of this matrix.");
    DOC_RVAL("Scale vector", DOC_TYPE_VECTOR3F);
WRAP_AND_REGISTER_METHOD(getScale);


WRAP_METHOD_BEGIN(getRotation, "Get orientation part of this matrix.");
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
WRAP_METHOD_REGISTER(getRotation);

WRAP_METHOD_BEGIN(getRotationQuaternion, "Get orientation part of this matrix.");
    DOC_RVAL("orientation quaternion", DOC_TYPE_VECTOR3F);
    DOC_END;
    asl::Matrix4f myObj;
    if (convertFrom(cx, OBJECT_TO_JSVAL(obj), myObj)) {
        asl::Quaternion<float> myRotation;
        myObj.getRotation(myRotation);
        *rval = as_jsval(cx, myRotation);
        return JS_TRUE;
    }
    return JS_FALSE;
}
WRAP_METHOD_REGISTER(getRotationQuaternion);


WRAP_METHOD_BEGIN(decompose, "Decompose matrix into it's components.");
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
WRAP_METHOD_REGISTER(decompose);

WRAP_METHOD_BEGIN(decomposeEuler, "Decompose matrix into it's components.");
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
        asl::Vector3<float> myOrientation;
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
WRAP_METHOD_REGISTER(decomposeEuler);

WRAP_METHOD_BEGIN(toString, "Returns string representation for this matrix.");
    DOC_END;
    std::string myStringRep = asl::as_string(JSMatrix::getJSWrapper(cx,obj).getNative());
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}
WRAP_METHOD_REGISTER(toString);

#else
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
    DOC_BEGIN("Rotate this matrix.");
    DOC_PARAM("theAxis", "Axis to rotate around", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theAngle", "Angle around the axis", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("theRotation", "Quaternion representation of the Euler angle", DOC_TYPE_QUATERNIONF);
    DOC_END;
    try {
        ensureParamCount(argc, 1, 2);
        JSClassTraits<NATIVE>::ScopedNativeRef myObjRef(cx, obj);

        if (argc == 2) {
            asl::Vector3<Number> myAxis;
            if (!convertFrom(cx, argv[0], myAxis)) {
                JS_ReportError(cx, "rotate(): argument must be a Vector3f (rotation axis)");
                return JS_FALSE;
            }
            Number myAngle;
            if (!convertFrom(cx, argv[1], myAngle)) {
                JS_ReportError(cx, "rotate(): argument must be a float (angle)");
                return JS_FALSE;
            }
            myObjRef.getNative().rotate(myAxis, myAngle);
        } else if (argc ==1) {
            asl::Quaternion<Number> myRotation;
            if (!convertFrom(cx, argv[0], myRotation)) {
                JS_ReportError(cx, "rotate(): argument must be a quaternion (rotation)");
                return JS_FALSE;
            }
            myObjRef.getNative().rotate(myRotation);
        } else {
            throw asl::Exception(string("Not enough arguments, must be 2 (rotation axis, angle) or 1(rotation)."));
        }
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
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
getRotationQuaternion(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get orientation part of this matrix.");
    DOC_RVAL("orientation quaternion", DOC_TYPE_QUATERNIONF);
    DOC_END;
    asl::Matrix4f myObj;
    if (convertFrom(cx, OBJECT_TO_JSVAL(obj), myObj)) {
        asl::Quaternion<float> myRotation;
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
decomposeEuler(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
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
        asl::Vector3<float> myOrientation;
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

#endif

JSMatrix::~JSMatrix() {
    DB(AC_TRACE << "JSMatrix DTOR " << this);
}
#if 0
JSFunctionSpec *
JSMatrix::Functions() {
    AC_DEBUG << "Registering class '" << ClassName() << "'";
    JSFunctionSpec * myFunctions = Base::getMethods();
#if 0
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
        {"makeScaling",        makeScaling,             1},
        {"makeTranslating",    makeTranslating,         1},
        {"makeLookAt",         makeLookAt,              3},
        {"makePerspective",    makePerspective,         4},
        {"rotateX",            rotateX,                 1},
        {"rotateY",            rotateY,                 1},
        {"rotateZ",            rotateZ,                 1},
        {"rotateXYZ",          rotateXYZ,               1},
        {"rotate",             rotate,                  2},
        //{"getRotationAxisAngle",      getRotationAxisAngle,           1},
        {"scale",              scale,                   1},
        {"translate",          translate,               1},
        {"postMultiply",       postMultiply,            1},
        {"invert",             invert,                  0},
        {"getTranslation",     getTranslation,          0},
        {"getRotation",        getRotation,             0},
        {"getScale",           getScale,                0},
        {"decompose",          decompose,               0},
        {"decomposeEuler",     decomposeEuler,          0},
        {"toString",           toString,                0},
        {0}
    };
#endif
    return myFunctions;
}
#endif

#if 0
JSPropertySpec *
JSMatrix::Properties() {
    static JSPropertySpec myProperties[] = {
        {"type", PROP_type, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {"typename", PROP_typename, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {0}
    };
    return myProperties;
}
#endif


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
        }
        myNewMatrix.assign(float(myArgs[0]), float(myArgs[1]),
                           float(myArgs[2]), float(myArgs[3]),
                           float(myArgs[4]), float(myArgs[5]),
                           float(myArgs[6]), float(myArgs[7]),
                           float(myArgs[8]), float(myArgs[9]),
                           float(myArgs[10]),float(myArgs[11]),
                           float(myArgs[12]),float(myArgs[13]),
                           float(myArgs[14]),float(myArgs[15]));

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
            } catch (asl::Exception & /*ex*/) {
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
