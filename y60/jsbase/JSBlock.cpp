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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $RCSfile: JSBlock.cpp,v $
//   $Author: christian $
//   $Revision: 1.6 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

// own header
#include "JSBlock.h"

#include "JScppUtils.h"
#include "JSWrapper.impl"

#include <asl/base/file_functions.h>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Block, asl::Ptr<Block>, StaticAccessProtocol>;

typedef asl::Block NATIVE;

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

static JSBool
toBase64String(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Converts the block into a string.");
    DOC_END;
    try {
        const ReadableBlock & myBlock = JSBlock::getJSWrapper(cx,obj).getNative();
        std::string myStringRep;
        binToBase64(myBlock, myStringRep, cb64); //cb64 necessary because the default cb66 is depricated [jb]
        *rval = as_jsval(cx, myStringRep);
    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}

static JSBool
resize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Resizes the block (if possible).");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::resize,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSBlock::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"toBase64String",       toBase64String,          0},
        {"resize",               resize,                  1},
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
        myNewObject = new JSBlock(myNewBlock, myNewBlock.get());
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
        myNewObject = new JSBlock(myNewBlock, myNewBlock.get());
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
    JSObject * myReturnObject = JSBlock::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSBlock::OWNERPTR theOwner, JSBlock::NATIVE * theBlock) {
    JSObject * myObject = JSBlock::Construct(cx, theOwner, theBlock);
    return OBJECT_TO_JSVAL(myObject);
}

}
