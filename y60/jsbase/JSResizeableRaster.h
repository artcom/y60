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

#ifndef _Y60_ACXPSHELL_JSRESIZEABLERASTER_H_INCLUDED_
#define _Y60_ACXPSHELL_JSRESIZEABLERASTER_H_INCLUDED_

#include "y60_jsbase_settings.h"

#include "JSWrapper.h"
#include <asl/dom/Value.h>

namespace jslib {

class Y60_JSBASE_DECL JSResizeableRaster
    : public JSWrapper<dom::ResizeableRaster,
                       dom::ValuePtr,
                       VectorValueAccessProtocol> // TODO: verify if correct, probably is not
{
public:
    typedef dom::ResizeableRaster NATIVE_VECTOR;
    typedef JSWrapper<NATIVE_VECTOR, dom::ValuePtr, VectorValueAccessProtocol> Base;

    static const char * ClassName() {
        return "ResizeableRaster";
    }

    static JSFunctionSpec * Functions();
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();


    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    //virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    //virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static
    JSObject * Construct(JSContext *cx, NativeValuePtr theVector) {
        return Base::Construct(cx, theVector, 0);
    }
    JSResizeableRaster(NativeValuePtr theVector)
        : Base(theVector, 0)
    {}
    JSResizeableRaster()
        : Base(NativeValuePtr(), 0)
    {}
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
        DOC_CREATE(JSResizeableRaster);
        return myClass;
    }
};

template <>
struct JSClassTraits<dom::ResizeableRaster>
    : public JSClassTraitsWrapper<dom::ResizeableRaster, JSResizeableRaster> {};

Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, dom::ValuePtr theValuePtr, dom::ResizeableRaster * theVectorPtr);

}

#endif
