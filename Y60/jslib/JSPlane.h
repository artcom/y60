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
//   $RCSfile: JSPlane.h,v $
//   $Author: christian $
//   $Revision: 1.5 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSPLANE_INCLUDED_
#define _Y60_ACXPSHELL_JSPLANE_INCLUDED_

#include "JSWrapper.h"

#include <asl/Vector234.h>
#include <asl/numeric_functions.h>
#include <asl/linearAlgebra.h>
#include <asl/Plane.h>
#include <dom/Value.h>

namespace jslib {

typedef float PlaneNumber;

class JSPlane : public JSWrapper<asl::Plane<PlaneNumber> > {
    JSPlane() {}
public:
    typedef JSWrapper<asl::Plane<PlaneNumber> > Base;

    static const char * ClassName() {
        return "Plane";
    }
    static JSFunctionSpec * Functions();

    enum PropertyNumbers {
        PROP_normal = -100,
        PROP_offset
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
    JSObject * Construct(JSContext *cx, JSPlane::NativeValuePtr theValue) {
        return Base::Construct(cx, theValue, 0);
    }
    static
    JSObject * Construct(JSContext *cx, const asl::Plane<PlaneNumber> & thePlane) {
        JSPlane::NativeValuePtr myValue(new dom::SimpleValue<asl::Plane<PlaneNumber> >(thePlane, 0));
        return Base::Construct(cx, myValue, 0);
    }
    JSPlane(JSPlane::NativeValuePtr theValue)
        : Base(theValue, 0)
    {}

    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
};


template <>
struct JSClassTraits<asl::Plane<PlaneNumber> >
    : public JSClassTraitsWrapper<asl::Plane<PlaneNumber> , JSPlane> {};

bool convertFrom(JSContext *cx, jsval theValue, asl::Plane<PlaneNumber>  & thePlane);

jsval as_jsval(JSContext *cx, const asl::Plane<PlaneNumber>  & theValue);

jsval as_jsval(JSContext *cx, dom::ValuePtr theValue, asl::Plane<PlaneNumber>  * thePlane);

}

#endif

