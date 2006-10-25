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
//   $RCSfile: JSAccessibleVector.cpp,v $
//   $Author: pavel $
//   $Revision: 1.4 $
//   $Date: 2005/04/24 00:41:19 $
//
//
//=============================================================================

#include "JSAccessibleVector.h"
#include "JSNode.h"
#include "JSWrapper.impl"

#include <iostream>

using namespace std;

namespace jslib {

typedef dom::AccessibleVector NATIVE_VECTOR;
typedef JSWrapper<NATIVE_VECTOR,dom::ValuePtr> Base;

template class JSWrapper<NATIVE_VECTOR, dom::ValuePtr, VectorValueAccessProtocol>;

static JSBool
item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns one item of the vector");
    DOC_PARAM("theIndex", "The index of the item to get", DOC_TYPE_INTEGER);
    DOC_RVAL("The item", DOC_TYPE_OBJECT);
    DOC_END;
    typedef dom::ValuePtr (NATIVE_VECTOR::*MyMethod)(int);
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::getElement,cx,obj,argc,argv,rval);
}
JSFunctionSpec *
JSAccessibleVector::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name         native          nargs    */
        {"item",             item,            1},
        {0}
    };
    return myFunctions;
}
enum PropertyNumbers {PROP_length = -100};

JSPropertySpec *
JSAccessibleVector::Properties() {
    static JSPropertySpec myProperties[] = {
        {"length", PROP_length, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSAccessibleVector::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSAccessibleVector::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSAccessibleVector::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}


// getproperty handling
JSBool
JSAccessibleVector::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_length:
                *vp = as_jsval(cx, getNative().length());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSAccessibleVector::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}

JSBool JSAccessibleVector::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    *vp = as_jsval(cx, getNative().getElement(theIndex));
    return JS_TRUE;
}

// setproperty handling
JSBool JSAccessibleVector::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JS_ReportError(cx,"JSAccessibleVector::setPropertySwitch: index %d out of range", theID);
    return JS_FALSE;
}

JSBool JSAccessibleVector::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    dom::ValuePtr myArg;
    if (convertFrom(cx, *vp, getNative().elementName(), myArg)) {
        //IF_NOISY_Y(AC_TRACE << "JSAccessibleVector::setPropertyIndex theIndex =" << theIndex << " myArg: " << (void*)myArg.getNativePtr() << endl);
        openNative().setElement(theIndex,*myArg);
        closeNative();
        return JS_TRUE;
    }
    return JS_TRUE;
}

JSBool
JSAccessibleVector::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs a new accessible vecotr");
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSAccessibleVector * myNewObject = 0;
    if (argc == 0) {
        myNewObject=new JSAccessibleVector();
    } else {
        JS_ReportError(cx,"Constructor for %s: superflous argument ignored",ClassName());
        myNewObject=new JSAccessibleVector();
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSAccessibleVector::Constructor: bad parameters");
    return JS_FALSE;
}


jsval as_jsval(JSContext *cx, dom::ValuePtr theValuePtr, dom::AccessibleVector *) {
    JSObject * myObject = JSAccessibleVector::Construct(cx, theValuePtr);
    return OBJECT_TO_JSVAL(myObject);
}

}
