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

#ifndef _Y60_ACXPSHELL_JSLine_H_INCLUDED_
#define _Y60_ACXPSHELL_JSLine_H_INCLUDED_

#include "y60_jsbase_settings.h"

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
class Y60_JSBASE_DECL JSGenericLine : public JSWrapper<NATIVE_LINE>
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

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Line<LineNumber> & theLine);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const asl::Line<LineNumber>  & theValue);
//jsval as_jsval(JSContext *cx, dom::ValuePtr theValue, asl::Line<LineNumber> * theLine);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, JSLine::NativeValuePtr theLine);

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Ray<LineNumber> & theLine);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const asl::Ray<LineNumber>  & theValue);
//jsval as_jsval(JSContext *cx, dom::ValuePtr theValue, asl::Ray<LineNumber> * theLine);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, JSRay::NativeValuePtr theLine);

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::LineSegment<LineNumber> & theLine);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const asl::LineSegment<LineNumber> & theValue);
//jsval as_jsval(JSContext *cx, dom::ValuePtr theValue, asl::LineSegment<LineNumber> * theLine);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, JSLineSegment::NativeValuePtr theLine);

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

