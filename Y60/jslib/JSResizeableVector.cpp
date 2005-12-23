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
//   $RCSfile: JSResizeableVector.cpp,v $
//   $Author: pavel $
//   $Revision: 1.5 $
//   $Date: 2005/04/24 00:41:19 $
//
//
//=============================================================================

#include "JSResizeableVector.h"
#include "JSNode.h"

#include <asl/string_functions.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

typedef dom::ResizeableVector NATIVE_VECTOR;
//typedef JSWrapper<NATIVE_VECTOR,dom::ValuePtr> Base;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a string representation of the vector.");
    DOC_END;
    std::string myStringRep = asl::as_string(JSResizeableVector::getJSWrapper(cx,obj).getNative());
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns an element from the vector. Throws an exception, if index is out of bounds.");
    DOC_PARAM("theIndex", "Index of the element to retrieve.", DOC_TYPE_INTEGER);
    DOC_RVAL("theElement", DOC_TYPE_OBJECT);
    DOC_END;
    typedef dom::ValuePtr (NATIVE_VECTOR::*MyMethod)(int);
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::getElement,cx,obj,argc,argv,rval);
}
static JSBool
resize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Resizes the vector to the requested size, adding default elements or removing superflous elements.");
    DOC_PARAM("theSize", "The new size of the vector", DOC_TYPE_INTEGER);
    DOC_END;
    typedef void (NATIVE_VECTOR::*MyMethod)(int);
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::resize,cx,obj,argc,argv,rval);
}

/* TODO
static JSBool
append(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("todo");
    DOC_END;
    typedef bool (NATIVE_VECTOR::*MyMethod)(const dom::ValueBase &);
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::append,cx,obj,argc,argv,rval);
}
*/
static JSBool
erase(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Erases one element from the vector");
    DOC_PARAM("theIndex", "The index of the element to erase", DOC_TYPE_INTEGER);
    DOC_RVAL("true, if successfull", DOC_TYPE_BOOLEAN);
    DOC_END;
    typedef bool (NATIVE_VECTOR::*MyMethod)(int);
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::erase,cx,obj,argc,argv,rval);
}

/* TODO
static JSBool
insertBefore(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("todo");
    DOC_END;
    typedef bool (NATIVE_VECTOR::*MyMethod)(int, const ValueBase &);
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::insertBefore,cx,obj,argc,argv,rval);
}
*/

JSFunctionSpec *
JSResizeableVector::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name         native          nargs    */
        {"toString",         toString,        0},
        {"item",             item,            1},
        {"resize",           resize,          1},
// TODO   {"append",           append,          1},
        {"erase",            erase,           1},
// TODO   {"insertBefore",     insertBefore,    2},
        {0}
    };
    return myFunctions;
}
enum PropertyNumbers {PROP_length = -100};

JSPropertySpec *
JSResizeableVector::Properties() {
    static JSPropertySpec myProperties[] = {
        {"length", PROP_length, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSResizeableVector::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSResizeableVector::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSResizeableVector::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSResizeableVector::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_length:
            *vp = as_jsval(cx, getNative().length());
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSResizeableVector::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool JSResizeableVector::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    *vp = as_jsval(cx, getNative().getElement(theIndex));
    return JS_TRUE;
}

// setproperty handling
JSBool JSResizeableVector::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JS_ReportError(cx,"JSResizeableVector::setPropertySwitch: index %d out of range", theID);
    return JS_FALSE;
}

JSBool JSResizeableVector::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    dom::ValuePtr myArg;
    if (convertFrom(cx, *vp, getNative().elementName(), myArg)) {
        //AC_TRACE << "JSResizeableVector::setPropertyIndex theIndex =" << theIndex << " myArg: " << myArg->getString() << endl;
        bool mySuccess = openNative().setElement(theIndex,*myArg);
        closeNative();
        if (!mySuccess) {
            JS_ReportError(cx, (string("JSResizeableVector::setPropertyIndex:") +
                        " setElement failed: theIndex =" + as_string(theIndex) + " myArg: " +
                          myArg->getString()).c_str());
            return JS_FALSE;
        }
        return JS_TRUE;
    }
    JS_ReportError(cx, (string("JSResizeableVector::setPropertyIndex:")
                        + "convertFrom failed: theIndex = " + as_string(theIndex)).c_str());
    return JS_FALSE;
}

JSBool
JSResizeableVector::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creats a new resizeable vector");
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSResizeableVector * myNewObject = 0;
    if (argc == 0) {
        myNewObject=new JSResizeableVector();
    } else {
        JS_ReportError(cx,"Constructor for %s: superflous argument ignored",ClassName());
        myNewObject=new JSResizeableVector();
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSResizeableVector::Constructor: bad parameters");
    return JS_FALSE;
}


jsval as_jsval(JSContext *cx, dom::ValuePtr theValuePtr, dom::ResizeableVector *) {
    JSObject * myObject = JSResizeableVector::Construct(cx, theValuePtr);
    return OBJECT_TO_JSVAL(myObject);
}
}
