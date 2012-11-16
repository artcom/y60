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
#include "JSResizeableVector.h"

#include "JSNode.h"
#include "JSWrapper.impl"

#include <asl/base/string_functions.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

typedef dom::ResizeableVector NATIVE_VECTOR;
//typedef JSWrapper<NATIVE_VECTOR,dom::ValuePtr> Base;

template class JSWrapper<dom::ResizeableVector, dom::ValuePtr, VectorValueAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a string representation of the vector.");
    DOC_END;
    std::string myStringRep = asl::as_string(JSResizeableVector::getJSWrapper(cx,obj).getNative());
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
getItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns an element from the vector. Throws an exception, if index is out of bounds.");
    DOC_PARAM("theIndex", "Index of the element to retrieve.", DOC_TYPE_INTEGER);
    DOC_RVAL("theElement", DOC_TYPE_OBJECT);
    DOC_END;
    typedef dom::ValuePtr (NATIVE_VECTOR::*MyMethod)(asl::AC_SIZE_TYPE) const;
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::getItem,cx,obj,argc,argv,rval);
}

static JSBool
setItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("sets an element in the vector. Throws an exception, if index is out of bounds.");
    DOC_PARAM("theIndex", "Index of the element to set.", DOC_TYPE_INTEGER);
    DOC_PARAM("theElement","", DOC_TYPE_OBJECT);
    DOC_RVAL("success", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        if (argc != 2) {
            JS_ReportError(cx, "setItem(thePos, theElement): needs two arguments");
            return JS_FALSE;
        }

        NativeRef<dom::ResizeableVector> myNativeRef(cx,obj);
        dom::ResizeableVector & myNative = myNativeRef.getValue();
        asl::AC_SIZE_TYPE myArg0;
        dom::ValuePtr myArg1;
        if (convertFrom(cx, argv[0], myArg0)) {
            if (convertFrom(cx, argv[1], myNative.elementName(), myArg1)) {
                myNative.setItem(myArg0, *myArg1);
                return JS_TRUE;
            } else {
                JS_ReportError(cx, (string("JSResizeableVector::setItem:") + "could not convert second argumentvalue "
                                    +as_string(cx, argv[1])+" to type " + myNative.elementName()).c_str());
                return JS_FALSE;
            }
        } else {
            JS_ReportError(cx, (string("JSResizeableVector::setItem:") + "could not convert first argument value "
                                +as_string(cx, argv[0])+" to type unsigned integer").c_str());
            return JS_FALSE;
        }

    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}

static JSBool
resize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Resizes the vector to the requested size, adding default elements or removing superflous elements.");
    DOC_PARAM("theSize", "The new size of the vector", DOC_TYPE_INTEGER);
    DOC_END;
    typedef void (NATIVE_VECTOR::*MyMethod)(unsigned);
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::resize,cx,obj,argc,argv,rval);
}


static JSBool
appendItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("appends an element to the vector");
    DOC_PARAM("theElement", "The element to append", DOC_TYPE_OBJECT);
    DOC_END;
    try {
        if (argc == 0) {
            JS_ReportError(cx, "append(theElement): needs an argument");
            return JS_FALSE;
        }

        NativeRef<dom::ResizeableVector> myNativeRef(cx,obj);
        dom::ResizeableVector & myNative = myNativeRef.getValue();
        dom::ValuePtr myArg;
        if (convertFrom(cx, argv[0], myNative.elementName(), myArg)) {
            myNative.appendItem(*myArg);
        } else {
            JS_ReportError(cx, (string("JSResizeableVector::append:") + "could not convert argument value "
                                +as_string(cx, argv[0])+" to type " + myNative.elementName()).c_str());
            return JS_FALSE;
        }

    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}

static
dom::ValuePtr makeListValue(JSContext *cx, NativeRef<dom::ResizeableVector> & theNativeRef, jsval theArg) {
    try {
        const dom::ValueBase & thisValue = dynamic_cast<const dom::ValueBase &>(theNativeRef.getValue());
        dom::ValuePtr newValue = thisValue.create(as_string(cx, theArg), 0);
        return newValue;
    } catch (dom::ConversionFailed &) {}

    // try argument conversion by string
    std::string myArrayString;
    if (JSA_ArrayToString(cx, &theArg, myArrayString)) {
        const dom::ValueBase & thisValue = dynamic_cast<const dom::ValueBase &>(theNativeRef.getValue());
        return thisValue.create(myArrayString, 0);
    } else {
        return dom::ValuePtr();
    }
}

static JSBool
appendList(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("appends a list of elements to the vector");
    DOC_PARAM("theList", "The list of elements to append", DOC_TYPE_OBJECT);
    DOC_END;
    try {
        if (argc == 0) {
            JS_ReportError(cx, "JSResizeableVector::appendList(theList): needs an argument");
            return JS_FALSE;
        }
        NativeRef<dom::ResizeableVector> myNativeRef(cx,obj);
        if (!JSVAL_IS_OBJECT(argv[0])) {
            JS_ReportError(cx, "JSResizeableVector::appendList(theList): argument is not an object");
            return JS_FALSE;
        }
        try {
            const dom::ResizeableVector & myVector = JSClassTraits<dom::ResizeableVector>::getNativeRef(cx, JSVAL_TO_OBJECT(argv[0]));
            const dom::ValueBase & myVectorValue = dynamic_cast<const dom::ValueBase &>(myVector);
            myNativeRef.getValue().appendList(myVectorValue);
            return JS_TRUE;
        } catch (jslib::BadJSNative &) {}

        dom::ValuePtr myNewValue = makeListValue(cx, myNativeRef, argv[0]);
        if (myNewValue) {
            myNativeRef.getValue().appendList(*myNewValue);
        } else {
            JS_ReportError(cx, "JSResizeableVector::appendList(theList): argument conversion failed");
            return JS_FALSE;
        }

    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}

static JSBool
eraseItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Erases one element from the vector");
    DOC_PARAM("theIndex", "The index of the element to erase", DOC_TYPE_INTEGER);
    DOC_END;
    typedef void (NATIVE_VECTOR::*MyMethod)(asl::AC_SIZE_TYPE);
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::eraseItem,cx,obj,argc,argv,rval);
}
static JSBool
eraseList(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Erases elements from the vector");
    DOC_PARAM("theIndex", "The index of the element to erase", DOC_TYPE_INTEGER);
    DOC_PARAM("theCount", "The count of the element to erase", DOC_TYPE_INTEGER);
    DOC_END;
    typedef void (NATIVE_VECTOR::*MyMethod)(asl::AC_SIZE_TYPE,asl::AC_SIZE_TYPE);
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::eraseList,cx,obj,argc,argv,rval);
}

static JSBool
insertItemBefore(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("insertItemBefore inserts an element in the vector before thePosition");
    DOC_PARAM("thePosition", "The position to insert before", DOC_TYPE_INTEGER);
    DOC_PARAM("theElement", "The element to insert", DOC_TYPE_OBJECT);
    DOC_END;
    try {
        if (argc != 2) {
            JS_ReportError(cx, "insertItemBefore(thePos, theElement): needs two arguments");
            return JS_FALSE;
        }

        NativeRef<dom::ResizeableVector> myNativeRef(cx,obj);
        dom::ResizeableVector & myNative = myNativeRef.getValue();
        unsigned int myArg0;
        dom::ValuePtr myArg1;
        if (convertFrom(cx, argv[0], myArg0)) {
            if (convertFrom(cx, argv[1], myNative.elementName(), myArg1)) {
                myNative.insertItemBefore(myArg0, *myArg1);
            } else {
                JS_ReportError(cx, (string("JSResizeableVector::insertItemBefore:") + "could not convert second argumentvalue "
                                    +as_string(cx, argv[1])+" to type " + myNative.elementName()).c_str());
                return JS_FALSE;
            }
        } else {
            JS_ReportError(cx, (string("JSResizeableVector::insertItemBefore:") + "could not convert first argument value "
                                +as_string(cx, argv[0])+" to type unsigned integer").c_str());
            return JS_FALSE;
        }

    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}
static JSBool
insertListBefore(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("insertListBefore insert a list of elements in the vector before thePosition");
    DOC_PARAM("thePosition", "The position to insert before", DOC_TYPE_INTEGER);
    DOC_PARAM("theList", "The list of elements to insert", DOC_TYPE_OBJECT);
    DOC_END;
    try {
        if (argc != 2) {
            JS_ReportError(cx, "insertListBefore(thePos, theList): needs two arguments");
            return JS_FALSE;
        }

        asl::AC_SIZE_TYPE thePos;
        dom::ValuePtr myArg1;
        if (convertFrom(cx, argv[0], thePos)) {
            NativeRef<dom::ResizeableVector> myNativeRef(cx,obj);
            if (!JSVAL_IS_OBJECT(argv[1])) {
                JS_ReportError(cx, "JSResizeableVector::insertListBefore(thePos, theList): argument #2 is not an object");
                return JS_FALSE;
            }

            try {
                const dom::ResizeableVector & myVector = JSClassTraits<dom::ResizeableVector>::getNativeRef(cx, JSVAL_TO_OBJECT(argv[1]));
                const dom::ValueBase & myVectorValue = dynamic_cast<const dom::ValueBase &>(myVector);
                myNativeRef.getValue().insertListBefore(thePos, myVectorValue);
                return JS_TRUE;
            } catch (jslib::BadJSNative &) {}

            dom::ValuePtr myNewValue = makeListValue(cx, myNativeRef, argv[1]);
            if (myNewValue) {
                myNativeRef.getValue().insertListBefore(thePos, *myNewValue);
            } else {
                JS_ReportError(cx, "JSResizeableVector::insertListBefore(thePos, theList): argument conversion failed");
                return JS_FALSE;
            }
        } else {
            JS_ReportError(cx, (string("JSResizeableVector::insertListBefore(thePos, theList):") + "could not convert first argument value "
                                +as_string(cx, argv[0])+" to type unsigned integer").c_str());
            return JS_FALSE;
        }

    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}
static JSBool
setList(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("setList overwrites a list of elements in the vector starting at thePosition");
    DOC_PARAM("thePosition", "The first  position to overwrite", DOC_TYPE_INTEGER);
    DOC_PARAM("theList", "The list of elements", DOC_TYPE_OBJECT);
    DOC_END;
    try {
        if (argc != 2) {
            JS_ReportError(cx, "JSResizeableVector::setList(thePos, theList): needs two arguments");
            return JS_FALSE;
        }

        asl::AC_SIZE_TYPE thePos;
        dom::ValuePtr myArg1;
        if (convertFrom(cx, argv[0], thePos)) {
            NativeRef<dom::ResizeableVector> myNativeRef(cx,obj);
            if (!JSVAL_IS_OBJECT(argv[1])) {
                JS_ReportError(cx, "JSResizeableVector::setList(thePos, theList): argument #2 is not an object");
                return JS_FALSE;
            }

            try {
                const dom::ResizeableVector & myVector = JSClassTraits<dom::ResizeableVector>::getNativeRef(cx, JSVAL_TO_OBJECT(argv[1]));
                const dom::ValueBase & myVectorValue = dynamic_cast<const dom::ValueBase &>(myVector);
                myNativeRef.getValue().setList(thePos, myVectorValue);
                return JS_TRUE;
            } catch (jslib::BadJSNative &) {}

            dom::ValuePtr myNewValue = makeListValue(cx, myNativeRef, argv[1]);
            if (myNewValue) {
                myNativeRef.getValue().setList(thePos, *myNewValue);
            } else {
                JS_ReportError(cx, "JSResizeableVector::setList(thePos, theList): argument conversion failed");
                return JS_FALSE;
            }
        } else {
            JS_ReportError(cx, (string("JSResizeableVector::setList(thePos, theList):") + "could not convert first argument value "
                                +as_string(cx, argv[0])+" to type unsigned integer").c_str());
            return JS_FALSE;
        }

    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}

static JSBool
getList(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("gets elements from the vector");
    DOC_PARAM("theIndex", "The start index of the elements to return", DOC_TYPE_INTEGER);
    DOC_PARAM("theCount", "The count of elements to return", DOC_TYPE_INTEGER);
    DOC_RVAL("the requested list", DOC_TYPE_OBJECT);
    DOC_END;
    typedef dom::ValuePtr (NATIVE_VECTOR::*MyMethod)(asl::AC_SIZE_TYPE,asl::AC_SIZE_TYPE);
    return Method<NATIVE_VECTOR>::call((MyMethod)&NATIVE_VECTOR::getList,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSResizeableVector::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                  native          nargs    */
        {"toString",             toString,        0},
        {"getItem",              getItem,         1},
        {"setItem",              setItem,         2},
        {"setList",              setList,         2},
        {"getList",              getList,         2},
        {"resize",               resize,          1},
        {"appendItem",           appendItem,      1},
        {"appendList",           appendList,      1},
        {"eraseItem",            eraseItem,       1},
        {"eraseList",            eraseList,       2},
        {"insertItemBefore",     insertItemBefore,    2},
        {"insertListBefore",     insertListBefore,    2},
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
    try {
        *vp = as_jsval(cx, getNative().getItem(theIndex));
    } HANDLE_CPP_EXCEPTION;
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
        NativeRef<dom::ResizeableVector> myNativeRef(cx,obj);
        try {
            myNativeRef.getValue().setItem(theIndex,*myArg);
        } HANDLE_CPP_EXCEPTION;
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
