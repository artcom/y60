//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSLine_H_INCLUDED_
#define _Y60_ACXPSHELL_JSLine_H_INCLUDED_

#include "JSWrapper.h"
#include "JSVector.h"

#include <asl/math/Vector234.h>
#include <asl/math/numeric_functions.h>
#include <asl/math/linearAlgebra.h>
#include <asl/math/Line.h>
#include <asl/dom/Value.h>


namespace jslib {

// TODO: These typedefs are used inconsistently and all evaluate to float. That doesn't 
// make sense.
typedef float LineNumber;
typedef float RayNumber;
typedef float LineSegmentNumber;

template <class NATIVE_LINE>
class JSGenericLine : public JSWrapper<NATIVE_LINE>
{
    typedef JSWrapper<NATIVE_LINE> Base;
    JSGenericLine() {}
public:
    static const char * ClassName();
    static JSFunctionSpec * Functions();

    enum PropertyNumbers {
        PROP_origin = -100,
        PROP_direction,
        PROP_end
    };
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();

    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    //virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    //virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSObject * Construct(JSContext *cx, typename JSGenericLine<NATIVE_LINE>::NativeValuePtr theValue);
    static JSObject * Construct(JSContext *cx, const NATIVE_LINE & theLine);
    JSGenericLine(typename JSGenericLine<NATIVE_LINE>::NativeValuePtr theValue);

    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
};


typedef JSGenericLine<asl::Line<LineNumber> > JSLine;
typedef JSGenericLine<asl::Ray<LineNumber> > JSRay;
typedef JSGenericLine<asl::LineSegment<LineNumber> > JSLineSegment;

bool convertFrom(JSContext *cx, jsval theValue, asl::Line<LineNumber> & theLine);
jsval as_jsval(JSContext *cx, const asl::Line<LineNumber>  & theValue);
//jsval as_jsval(JSContext *cx, dom::ValuePtr theValue, asl::Line<LineNumber> * theLine);
jsval as_jsval(JSContext *cx, JSLine::NativeValuePtr theLine);

bool convertFrom(JSContext *cx, jsval theValue, asl::Ray<LineNumber> & theLine);
jsval as_jsval(JSContext *cx, const asl::Ray<LineNumber>  & theValue);
//jsval as_jsval(JSContext *cx, dom::ValuePtr theValue, asl::Ray<LineNumber> * theLine);
jsval as_jsval(JSContext *cx, JSRay::NativeValuePtr theLine);

bool convertFrom(JSContext *cx, jsval theValue, asl::LineSegment<LineNumber> & theLine);
jsval as_jsval(JSContext *cx, const asl::LineSegment<LineNumber> & theValue);
//jsval as_jsval(JSContext *cx, dom::ValuePtr theValue, asl::LineSegment<LineNumber> * theLine);
jsval as_jsval(JSContext *cx, JSLineSegment::NativeValuePtr theLine);

template <class NATIVE_LINE>
struct JSLineClassTraits : public JSClassTraitsWrapper<NATIVE_LINE, JSGenericLine<NATIVE_LINE> > {};

template <>
struct JSClassTraits<asl::Line<LineNumber> > : public JSLineClassTraits<asl::Line<LineNumber> > {
};

template <>
struct JSClassTraits<asl::Ray<LineNumber> > : public JSLineClassTraits<asl::Ray<LineNumber> > {
};

template <>
struct JSClassTraits<asl::LineSegment<LineNumber> > : public JSLineClassTraits<asl::LineSegment<LineNumber> > {
};

}

#endif

