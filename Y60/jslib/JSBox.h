//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSBox.h,v $
//   $Author: christian $
//   $Revision: 1.5 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSBOX_H_INCLUDED_
#define _Y60_ACXPSHELL_JSBOX_H_INCLUDED_

#include "JSWrapper.h"

#include <asl/Vector234.h>
#include <asl/numeric_functions.h>
#include <asl/linearAlgebra.h>
#include <asl/Box.h>
#include <dom/Value.h>

namespace jslib {

typedef float BoxNumber;

class JSBox3f : public JSWrapper<asl::Box3<BoxNumber> > {
    JSBox3f() {}
public:
    typedef JSWrapper<asl::Box3<BoxNumber> > Base;

    static const char * ClassName() {
        return "Box3f";
    }
    static JSFunctionSpec * Functions();

    enum PropertyNumbers {
        PROP_length = -100,
        PROP_hasPosition,
        PROP_hasSize,
        PROP_hasArea,
        PROP_hasVolume,
        PROP_isEmpty,
        PROP_min,
        PROP_max,
        PROP_center,
        PROP_size,
        PROP_volume,
        PROP_area
    };
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();


    virtual unsigned long length() const {
        return 2;
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
    JSObject * Construct(JSContext *cx, JSBox3f::NativeValuePtr theValue) {
        return Base::Construct(cx, theValue, 0);
    }
    static
    JSObject * Construct(JSContext *cx, const asl::Box3<BoxNumber> & theBox3f) {
        JSBox3f::NativeValuePtr myValue(new dom::ValueWrapper<asl::Box3<BoxNumber> >::Type(theBox3f, 0));
        return Base::Construct(cx, myValue, 0);
    }
    JSBox3f(JSBox3f::NativeValuePtr theValue)
        : Base(theValue, 0)
    {}

    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
};


template <>
struct JSClassTraits<asl::Box3<BoxNumber> >
    : public JSClassTraitsWrapper<asl::Box3<BoxNumber> , JSBox3f> {};

bool convertFrom(JSContext *cx, jsval theValue, asl::Box3<BoxNumber>  & theBox3f);

jsval as_jsval(JSContext *cx, const asl::Box3<BoxNumber>  & theValue);

jsval as_jsval(JSContext *cx, JSBox3f::NativeValuePtr theValue);

}

#endif

