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
//   $RCSfile: JSBlock.cpp,v $
//   $Author: christian $
//   $Revision: 1.6 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSBlock.h"
#include "JScppUtils.h"
#include "JSWrapper.impl"

#include <asl/file_functions.h>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Block, Ptr<Block>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Converts the block into a string.");
    DOC_END;
    try {
        const ReadableBlock & myBlock = JSBlock::getJSWrapper(cx,obj).getNative();
        std::string myStringRep(myBlock.strbegin(), myBlock.strend());
        *rval = as_jsval(cx, myStringRep);
    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}

JSFunctionSpec *
JSBlock::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSBlock::Properties() {
    static JSPropertySpec myProperties[] = {
        {"size", PROP_size, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"bytes", PROP_bytes, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"capacity", PROP_capacity, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSBlock::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_size:
            *vp = as_jsval(cx, getNative().size());
            return JS_TRUE;
        case PROP_bytes:
            {
                std::vector<asl::Unsigned8> myBytes(getNative().size());
                std::copy(getNative().begin(), getNative().end(), myBytes.begin());
                *vp = as_jsval(cx, myBytes);
            }
            return JS_TRUE;
        case PROP_capacity:
            *vp = as_jsval(cx, getNative().capacity());
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSBlock::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSBlock::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    *vp = as_jsval(cx, getNative()[theIndex]);
    return JS_TRUE;
}

// setproperty handling
JSBool
JSBlock::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_size:
            //jsval dummy;
            //return Method<NATIVE>::call(&NATIVE::isOpen, cx, obj, 1, vp, &dummy);
            return JS_FALSE;
        default:
            JS_ReportError(cx,"JSBlock::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}
JSBool
JSBlock::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    unsigned char myArg;
    if (convertFrom(cx, *vp, myArg)) {
        openNative()[theIndex] = myArg;
        closeNative();
        return JS_TRUE;
    }
    return JS_TRUE;
}


JSBool
JSBlock::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new Block from a file");
    DOC_PARAM("theFileName", "", DOC_TYPE_STRING);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    JSBlock * myNewObject = 0;

    if (argc == 0) {
        OWNERPTR myNewBlock = OWNERPTR(new asl::Block());
        myNewObject = new JSBlock(myNewBlock, &(*myNewBlock));
    } else if (argc == 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"JSBlock::Constructor: bad argument #1 (undefined)");
            return JS_FALSE;
        }

        OWNERPTR myNewBlock = OWNERPTR(new asl::Block());
        std::vector<asl::Unsigned8> myBytes;
        string myFilename = "";

        if (convertFrom(cx, argv[0], myBytes)) {
            myNewBlock->resize(myBytes.size());
            std::copy(myBytes.begin(), myBytes.end(), myNewBlock->begin());
        } else if (convertFrom(cx, argv[0], myFilename)) {
            bool myResult = readFile(myFilename, *myNewBlock);
            if (!myResult) {
                JS_ReportError(cx, "Constructor for %s: Could not read file %s",ClassName(), myFilename.c_str());
                return JS_FALSE;
            }
        } else {
            JS_ReportError(cx, "JSBlock::Constructor: argument #1 must be a string (filename)");
            return JS_FALSE;
        }
        myNewObject = new JSBlock(myNewBlock, &(*myNewBlock));
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1 (filename) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSBlock::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSBlock::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSBlock::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSBlock::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

JSObject *
JSBlock::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_CREATE(JSBlock);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSBlock::NATIVE *& theBlock) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSBlock::NATIVE >::Class()) {
                typedef JSClassTraits<JSBlock::NATIVE>::ScopedNativeRef NativeRef;
                NativeRef myObj(cx, myArgument);
                theBlock = &myObj.getNative();
                return true;
            }
        }
    }
    return false;
}
bool convertFrom(JSContext *cx, jsval theValue, JSBlock::OWNERPTR & theBlock) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSBlock::NATIVE >::Class()) {
                theBlock = JSBlock::getJSWrapper(cx,myArgument).getOwner();
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSBlock::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSBlock::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSBlock::OWNERPTR theOwner, JSBlock::NATIVE * theBlock) {
    JSObject * myObject = JSBlock::Construct(cx, theOwner, theBlock);
    return OBJECT_TO_JSVAL(myObject);
}

}
