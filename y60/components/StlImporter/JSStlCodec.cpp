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

#include "JSStlCodec.h"

#include <asl/zip/PackageManager.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSBlock.h>
#include <y60/jsbase/JSWrapper.impl>

using namespace asl;
using namespace y60;
using namespace std;
using namespace dom;

namespace jslib {

template class JSWrapper<y60::StlCodec, asl::Ptr<y60::StlCodec>, 
         jslib::StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    *rval = as_jsval(cx, "StlCodec");
    return JS_TRUE;
}

static JSBool
exportShape(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<JSStlCodec::NATIVE>::call(&JSStlCodec::NATIVE::exportShape,cx,obj,argc,argv,rval);
}

static JSBool
exportShapes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<JSStlCodec::NATIVE>::call(&JSStlCodec::NATIVE::exportShapes,cx,obj,argc,argv,rval);
}

static JSBool
close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<JSStlCodec::NATIVE>::call(&JSStlCodec::NATIVE::close,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSStlCodec::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"exportShape",          exportShape,             1},
        {"exportShapes",         exportShapes,            2},
        {"close",                close,                   0},
        {0}
    };
    return myFunctions;
}
JSConstIntPropertySpec *
JSStlCodec::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};
JSPropertySpec *
JSStlCodec::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSStlCodec::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSStlCodec::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSStlCodec::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSStlCodec::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSStlCodec::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }
    ensureParamCount(argc, 1, 2);
    OWNERPTR myNewNative;
    if (argc == 1) {
        asl::Ptr<asl::Block> myBlock;
        if (!convertFrom(cx, argv[0], myBlock)) {
            JS_ReportError(cx, "Constructor for %s: first of 1 argument must be a block "
                    ", got %d", ClassName(), argc);
        }
        myNewNative = OWNERPTR(new StlCodec(myBlock));
    } else if (argc == 2) {
        std::string myFilename;
        bool myBigEndianFlag;
        if (!convertFrom(cx, argv[0], myFilename)) {
            JS_ReportError(cx, "Constructor for %s: first of 2 arguments must be a string "
                    "(filepath)", ClassName());
        }
        if (!convertFrom(cx, argv[1], myBigEndianFlag)) {
            JS_ReportError(cx, "Constructor for %s: 2nd on 2 arguments must be a bool "
                    "(bigendian?)", ClassName());
        }
        myNewNative = OWNERPTR(new StlCodec(myFilename, myBigEndianFlag));
    } else {
        JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected one or two "
            "(filepath), got %d", ClassName(), argc);
        return JS_FALSE;
    }

    JSStlCodec * myNewObject = new JSStlCodec(myNewNative, myNewNative.get());
    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}

JSPropertySpec *
JSStlCodec::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSStlCodec::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}


JSObject *
JSStlCodec::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_MODULE_CREATE("Components", JSStlCodec);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSStlCodec::OWNERPTR & theStlCodec) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSStlCodec::NATIVE >::Class()) {
                theStlCodec = JSClassTraits<JSStlCodec::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSStlCodec::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSStlCodec::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSStlCodec::OWNERPTR theOwner, JSStlCodec::NATIVE * theStlCodec) {
    JSObject * myObject = JSStlCodec::Construct(cx, theOwner, theStlCodec);
    return OBJECT_TO_JSVAL(myObject);
}


} // namespace jslib
