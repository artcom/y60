//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSFRUSTUM_INCLUDED_
#define _Y60_ACXPSHELL_JSFRUSTUM_INCLUDED_

#include "JSWrapper.h"
#include "JSEnum.h"

#include <asl/Vector234.h>
#include <asl/numeric_functions.h>
#include <asl/linearAlgebra.h>
#include <asl/Frustum.h>
#include <dom/Value.h>
#include <y60/FrustumValue.h>

namespace jslib {

class JSFrustum : public JSWrapper<asl::Frustum > {
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

bool convertFrom(JSContext *cx, jsval theValue, JSFrustum::NativeValuePtr & theFrustum);
bool convertFrom(JSContext *cx, jsval theValue, asl::Frustum  & theFrustum);

jsval as_jsval(JSContext *cx, const asl::Frustum  & theValue);
jsval as_jsval(JSContext *cx, JSFrustum::NativeValuePtr theValue);

}

#endif


