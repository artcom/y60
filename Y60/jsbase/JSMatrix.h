//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSMATRIX_INCLUDED_
#define _Y60_ACXPSHELL_JSMATRIX_INCLUDED_

#include "JSWrapper.h"

#include <asl/Vector234.h>
#include <asl/numeric_functions.h>
#include <asl/linearAlgebra.h>
#include <asl/Matrix4.h>
#include <dom/Value.h>


namespace jslib {

/*
typedef asl::Ptr<dom::SimpleValue<asl::Matrix4f>, dom::ThreadingModel > JSMatrix::NativeValuePtr;
typedef JSWrapper<
            asl::Matrix4f,
            JSMatrix::NativeValuePtr,
            ValueAccessProtocol<
                asl::Matrix4f,
                JSMatrix::NativeValuePtr
            >
        > Matrix4fWrapper;
*/
class JSMatrix : public JSWrapper<asl::Matrix4f>
{
public:
    typedef JSWrapper<asl::Matrix4f> Base;

    static const char * ClassName() {
        return "Matrix4f";
    }
    static JSFunctionSpec * Functions();

    enum PropertyNumbers {
        PROP_type = -100,
        PROP_typename,
        PROP_IDENTITY,
        PROP_X_ROTATING,
        PROP_Y_ROTATING,
        PROP_Z_ROTATING,
        PROP_ROTATING,
        PROP_SCALING,
        PROP_LINEAR,
        PROP_TRANSLATING,
        PROP_AFFINE,
        PROP_UNKNOWN
    };
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();

    virtual ~JSMatrix();
    virtual unsigned long length() const {
        return 4;
    }
    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static
    JSObject * Construct(JSContext *cx, JSMatrix::NativeValuePtr theValue) {
        return Base::Construct(cx, theValue, 0);
    }
    static
    JSObject * Construct(JSContext *cx, const asl::Matrix4f & theMatrix) {
        JSMatrix::NativeValuePtr myValue(new dom::SimpleValue<asl::Matrix4f>(theMatrix, 0));
        return Base::Construct(cx, myValue, 0);
    }
    JSMatrix(JSMatrix::NativeValuePtr theValue)
        : Base(theValue, 0)
    {}

    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
};


template <>
struct JSClassTraits<asl::Matrix4f> : public JSClassTraitsWrapper<asl::Matrix4f, JSMatrix> {};

bool convertFrom(JSContext *cx, jsval theValue, JSMatrix::NativeValuePtr & theMatrix);
//bool convertFrom(JSContext *cx, jsval theValue, asl::Matrix4f const * & theMatrix);
bool convertFrom(JSContext *cx, jsval theValue, asl::Matrix4f & theMatrix);

jsval as_jsval(JSContext *cx, const asl::Matrix4f & theValue);
jsval as_jsval(JSContext *cx, JSMatrix::NativeValuePtr theValue);

}

#endif

