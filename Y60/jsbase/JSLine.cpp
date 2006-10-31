//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSLine.h"
#include "JSLine.impl"
#include "JSVector.h"
#include "JSWrapper.impl"
#include <iostream>

using namespace std;

namespace jslib {

template<>
const char *
JSLine::ClassName() {
    return "Line";
}

template<>
const char *
JSRay::ClassName() {
    return "Ray";
}

template<>
const char *
JSLineSegment::ClassName() {
    return "LineSegment";
}

template class JSWrapper<asl::Line<LineNumber> >;
template class JSWrapper<asl::Ray<LineNumber> >;
template class JSWrapper<asl::LineSegment<LineNumber> >;

template class JSGenericLine<asl::Line<LineNumber> >;
template class JSGenericLine<asl::Ray<LineNumber> >;
template class JSGenericLine<asl::LineSegment<LineNumber> >;

template<>
JSPropertySpec *
JSLineSegment::Properties() {
    static JSPropertySpec myProperties[] = {
        {"origin", PROP_origin, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // readwrite attribute Point3f
        {"end", PROP_end, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},     // readwrite attribute Vector3f
        {0}
    };
    return myProperties;
}


// getproperty handling
template<>
JSBool
JSLineSegment::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_origin:
                *vp = as_jsval(cx, getNative().origin);
                return JS_TRUE;
            case PROP_end:
                *vp = as_jsval(cx, getNative().end);
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSLine::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
};

// setproperty handling
template<>
JSBool
JSLineSegment::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    typedef asl::LineSegment<LineNumber> NATIVE;
    jsval dummy;
    switch (theID) {
        case PROP_origin:
            return Method<NATIVE>::call(&NATIVE::setOrigin, cx, obj, 1, vp, &dummy);
        case PROP_end:
            return Method<NATIVE>::call(&NATIVE::setEnd, cx, obj, 1, vp, &dummy);
        default:
            JS_ReportError(cx,"JSLine::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
};

template <class NATIVE_LINE>
struct JSConverter {
    static
    bool convertFrom(JSContext *cx, jsval theValue, NATIVE_LINE & theLine) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<NATIVE_LINE >::Class()) {
                    theLine = JSClassTraits<NATIVE_LINE >::getNativeRef(cx,myArgument);
                    return true;
                }
            }
        }
        return false;
    }

    static
    jsval as_jsval(JSContext *cx, const NATIVE_LINE  & theValue) {
        JSObject * myReturnObject = JSGenericLine<NATIVE_LINE>::Construct(cx, theValue);
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    static
    jsval as_jsval(JSContext *cx, typename JSGenericLine<NATIVE_LINE>::NativeValuePtr theValue) {
        JSObject * myObject = JSGenericLine<NATIVE_LINE>::Construct(cx, theValue);
        return OBJECT_TO_JSVAL(myObject);
    }
};

bool convertFrom(JSContext *cx, jsval theValue, asl::Line<LineNumber> & theLine) {
    return JSConverter<asl::Line<LineNumber> >::convertFrom(cx, theValue, theLine);
}
jsval as_jsval(JSContext *cx, const asl::Line<LineNumber> & theValue) {
    return JSConverter<asl::Line<LineNumber> >::as_jsval(cx, theValue);
}
jsval as_jsval(JSContext *cx, JSLine::NativeValuePtr theValue) {
    return JSConverter<asl::Line<LineNumber> >::as_jsval(cx, theValue);
}


bool convertFrom(JSContext *cx, jsval theValue, asl::Ray<LineNumber> & theLine) {
    return JSConverter<asl::Ray<LineNumber> >::convertFrom(cx, theValue, theLine);
}
jsval as_jsval(JSContext *cx, const asl::Ray<LineNumber>  & theValue) {
    return JSConverter<asl::Ray<LineNumber> >::as_jsval(cx, theValue);
}
jsval as_jsval(JSContext *cx, JSRay::NativeValuePtr theValue) {
    return JSConverter<asl::Ray<RayNumber> >::as_jsval(cx, theValue);
}


bool convertFrom(JSContext *cx, jsval theValue, asl::LineSegment<LineNumber> & theLine) {
    return JSConverter<asl::LineSegment<LineNumber> >::convertFrom(cx, theValue, theLine);
}
jsval as_jsval(JSContext *cx, const asl::LineSegment<LineNumber> & theValue){
    return JSConverter<asl::LineSegment<LineNumber> >::as_jsval(cx, theValue);
}
jsval as_jsval(JSContext *cx, JSLineSegment::NativeValuePtr theValue) {
    return JSConverter<asl::LineSegment<LineSegmentNumber> >::as_jsval(cx, theValue);
}

}
