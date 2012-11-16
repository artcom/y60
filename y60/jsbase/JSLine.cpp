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

// own header
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
