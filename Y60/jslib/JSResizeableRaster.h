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
//   $RCSfile: JSResizeableRaster.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2004/11/20 16:34:20 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSRESIZEABLERASTER_H_INCLUDED_
#define _Y60_ACXPSHELL_JSRESIZEABLERASTER_H_INCLUDED_

#include "JSWrapper.h"
#include <dom/Value.h>

namespace jslib {

class JSResizeableRaster
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
        : Base(NativeValuePtr(0), 0)
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

jsval as_jsval(JSContext *cx, dom::ValuePtr theValuePtr, dom::ResizeableRaster * theVectorPtr);

}

#endif
