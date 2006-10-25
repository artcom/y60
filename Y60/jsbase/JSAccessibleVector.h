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
//   $RCSfile: JSAccessibleVector.h,v $
//   $Author: martin $
//   $Revision: 1.4 $
//   $Date: 2004/11/07 04:50:19 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSACCESSIBLEVECTOR_H_INCLUDED_
#define _Y60_ACXPSHELL_JSACCESSIBLEVECTOR_H_INCLUDED_

#include "JSWrapper.h"
#include <dom/Value.h>

namespace jslib {

class JSAccessibleVector
    : public JSWrapper<dom::AccessibleVector,
                       dom::ValuePtr,
                       VectorValueAccessProtocol> // TODO: verify if correct, probably is not
{
public:
    typedef dom::AccessibleVector NATIVE_VECTOR;
    typedef JSWrapper<NATIVE_VECTOR, dom::ValuePtr, VectorValueAccessProtocol> Base;

    static const char * ClassName() {
        return "AccessibleVector";
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
    JSObject * Construct(JSContext *cx, NativeValuePtr theVector) {
        return Base::Construct(cx, theVector, 0);
    }
    JSAccessibleVector(NativeValuePtr theVector)
        : Base(theVector, 0)
    {}
    JSAccessibleVector()
        : Base(NativeValuePtr(0), 0)
    {}
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
        DOC_CREATE(JSAccessibleVector);
        return myClass;
    }
};

template <>
struct JSClassTraits<dom::AccessibleVector>
    : public JSClassTraitsWrapper<dom::AccessibleVector, JSAccessibleVector> {};

jsval as_jsval(JSContext *cx, dom::ValuePtr theValuePtr, dom::AccessibleVector * theVectorPtr);

}

#endif
