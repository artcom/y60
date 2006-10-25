//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSTRIANGLE_INCLUDED_
#define _Y60_ACXPSHELL_JSTRIANGLE_INCLUDED_

#include "JSWrapper.h"

#include <asl/Vector234.h>
#include <asl/numeric_functions.h>
#include <asl/linearAlgebra.h>
#include <asl/Triangle.h>
#include <dom/Value.h>


namespace jslib {

typedef float TriangleNumber;

class JSTriangle : public JSWrapper<asl::Triangle<TriangleNumber> > {
    JSTriangle() {}
public:
    typedef JSWrapper<asl::Triangle<TriangleNumber> > Base;

    static const char * ClassName() {
        return "Triangle";
    }
    static JSFunctionSpec * Functions();

    enum PropertyNumbers {
        PROP_normal = -100,
        PROP_plane,
        PROP_length
    };
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();


    virtual unsigned long length() const {
        return 3;
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
    JSObject * Construct(JSContext *cx, JSTriangle::NativeValuePtr theValue) {
        return Base::Construct(cx, theValue, 0);
    }
    static
    JSObject * Construct(JSContext *cx, const asl::Triangle<TriangleNumber> & theTriangle) {
        JSTriangle::NativeValuePtr myValue(new dom::ValueWrapper<asl::Triangle<TriangleNumber> >::Type(theTriangle, 0));
        return Base::Construct(cx, myValue, 0);
    }
    JSTriangle(JSTriangle::NativeValuePtr theValue)
        : Base(theValue, 0)
    {}
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
};

template <>
struct JSClassTraits<asl::Triangle<TriangleNumber> >
    : public JSClassTraitsWrapper<asl::Triangle<TriangleNumber> , JSTriangle> {};

bool convertFrom(JSContext *cx, jsval theValue, asl::Triangle<TriangleNumber>  & theTriangle);

jsval as_jsval(JSContext *cx, const asl::Triangle<TriangleNumber>  & theValue);

jsval as_jsval(JSContext *cx, JSTriangle::NativeValuePtr theValue);

}

#endif

