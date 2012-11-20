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

#ifndef _Y60_ACXPSHELL_JSMATRIX_INCLUDED_
#define _Y60_ACXPSHELL_JSMATRIX_INCLUDED_

#include "y60_jsbase_settings.h"

#include "JSVector.h"

#include "JSWrapper.h"

#include <asl/math/Vector234.h>
#include <asl/math/numeric_functions.h>
#include <asl/math/linearAlgebra.h>
#include <asl/math/Matrix4.h>
#include <asl/dom/Value.h>


namespace jslib {

class Y60_JSBASE_DECL JSMatrix : public JSWrapper<asl::Matrix4f>
{
public:
    typedef JSWrapper<asl::Matrix4f> Base;

    static const char * ClassName() {
        return "Matrix4f";
    }
    // static JSFunctionSpec * Functions();

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
    //static JSPropertySpec * Properties();
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

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, JSMatrix::NativeValuePtr & theMatrix);
//bool convertFrom(JSContext *cx, jsval theValue, asl::Matrix4f const * & theMatrix);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Matrix4f & theMatrix);

Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const asl::Matrix4f & theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, JSMatrix::NativeValuePtr theValue);

}

#endif

