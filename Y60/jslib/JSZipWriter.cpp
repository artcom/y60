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
//   $RCSfile: JSZipWriter.cpp,v $
//   $Author: christian $
//   $Revision: 1.6 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSZipWriter.h"
#include "JScppUtils.h"
#include "JSBlock.h"

#include <asl/file_functions.h>

using namespace std;
using namespace asl;
using namespace jslib;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints out some boring info.");
    DOC_END;
    std::string myStringRep = string("JSZipWriter Object");
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
Append(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("Appends data to a file inside the zip file.");
    DOC_PARAM("theString", "A string to append to the zip-file.", DOC_TYPE_STRING);
    DOC_PARAM("theFilename", "The filename inside the zip file to append to.", DOC_TYPE_STRING);
    DOC_RESET;
    DOC_PARAM("theBlock", "A block of data to append to the zip-file.", DOC_TYPE_BLOCK);
    DOC_PARAM("theFilename", "The filename inside the zip file to append to.", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 2);

        asl::ZipWriter * myNative = 0;
        if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
            JS_ReportError(cx,"ZipWriter::append: self is not a ZipWriter");
            return JS_FALSE;
        }

        string myString;
        asl::Block * myBlock = 0;
        if (!convertFrom(cx, argv[0], myBlock) && !convertFrom(cx,argv[0], myString)) {
            JS_ReportError(cx,"ZipWriter::append: 1st argument must be a block or string");
            return JS_FALSE;
        }
        string myFilename;
        if (!convertFrom(cx, argv[1], myFilename)) {
            JS_ReportError(cx,"ZipWriter::append: 2nd argument must be a block");
            return JS_FALSE;
        }
        if ( myBlock ) {
            myNative->append(*myBlock, myFilename);
        } else {
            asl::Block myStringBlock(reinterpret_cast<const unsigned char*>(&(*myString.begin())),
                                     reinterpret_cast<const unsigned char*>(&(*myString.end())));
            myNative->append(myStringBlock, myFilename);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION
}

JSFunctionSpec *
JSZipWriter::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"append",               Append,                  2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSZipWriter::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSZipWriter::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSZipWriter::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSZipWriter::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
            //jsval dummy;
            //return Method<NATIVE>::call(&NATIVE::isOpen, cx, obj, 1, vp, &dummy);
            return JS_FALSE;
        default:
            JS_ReportError(cx,"JSZipWriter::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSZipWriter::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new ZipWriter from a file");
    DOC_PARAM("theFileName", "The name of the output file.", DOC_TYPE_STRING);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    JSZipWriter * myNewObject = 0;

    if (argc == 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"JSZipWriter::Constructor: bad argument #1 (undefined)");
            return JS_FALSE;
        }

        string myFilename = "";
        if (!convertFrom(cx, argv[0], myFilename)) {
            JS_ReportError(cx, "JSZipWriter::Constructor: argument #1 must be a string (filename)");
            return JS_FALSE;
        }

        try {
            OWNERPTR myNewZipWriter = OWNERPTR(new asl::ZipWriter(myFilename));
            myNewObject = new JSZipWriter(myNewZipWriter, &(*myNewZipWriter));
        } HANDLE_CPP_EXCEPTION;
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1 (filename) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSZipWriter::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSZipWriter::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSZipWriter::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSZipWriter::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

JSObject *
JSZipWriter::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_CREATE(JSZipWriter);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSZipWriter::NATIVE *& theZipWriter) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::ZipWriter>::Class()) {
                typedef JSClassTraits<asl::ZipWriter>::ScopedNativeRef NativeRef;
                NativeRef myObj(cx, myArgument);
                theZipWriter = &myObj.getNative();
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSZipWriter::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSZipWriter::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSZipWriter::OWNERPTR theOwner, JSZipWriter::NATIVE * theZipWriter) {
    JSObject * myObject = JSZipWriter::Construct(cx, theOwner, theZipWriter);
    return OBJECT_TO_JSVAL(myObject);
}

}
