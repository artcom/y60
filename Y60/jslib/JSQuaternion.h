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
//   $RCSfile: JSQuaternion.h,v $
//   $Author: ulrich $
//   $Revision: 1.7 $
//   $Date: 2005/03/29 10:57:49 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSQUATERNION_INCLUDED_
#define _Y60_ACXPSHELL_JSQUATERNION_INCLUDED_

#include "JSWrapper.h"

#include <asl/Vector234.h>
#include <asl/numeric_functions.h>
#include <asl/linearAlgebra.h>
#include <asl/Quaternion.h>
#include <dom/Value.h>


namespace jslib {

typedef float QuaternionNumber;

class JSQuaternion : public JSWrapper<asl::Quaternion<QuaternionNumber> > {
    JSQuaternion() {}
public:
    typedef JSWrapper<asl::Quaternion<QuaternionNumber> > Base;

    static const char * ClassName() {
        return "Quaternionf";
    }
    static JSFunctionSpec * Functions();
    static JSFunctionSpec * StaticFunctions();

    enum PropertyNumbers {
        PROP_imag = -100,
        PROP_real,
        PROP_length
    };

    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
 
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
    JSObject * Construct(JSContext *cx, JSQuaternion::NativeValuePtr theValue) {
        return Base::Construct(cx, theValue, 0);
    }
    static
    JSObject * Construct(JSContext *cx, const asl::Quaternion<QuaternionNumber> & theQuaternion) {
        JSQuaternion::NativeValuePtr myValue(new dom::ValueWrapper<asl::Quaternion<QuaternionNumber> >::Type(theQuaternion, 0));
        return Base::Construct(cx, myValue, 0);
    }
    JSQuaternion(JSQuaternion::NativeValuePtr theValue)
        : Base(theValue, 0)
    {}

    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
};

template <>
struct JSClassTraits<asl::Quaternion<QuaternionNumber> >
    : public JSClassTraitsWrapper<asl::Quaternion<QuaternionNumber> , JSQuaternion> {};

bool convertFrom(JSContext *cx, jsval theValue, JSQuaternion::NativeValuePtr & theQuaternion);
bool convertFrom(JSContext *cx, jsval theValue, asl::Quaternion<QuaternionNumber>  & theQuaternion);

jsval as_jsval(JSContext *cx, const asl::Quaternion<QuaternionNumber> & theValue);
jsval as_jsval(JSContext *cx, JSQuaternion::NativeValuePtr theValue);

}

#endif

