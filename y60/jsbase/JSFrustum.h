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

#ifndef _Y60_ACXPSHELL_JSFRUSTUM_INCLUDED_
#define _Y60_ACXPSHELL_JSFRUSTUM_INCLUDED_

#include "y60_jsbase_settings.h"

#include "JSWrapper.h"
#include "JSEnum.h"

#include <asl/math/Vector234.h>
#include <asl/math/numeric_functions.h>
#include <asl/math/linearAlgebra.h>
#include <asl/math/Frustum.h>
#include <asl/dom/Value.h>
#include <y60/base/FrustumValue.h>

namespace jslib {

class Y60_JSBASE_DECL JSFrustum : public JSWrapper<asl::Frustum > {
    JSFrustum() {}
public:
    typedef JSWrapper<asl::Frustum > Base;

    static const char * ClassName() {
        return "Frustum";
    }
    static JSFunctionSpec * Functions();

    enum PropertyNumbers {
        PROP_left = -100,
        PROP_right,
        PROP_top,
        PROP_bottom,
        PROP_near,
        PROP_far,

        PROP_type,
        PROP_width,
        PROP_height,
        PROP_hshift,
        PROP_vshift,
        PROP_hfov,
        PROP_vfov,

        PROP_left_plane,
        PROP_right_plane,
        PROP_top_plane,
        PROP_bottom_plane,
        PROP_near_plane,
        PROP_far_plane,

        PROP_projectionmatrix,

        PROP_PERSPECTIVE,
        PROP_ORTHONORMAL,
        PROP_END
    };
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();


    virtual unsigned long length() const {
        return 1;
    }
    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    //virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    //virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static
    JSObject * Construct(JSContext *cx, JSFrustum::NativeValuePtr theValue) {
        return Base::Construct(cx, theValue, 0);
    }
    static
    JSObject * Construct(JSContext *cx, const asl::Frustum & theFrustum) {
        JSFrustum::NativeValuePtr myValue(new dom::ValueWrapper<asl::Frustum >::Type(theFrustum, 0));
        return Base::Construct(cx, myValue, 0);
    }
    JSFrustum(JSFrustum::NativeValuePtr theValue)
        : Base(theValue, 0)
    {}


    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
};


template <>
struct JSClassTraits<asl::Frustum >
    : public JSClassTraitsWrapper<asl::Frustum , JSFrustum> {};

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, JSFrustum::NativeValuePtr & theFrustum);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Frustum  & theFrustum);

Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const asl::Frustum  & theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, JSFrustum::NativeValuePtr theValue);

}

#endif


