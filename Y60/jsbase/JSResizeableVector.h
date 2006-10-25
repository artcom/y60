//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSRESIZEABLEVECTOR_H_INCLUDED_
#define _Y60_ACXPSHELL_JSRESIZEABLEVECTOR_H_INCLUDED_

#include "JSWrapper.h"

#include <dom/Value.h>

namespace jslib {

class JSResizeableVector
    : public JSWrapper<dom::ResizeableVector, dom::ValuePtr, VectorValueAccessProtocol>
{
public:
    typedef dom::ResizeableVector NATIVE_VECTOR;
    typedef JSWrapper<NATIVE_VECTOR, dom::ValuePtr, VectorValueAccessProtocol> Base;

    static const char * ClassName() {
        return "ResizeableVector";
    }
    static JSFunctionSpec * Functions();
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();


    virtual unsigned long length() const {
        return getNative().length();
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
    JSObject * Construct(JSContext *cx, dom::ValuePtr theValuePtr) {
        return Base::Construct(cx, theValuePtr, 0);
    }
    JSResizeableVector(dom::ValuePtr theValuePtr)
        : Base(theValuePtr, 0)
    {}
    JSResizeableVector()
        : Base(NativeValuePtr(0), 0)
    {}
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
        DOC_CREATE(JSResizeableVector);
        return myClass;
    }
};

template <>
struct JSClassTraits<dom::ResizeableVector>
    : public JSClassTraitsWrapper<dom::ResizeableVector, JSResizeableVector> {};

jsval as_jsval(JSContext *cx, dom::ValuePtr theValuePtr, dom::ResizeableVector * theVectorPtr);

}

#endif
